/*
 * File:   ArtificialAgent.c
 * Author: Kenneth Mai
 *
 * Created on March 8, 2018, 5:47 PM
 */

//Mircochip/user libraries
#include <xc.h>
#include "Field.h"
#include "FieldOled.h"
#include "Protocol.h"
#include "Agent.h"
#include "Oled.h"

static char maxMessage[PROTOCOL_MAX_MESSAGE_LEN];
static AgentState agentState;
static FieldOledTurn fieldTurn;
static Field playerField;
static Field enemyField;
static GuessData guessData, enemyGuessData;
static NegotiationData negotiationData, enemyNegotiationData;

void AgentInit(void) {
    // initialization of the field
    FieldInit(&playerField, FIELD_POSITION_EMPTY);
    FieldInit(&enemyField, FIELD_POSITION_UNKNOWN);

    while (!FieldAddBoat(&playerField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_SMALL));
    while (!FieldAddBoat(&playerField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_MEDIUM));
    while (!FieldAddBoat(&playerField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_LARGE));
    while (!FieldAddBoat(&playerField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_HUGE));

    OledClear(OLED_COLOR_BLACK);
    FieldOledDrawScreen(&playerField, &enemyField, fieldTurn);
    OledUpdate();
}

int AgentRun(char in, char *outBuffer) {
    while (1) {
        // would we need two negotiationData and two guessData?
        int protocolDecode = ProtocolDecode(in, &negotiationData, &guessData);
        int enemyProtocolDecode = ProtocolDecode(in, &enemyNegotiationData, &enemyGuessData);

        switch (agentState) {
        
            default:
                break;


            // Start everything up...
            case AGENT_STATE_GENERATE_NEG_DATA:
                ProtocolGenerateNegotiationData(&negotiationData);
                agentState = AGENT_STATE_SEND_CHALLENGE_DATA;
                return ProtocolEncodeChaMessage(outBuffer, &negotiationData);
            
            case AGENT_STATE_SEND_CHALLENGE_DATA:
                agentState = AGENT_STATE_DETERMINE_TURN_ORDER;
                if(enemyProtocolDecode == PROTOCOL_PARSED_CHA_MESSAGE) {
                    return ProtocolEncodeDetMessage(outBuffer, &negotiationData);
                }
                return 0;
                
            case AGENT_STATE_DETERMINE_TURN_ORDER:
                if(ProtocolValidateNegotiationData(&enemyNegotiationData)) {
                    TurnOrder whoseTurn = ProtocolGetTurnOrder(&negotiationData, &enemyNegotiationData);
                    switch(whoseTurn) {
                        case TURN_ORDER_TIE:
                            OledClear(OLED_COLOR_BLACK);
                            OledDrawString(AGENT_ERROR_STRING_ORDERING);
                            OledUpdate();
                            agentState = AGENT_STATE_INVALID;
                            break;
                        case TURN_ORDER_DEFER:
                            fieldTurn = FIELD_OLED_TURN_THEIRS;
                            FieldOledDrawScreen(&playerField, &enemyField, fieldTurn);
                            agentState = AGENT_STATE_WAIT_FOR_GUESS;
                            break;
                        case TURN_ORDER_START:
                            fieldTurn = FIELD_OLED_TURN_MINE;
                            FieldOledDrawScreen(&playerField, &enemyField, fieldTurn);
                            agentState = AGENT_STATE_SEND_GUESS;
                            break;
                        default:
                            OledClear(OLED_COLOR_BLACK);
                            OledDrawString(AGENT_ERROR_STRING_NEG_DATA);
                            agentState = AGENT_STATE_INVALID;
                            break;
                    }
                }
                break;
                
            case AGENT_STATE_INVALID:
                return 0;
            case AGENT_STATE_WAIT_FOR_GUESS:

                if (protocolDecode == PROTOCOL_PARSED_COO_MESSAGE) {

                    uint8_t agentStatus = AgentGetStatus();

                    //If lives are remaining -> SEND_GUESS
                    if (agentStatus != 0) {
                        fieldTurn = FIELD_OLED_TURN_MINE;
                        FieldOledDrawScreen(&playerField, &enemyField, fieldTurn);
                        OledUpdate();
                        strcpy(outBuffer, maxMessage);
                        agentState = AGENT_STATE_SEND_GUESS;

                        //If lives = 0 -> LOST
                    } else if (agentStatus == 0) {
                        fieldTurn = FIELD_OLED_TURN_NONE;
                        FieldOledDrawScreen(&playerField, &enemyField, fieldTurn);
                        OledUpdate();
                        strcpy(outBuffer, maxMessage);
                        agentState = AGENT_STATE_LOST;
                    }
                }
                return ProtocolEncodeHitMessage(maxMessage, &guessData);
                break;

            case AGENT_STATE_SEND_GUESS:

                //Generate valid coordinates
                while (guessData.row >= FIELD_ROWS - 1) {
                    guessData.row = rand();
                }

                while (guessData.col >= FIELD_COLS - 1) {
                    guessData.col = rand();
                }
                guessData.hit = FieldAt(&enemyField, guessData.row, guessData.col);
                strcpy(outBuffer, maxMessage);
                agentState = AGENT_STATE_WAIT_FOR_HIT;
                return ProtocolEncodeCooMessage(maxMessage, &guessData);
                break;

            case AGENT_STATE_WAIT_FOR_HIT:

                if (protocolDecode == PROTOCOL_PARSED_HIT_MESSAGE) {

                    //Enemy has not yet been defeated -> WAIT_FOR_GUESS(other players turn)
                    if (AgentGetEnemyStatus() != 0) {
                        FieldUpdateKnowledge(&enemyField, &guessData);
                        fieldTurn = FIELD_OLED_TURN_THEIRS;
                        FieldOledDrawScreen(&playerField, &enemyField, fieldTurn);
                        OledUpdate();
                        agentState = AGENT_STATE_WAIT_FOR_GUESS;


                        //If enemy lives reaches 0 -> WON    
                    } else if (AgentGetEnemyStatus() == 0) {
                        FieldUpdateKnowledge(&enemyField, &guessData);
                        fieldTurn = FIELD_OLED_TURN_NONE;
                        FieldOledDrawScreen(&playerField, &enemyField, fieldTurn);
                        OledUpdate();
                        agentState = AGENT_STATE_WON;
                    }
                }
                break;

            case AGENT_STATE_LOST:
                //Lost
                break;

            case AGENT_STATE_WON:
                //Won
                break;

        }
    }
}

uint8_t AgentGetStatus(void) {
    return FieldGetBoatStates(&playerField);
}

uint8_t AgentGetEnemyStatus(void) {
    return FieldGetBoatStates(&enemyField);
}