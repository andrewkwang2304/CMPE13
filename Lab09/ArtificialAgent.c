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
static GuessData guessData;
static NegotiationData negotiationData;

int AgentRun(char in, char *outBuffer)
{
    while (1) {

        int protocolDecode = ProtocolDecode(in, &negotiationData, &guessData);
        
        switch (agentState) {
            
        default:
            break;

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