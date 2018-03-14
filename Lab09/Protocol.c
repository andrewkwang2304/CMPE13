/*
 * File:   Protocol.c
 * Author: Kenneth Mai
 *
 * Created on March 8, 2018, 5:46 PM
 */

//Microchip/User Libraries 
#include "BOARD.h"
#include "Protocol.h"
#include "Agent.h"
#include <xc.h>
#include <stdint.h>
#include <time.h>

//Declared Function Prototypes
static int ConvertAscii(char asciiChar);
static uint8_t CheckSum(char *stringChecksum);

//Defines/static Variables
#define TRUE 1
#define FALSE 0
#define maxTemplateMessage 4
static uint8_t checkSumMatch;
static char *parseMessage[maxTemplateMessage];
static char *messageTok;
static int messageTokCount = 0;

typedef enum {
    WAITING,
    RECORDING,
    FIRST_CHECKSUM_HALF,
    SECOND_CHECKSUM_HALF,
    NEWLINE,
} Prot_State;

typedef struct {
    char maxMessage[PROTOCOL_MAX_PAYLOAD_LEN];
    int arrayIndex;
    Prot_State protocolState;
    uint8_t checksum;
} Prot_Struct;

Prot_Struct protStruct;

/*Protocol.h functions*************************************************************************/

ProtocolParserStatus ProtocolDecode(char in, NegotiationData *nData, GuessData *gData)
{      
    while (1) {

        switch (protStruct.protocolState) {

        case WAITING:
            if (in == '$') {
                protStruct.arrayIndex = 0;
                protStruct.protocolState = RECORDING;
                return PROTOCOL_PARSING_GOOD;
            } else {
                protStruct.protocolState = WAITING;
                return PROTOCOL_WAITING;
            }
            break;

        case RECORDING:
            if(in == '*') {
                protStruct.protocolState = FIRST_CHECKSUM_HALF;
                return PROTOCOL_PARSING_GOOD;
            } else {
                protStruct.maxMessage[protStruct.arrayIndex] = in;
                protStruct.arrayIndex += 1;
                return PROTOCOL_PARSING_GOOD;
            }
            break;

        case FIRST_CHECKSUM_HALF:
            checkSumMatch = (((protStruct.checksum) | (ConvertAscii(in) & 0x0F)) -
                    CheckSum(protStruct.maxMessage));

            if (ConvertAscii(in)) {
                protStruct.checksum = ConvertAscii(in);
                protStruct.checksum = protStruct.checksum << 4;
                protStruct.protocolState = SECOND_CHECKSUM_HALF;
                return PROTOCOL_PARSING_GOOD;
            } else if (ConvertAscii(in) == FALSE || checkSumMatch != 0) {
                protStruct.protocolState = WAITING;
                return PROTOCOL_PARSING_FAILURE;
            }
            break;

        case SECOND_CHECKSUM_HALF:
            checkSumMatch = (((protStruct.checksum) | (ConvertAscii(in) & 0x0F)) -
                    CheckSum(protStruct.maxMessage));

            if ((ConvertAscii(in) == TRUE) && (checkSumMatch == 0)) {
                protStruct.maxMessage[protStruct.arrayIndex] = '\0';
                protStruct.protocolState = NEWLINE;
                return PROTOCOL_PARSING_GOOD;
            } else if ((ConvertAscii(in) == FALSE) || (protStruct.checksum - nData->hash != 0)) {
                protStruct.protocolState = WAITING;
                return PROTOCOL_PARSING_FAILURE;
            }
            break;

        case NEWLINE:
            //Parse Message [message, %u...]
            messageTok = strtok(protStruct.maxMessage, ",");
            while (messageTok != NULL) {
                parseMessage[messageTokCount] = messageTok;
                messageTok = strtok(NULL, ",");
                messageTokCount++;
            }

            if (in == '\n') {
                if (strcmp(parseMessage[0], "DET") == 0) {
                    nData->guess = atoi(parseMessage[1]);
                    nData->encryptionKey = atoi(parseMessage[2]);
                    return PROTOCOL_PARSED_DET_MESSAGE;
                } else if (strcmp(parseMessage[0], "CHA") == 0) {
                    nData->encryptedGuess = atoi(parseMessage[1]);
                    nData->hash = atoi(parseMessage[2]);
                    return PROTOCOL_PARSED_CHA_MESSAGE;
                } else if (strcmp(parseMessage[0], "COO") == 0) {
                    gData->row = atoi(parseMessage[1]);
                    gData->col = atoi(parseMessage[2]);
                    return PROTOCOL_PARSED_COO_MESSAGE;
                } else if (strcmp(parseMessage[0], "HIT") == 0) {
                    gData->row = atoi(parseMessage[1]);
                    gData->col = atoi(parseMessage[2]);
                    gData->hit = atoi(parseMessage[3]);
                    return PROTOCOL_PARSED_HIT_MESSAGE;
                }
                protStruct.protocolState = WAITING;
                
            } else if (in != '\n' || strcmp(parseMessage[0], "DET") == 1 || 
                    strcmp(parseMessage[0], "CHA") == 1 || strcmp(parseMessage[0], "COO") == 1 ||
                    strcmp(parseMessage[0], "HIT") == 1) {
                protStruct.protocolState = WAITING;
                return PROTOCOL_PARSING_FAILURE;
            }
            break;    
            
        default:
            break;
        }
    }
}

