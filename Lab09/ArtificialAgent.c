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

static char tempMes[PROTOCOL_MAX_PAYLOAD_LEN];
static int fieldTurn;

static AgentState agentState = AGENT_STATE_GENERATE_NEG_DATA;
static GuessData *guessData, *enemyGuessData;
static NegotiationData *negotiationData, *enemyNegotiationData;
static Field *playerField;
static Field *enemyField;

void AgentInit(void)
{
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

int AgentRun(char in, char *outBuffer)
{
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

uint8_t AgentGetStatus(void)
{
    return FieldGetBoatStates(playerField);
}

uint8_t AgentGetEnemyStatus(void)
{
    return FieldGetBoatStates(enemyField);
}

//// Libraries
//#include "Agent.h"
//#include "BOARD.h"
//#include "Field.h"
//#include "FieldOled.h"
//#include "Oled.h"
//
//// defines
//#define FIELD_DIRS 4
//
//// Static Variables
//static NegotiationData *nData, *opp_nData;
//static GuessData *gData, *opp_gData;
//static char message[PROTOCOL_MAX_MESSAGE_LEN];
//static int agentState;
//
//static int currentTurn;
//static Field *AgentField;
//static Field *EnemyField;
//
///***********************************************************************************/
//
//void AgentInit(void)
//{
//    // allocating data for structs
//    opp_nData = malloc(sizeof (NegotiationData));
//    nData = malloc(sizeof (NegotiationData));
//    opp_gData = malloc(sizeof (GuessData));
//    gData = malloc(sizeof (GuessData));
//    
//    AgentField = malloc(sizeof (Field));
//    EnemyField = malloc(sizeof (Field));
//
//    uint8_t randRow, randCol, placementFlag = FALSE;
//    int randDir, boatSize;
//
//    // sets Agent field empty and Enemy field unknown
//    FieldInit(AgentField, FIELD_POSITION_EMPTY);
//    FieldInit(EnemyField, FIELD_POSITION_UNKNOWN);
//
//    // individually placing boats
//    for (boatSize = FIELD_BOAT_SMALL; boatSize <= FIELD_BOAT_HUGE; boatSize++) {
//        // keep attempting until placed
//        while (placementFlag != TRUE) {
//            randRow = (rand() % (FIELD_ROWS));
//            randCol = (rand() % (FIELD_COLS));
//            randDir = (rand() % (FIELD_DIRS));
//            // true if placed
//            placementFlag = FieldAddBoat(AgentField, randRow, randCol, randDir, boatSize);
//        }
//        // continue for loop
//        placementFlag = FALSE;
//    }
//}
//
//int AgentRun(char in, char *outBuffer)
//{
//    int dataLength = 0, decodeStatus = 0, turnCheck, timer = 0;
//    uint8_t validate;
//
//    // if char input, decode
//    if (in != 0) {
//        decodeStatus = ProtocolDecode(in, opp_nData, opp_gData);
//    }
//
//    switch (agentState) {
//
//    case AGENT_STATE_GENERATE_NEG_DATA:
//        // generates and sends negotiation data to determine first turn
//        ProtocolGenerateNegotiationData(nData);
//        dataLength = ProtocolEncodeChaMessage(message, nData);
//        strncpy(outBuffer, message, dataLength);
//        agentState = AGENT_STATE_SEND_CHALLENGE_DATA;
//        return dataLength;
//
//    case AGENT_STATE_SEND_CHALLENGE_DATA:
//        // wait for challenge message
//        if (decodeStatus == PROTOCOL_PARSED_CHA_MESSAGE) {
//            dataLength = ProtocolEncodeDetMessage(message, nData);
//            strncpy(outBuffer, message, dataLength);
//            agentState = AGENT_STATE_DETERMINE_TURN_ORDER;
//        }
//        return dataLength;
//        break;
//
//    case AGENT_STATE_DETERMINE_TURN_ORDER:
//        // wait for determine message
//        if (decodeStatus == PROTOCOL_PARSED_DET_MESSAGE) {
//            // check if valid data
//            validate = ProtocolValidateNegotiationData(opp_nData);
//            // if not valid, error
//            if (validate == FALSE) {
//                OledClear(OLED_COLOR_BLACK);
//                OledDrawString(AGENT_ERROR_STRING_NEG_DATA);
//                OledUpdate();
//                agentState = AGENT_STATE_INVALID;
//            } else {
//                // otherwise, check first turn
//                turnCheck = ProtocolGetTurnOrder(nData, opp_nData);
//                if (turnCheck == TURN_ORDER_START) {
//                    // our Agent goes first
//                    currentTurn = FIELD_OLED_TURN_MINE;
//                    FieldOledDrawScreen(AgentField, EnemyField, currentTurn);
//                    agentState = AGENT_STATE_SEND_GUESS;
//                } else if (turnCheck == TURN_ORDER_DEFER) {
//                    // opponent starts
//                    currentTurn = FIELD_OLED_TURN_THEIRS;
//                    FieldOledDrawScreen(AgentField, EnemyField, currentTurn);
//                    agentState = AGENT_STATE_WAIT_FOR_GUESS;
//                } else {
//                    // if guess tie, error
//                    OledClear(OLED_COLOR_BLACK);
//                    OledDrawString(AGENT_ERROR_STRING_NEG_DATA);
//                    OledUpdate();
//                    agentState = AGENT_STATE_INVALID;
//                }
//            }
//        }
//        return dataLength;
//        break;
//
//    case AGENT_STATE_SEND_GUESS:
//        // timer to seem like artificalAgent is thinking
//        for(timer = 0; timer < (BOARD_GetPBClock() / 8); timer++);
//        // generate heuristic guess
//        while (EnemyField->field[gData->row][gData->col] != FIELD_POSITION_UNKNOWN) {
//            gData->row = rand() % FIELD_ROWS;
//            gData->col = rand() % FIELD_COLS;
//        }
//        // send guess message
//        dataLength = ProtocolEncodeCooMessage(message, gData);
//        strncpy(outBuffer, message, dataLength);
//        agentState = AGENT_STATE_WAIT_FOR_HIT;
//        return dataLength;
//        break;
//
//    case AGENT_STATE_WAIT_FOR_HIT:
//        // wait for opponent response to guess
//        if (decodeStatus == PROTOCOL_PARSED_HIT_MESSAGE) {
//            // update our view of enemy field
//            FieldUpdateKnowledge(EnemyField, opp_gData);
//            // if they still have boat(s), opponents turn
//            if (AgentGetEnemyStatus() != 0) {
//                currentTurn = FIELD_OLED_TURN_THEIRS;
//                agentState = AGENT_STATE_WAIT_FOR_GUESS;
//                // if enemy boats are all sunk, win
//            } else if (AgentGetEnemyStatus() == 0) {
//                currentTurn = FIELD_OLED_TURN_NONE;
//                agentState = AGENT_STATE_WON;
//            }
//            // update screen
//            FieldOledDrawScreen(AgentField, EnemyField, currentTurn);
//        }
//        return dataLength;
//        break;
//
//    case AGENT_STATE_WAIT_FOR_GUESS:
//        // wait for coordinate message
//        if (decodeStatus == PROTOCOL_PARSED_COO_MESSAGE) {
//            // add attack to our field
//            FieldRegisterEnemyAttack(AgentField, opp_gData);
//            // check if boats left
//            if (AgentGetStatus() != 0) {
//                currentTurn = FIELD_OLED_TURN_MINE;
//                agentState = AGENT_STATE_SEND_GUESS;
//            } else if (AgentGetStatus() == 0) {
//                // if none, lose
//                currentTurn = FIELD_OLED_TURN_NONE;
//                FieldOledDrawScreen(AgentField, EnemyField, currentTurn);
//                agentState = AGENT_STATE_LOST;
//            }
//            // update screen
//            FieldOledDrawScreen(AgentField, EnemyField, currentTurn);
//            dataLength = ProtocolEncodeHitMessage(message, opp_gData);
//            strncpy(outBuffer, message, dataLength);
//        }
//        return dataLength;
//        break;
//
//    case AGENT_STATE_WON:
//        // endless 0s
//        return dataLength;
//        break;
//
//    case AGENT_STATE_LOST:
//        // endless 0s
//        return dataLength;
//        break;
//
//    case AGENT_STATE_INVALID:
//        FATAL_ERROR();
//        return dataLength;
//        break;
//    }
//    return dataLength;
//}
//
//uint8_t AgentGetStatus(void)
//{
//    uint8_t shipStates;
//    // get boat status
//    shipStates = FieldGetBoatStates(AgentField);
//    return shipStates;
//}
//
//uint8_t AgentGetEnemyStatus(void)
//{
//    uint8_t shipStates;
//    // get boat status
//    shipStates = FieldGetBoatStates(EnemyField);
//    return shipStates;
//}
