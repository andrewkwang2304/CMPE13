/*
 * File:   ArtificialAgent.c
 * Author: Kenneth Mai
 *         Andrew Kwang
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
#include "Buttons.h"

static char tempMes[PROTOCOL_MAX_PAYLOAD_LEN];
static int fieldTurn;

static AgentState agentState = AGENT_STATE_GENERATE_NEG_DATA;
static GuessData *guessData, *enemyGuessData;
static NegotiationData *negotiationData, *enemyNegotiationData;
static Field *playerField;
static Field *enemyField;

void AgentInit(void) {
    negotiationData = malloc(sizeof (NegotiationData));
    guessData = malloc(sizeof (GuessData));
    enemyNegotiationData = malloc(sizeof (NegotiationData));
    enemyGuessData = malloc(sizeof (GuessData));

    playerField = malloc(sizeof (Field));
    enemyField = malloc(sizeof (Field));

    // initialization of the field
    FieldInit(playerField, FIELD_POSITION_EMPTY);
    FieldInit(enemyField, FIELD_POSITION_UNKNOWN);

    while (!FieldAddBoat(playerField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_SMALL));
    while (!FieldAddBoat(playerField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_MEDIUM));
    while (!FieldAddBoat(playerField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_LARGE));
    while (!FieldAddBoat(playerField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_HUGE));
}

int AgentRun(char in, char *outBuffer) {
    int protocolDecode = 0;
    int timeDelay = 0;
    int messageString = 0;

    if (in != NULL) {
        protocolDecode = ProtocolDecode(in, enemyNegotiationData, enemyGuessData);
    }

    switch (agentState) {

            // Start everything up...
        case AGENT_STATE_GENERATE_NEG_DATA:
            ProtocolGenerateNegotiationData(negotiationData);
            messageString = ProtocolEncodeChaMessage(tempMes, negotiationData);
            strncpy(outBuffer, tempMes, messageString);
            agentState = AGENT_STATE_SEND_CHALLENGE_DATA;
            return messageString;
            break;

        case AGENT_STATE_SEND_CHALLENGE_DATA:
            if (protocolDecode == PROTOCOL_PARSED_CHA_MESSAGE) {
                messageString = ProtocolEncodeDetMessage(tempMes, negotiationData);
                strncpy(outBuffer, tempMes, messageString);
                agentState = AGENT_STATE_DETERMINE_TURN_ORDER;
            }
            return messageString;
            break;

        case AGENT_STATE_DETERMINE_TURN_ORDER:
            if (protocolDecode == PROTOCOL_PARSED_DET_MESSAGE) {

                if (ProtocolValidateNegotiationData(enemyNegotiationData)) {
                    TurnOrder whoseTurn = ProtocolGetTurnOrder(negotiationData, enemyNegotiationData);
                    switch (whoseTurn) {
                        case TURN_ORDER_DEFER:
                            fieldTurn = FIELD_OLED_TURN_THEIRS;
                            FieldOledDrawScreen(playerField, enemyField, fieldTurn);
                            agentState = AGENT_STATE_WAIT_FOR_GUESS;
                            break;
                        case TURN_ORDER_START:
                            fieldTurn = FIELD_OLED_TURN_MINE;
                            FieldOledDrawScreen(playerField, enemyField, fieldTurn);
                            agentState = AGENT_STATE_SEND_GUESS;
                            break;
                        case TURN_ORDER_TIE:
                            OledClear(OLED_COLOR_BLACK);
                            OledDrawString(AGENT_ERROR_STRING_ORDERING);
                            OledUpdate();
                            agentState = AGENT_STATE_INVALID;
                            break;
                        default:
                            OledClear(OLED_COLOR_BLACK);
                            OledDrawString(AGENT_ERROR_STRING_NEG_DATA);
                            OledUpdate();
                            agentState = AGENT_STATE_INVALID;
                            break;
                    }
                }
            }
            return messageString;
            break;

        case AGENT_STATE_SEND_GUESS:
            //Generate valid coordinates        
            while (enemyField->field[guessData->row][guessData->col] != FIELD_POSITION_UNKNOWN) {
                guessData->row = rand() % FIELD_ROWS;
                guessData->col = rand() % FIELD_COLS;
            }

            messageString = ProtocolEncodeCooMessage(tempMes, guessData);
            strncpy(outBuffer, tempMes, messageString);
            agentState = AGENT_STATE_WAIT_FOR_HIT;

            //Delayed thinking
            for (timeDelay = 0; timeDelay <= BOARD_GetPBClock() / 8; timeDelay++);
            return messageString;
            break;

        case AGENT_STATE_WAIT_FOR_GUESS:
            if (protocolDecode == PROTOCOL_PARSED_COO_MESSAGE) {

                uint8_t agentStatus = AgentGetStatus();
                FieldRegisterEnemyAttack(playerField, enemyGuessData);
                //If lives are remaining -> SEND_GUESS
                if (agentStatus != 0) {
                    fieldTurn = FIELD_OLED_TURN_MINE;
                    FieldOledDrawScreen(playerField, enemyField, fieldTurn);
                    messageString = ProtocolEncodeHitMessage(tempMes, enemyGuessData);
                    strncpy(outBuffer, tempMes, messageString);
                    agentState = AGENT_STATE_SEND_GUESS;

                    //If lives = 0 -> LOST
                } else if (agentStatus == 0) {
                    fieldTurn = FIELD_OLED_TURN_NONE;
                    FieldOledDrawScreen(playerField, enemyField, fieldTurn);
                    messageString = ProtocolEncodeHitMessage(tempMes, enemyGuessData);
                    strncpy(outBuffer, tempMes, messageString);
                    agentState = AGENT_STATE_LOST;
                } else {
                    FieldOledDrawScreen(playerField, enemyField, fieldTurn);
                }
            }
            return messageString;
            break;

        case AGENT_STATE_WAIT_FOR_HIT:
            if (protocolDecode == PROTOCOL_PARSED_HIT_MESSAGE) {

                FieldUpdateKnowledge(enemyField, enemyGuessData);

                //Enemy has not yet been defeated -> WAIT_FOR_GUESS(other players turn)
                if (AgentGetEnemyStatus() != 0) {
                    fieldTurn = FIELD_OLED_TURN_THEIRS;
                    agentState = AGENT_STATE_WAIT_FOR_GUESS;

                    //If enemy lives reaches 0 -> WON    
                } else if (AgentGetEnemyStatus() == 0) {
                    fieldTurn = FIELD_OLED_TURN_NONE;
                    agentState = AGENT_STATE_WON;
                }
                FieldOledDrawScreen(playerField, enemyField, fieldTurn);
            }
            return messageString;
            break;

        case AGENT_STATE_LOST:
            //Lost
            break;

        case AGENT_STATE_WON:
            //Won
            break;

        case AGENT_STATE_INVALID:
            //break out of program. not specified where to go in lab manual
            break;
    }
    return messageString;
}

uint8_t AgentGetStatus(void) {
    return FieldGetBoatStates(playerField);
}

uint8_t AgentGetEnemyStatus(void) {
    return FieldGetBoatStates(enemyField);
}