int ProtocolEncodeCooMessage(char *message, const GuessData *data)
{
    sprintf(protStruct.maxMessage, PAYLOAD_TEMPLATE_COO, data->row, data->col);
    protStruct.checksum = CheckSum(protStruct.maxMessage);
    sprintf(message, MESSAGE_TEMPLATE, protStruct.maxMessage, protStruct.checksum);
    return strlen(message);
}

int ProtocolEncodeHitMessage(char *message, const GuessData *data)
{
    sprintf(protStruct.maxMessage, PAYLOAD_TEMPLATE_HIT, data->row, data->col, data->hit);
    protStruct.checksum = CheckSum(protStruct.maxMessage);
    sprintf(message, MESSAGE_TEMPLATE, protStruct.maxMessage, protStruct.checksum);
    return strlen(message);
}

int ProtocolEncodeChaMessage(char *message, const NegotiationData *data)
{
    sprintf(protStruct.maxMessage, PAYLOAD_TEMPLATE_CHA, data->encryptedGuess, data->hash);
    protStruct.checksum = CheckSum(protStruct.maxMessage);
    sprintf(message, MESSAGE_TEMPLATE, protStruct.maxMessage, protStruct.checksum);
    return strlen(message);
}

int ProtocolEncodeDetMessage(char *message, const NegotiationData *data)
{
    sprintf(protStruct.maxMessage, PAYLOAD_TEMPLATE_DET, data->guess, data->encryptionKey);
    protStruct.checksum = CheckSum(protStruct.maxMessage);
    sprintf(message, MESSAGE_TEMPLATE, protStruct.maxMessage, protStruct.checksum);
    return strlen(message); 
}

void ProtocolGenerateNegotiationData(NegotiationData *data)
{
    data->encryptionKey = rand();
    data->guess = rand();
    data->encryptedGuess = data->encryptionKey ^ data->guess;
    data->hash = (data->guess >> 8) ^ (data->guess & 0xFF) ^
            (data->encryptionKey >> 8) ^ (data->encryptionKey & 0xFF);
}

uint8_t ProtocolValidateNegotiationData(const NegotiationData *data) 
{
    if ((data->encryptionKey ^ data->guess) != data->encryptedGuess) {
        printf("\nfirst one is wrong");
        return FALSE;
    }
    
    if (data->hash == ((data->guess >> 8) ^ (data->encryptionKey >> 8) ^ 
            (data->encryptionKey & 0xFF) ^ (data->guess & 0xFF))) {
        return TRUE;
    } else {
        return FALSE;
    }
}

TurnOrder ProtocolGetTurnOrder(const NegotiationData *myData, const NegotiationData *oppData)
{
    if ((myData->encryptedGuess ^ oppData->encryptedGuess) & 0x01) {
        if (myData->guess > oppData->guess) {
            return TURN_ORDER_START;
        } else {
            return TURN_ORDER_DEFER;
        }
    } else {
        return TURN_ORDER_TIE;
    }
}

/*Static Helper Functions*************************************************************************/

int ConvertAscii(char asciiChar)
{
    switch (asciiChar) {
    case '0'...'9':
        return (asciiChar - 48);
        break;
    case 'a'...'f':
        return (asciiChar - 87);
        break;

    case 'A'...'F':
        return (asciiChar - 55);
        break;
        
    default:
        return FALSE;
        break;
    }
}

uint8_t CheckSum(char *stringChecksum)
{
    int i;
    unsigned char checksum = 0;
    
    for(i = 0; stringChecksum[i] != NULL; i++) {
        checksum ^= stringChecksum[i];
    }
    return checksum;
}