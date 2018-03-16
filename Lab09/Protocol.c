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
#define undefined -256
#define maxTemplateMessage 4
static char *tempTokMessage;
static int detFlag = FALSE;
static int chaFlag = FALSE;
static int cooFlag = FALSE;
static int hitFlag = FALSE;

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

ProtocolParserStatus ProtocolDecode(char in, NegotiationData *nData, GuessData *gData) {
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
            if (in == '*') {
                protStruct.protocolState = FIRST_CHECKSUM_HALF;
                return PROTOCOL_PARSING_GOOD;
            } else {
                protStruct.maxMessage[protStruct.arrayIndex] = in;
                protStruct.arrayIndex++;
                return PROTOCOL_PARSING_GOOD;
            }
            break;

        case FIRST_CHECKSUM_HALF:

            if (ConvertAscii(in) != undefined) {
                protStruct.checksum = ConvertAscii(in) << 4;
                protStruct.protocolState = SECOND_CHECKSUM_HALF;
                return PROTOCOL_PARSING_GOOD;
            } else {
                protStruct.protocolState = WAITING;
                return PROTOCOL_PARSING_FAILURE;
            }
            break;

        case SECOND_CHECKSUM_HALF:
            if (ConvertAscii(in) != undefined) {
                protStruct.checksum = protStruct.checksum ^ ConvertAscii(in);
                protStruct.maxMessage[protStruct.arrayIndex] = NULL;
                protStruct.protocolState = NEWLINE;
                return PROTOCOL_PARSING_GOOD;
            } else {
                protStruct.protocolState = WAITING;
                return PROTOCOL_PARSING_FAILURE;
            }
            break;

        case NEWLINE:
            detFlag = FALSE;
            chaFlag = FALSE;
            cooFlag = FALSE;
            hitFlag = FALSE;

            tempTokMessage = protStruct.maxMessage;

            if (strstr(tempTokMessage, "DET") != NULL) {
                detFlag = TRUE;
            } else if (strstr(tempTokMessage, "CHA") != NULL) {
                chaFlag = TRUE;
            } else if (strstr(tempTokMessage, "COO") != NULL) {
                cooFlag = TRUE;
            } else if (strstr(tempTokMessage, "HIT") != NULL) {
                hitFlag = TRUE;
            }

            if (in == '\n') {
                //See whether DET,CHA,COO,or,HIT are in protStruct.maxMessage
                if (detFlag == TRUE) {
                    sscanf(protStruct.maxMessage, PAYLOAD_TEMPLATE_DET,
                            &nData->guess, &nData->encryptionKey);
                    protStruct.protocolState = WAITING;
                    return PROTOCOL_PARSED_DET_MESSAGE;
                } else if (chaFlag == TRUE) {
                    sscanf(protStruct.maxMessage, PAYLOAD_TEMPLATE_CHA,
                            &nData->encryptedGuess, &nData->hash);
                    protStruct.protocolState = WAITING;
                    return PROTOCOL_PARSED_CHA_MESSAGE;
                } else if (cooFlag == TRUE) {
                    sscanf(protStruct.maxMessage, PAYLOAD_TEMPLATE_COO,
                            &gData->row, &gData->col);
                    protStruct.protocolState = WAITING;
                    return PROTOCOL_PARSED_COO_MESSAGE;
                } else if (hitFlag == TRUE) {
                    sscanf(protStruct.maxMessage, PAYLOAD_TEMPLATE_HIT,
                            &gData->row, &gData->col,
                            &gData->hit);
                    protStruct.protocolState = WAITING;
                    return PROTOCOL_PARSED_HIT_MESSAGE;
                } else {
                    protStruct.protocolState = WAITING;
                    return PROTOCOL_PARSING_FAILURE;
                }

            } else {
                protStruct.protocolState = WAITING;
                return PROTOCOL_PARSING_FAILURE;
            }
            break;
    }
    return PROTOCOL_PARSING_GOOD;
}

int ProtocolEncodeCooMessage(char *message, const GuessData *data) {
    int messageLength;

    sprintf(protStruct.maxMessage, PAYLOAD_TEMPLATE_COO, data->row, data->col);
    protStruct.checksum = CheckSum(protStruct.maxMessage);
    sprintf(message, MESSAGE_TEMPLATE, protStruct.maxMessage, protStruct.checksum);
    messageLength = strlen(message);
    return messageLength;
}

int ProtocolEncodeHitMessage(char *message, const GuessData *data) {
    int messageLength;

    sprintf(protStruct.maxMessage, PAYLOAD_TEMPLATE_HIT, data->row, data->col, data->hit);
    protStruct.checksum = CheckSum(protStruct.maxMessage);
    sprintf(message, MESSAGE_TEMPLATE, protStruct.maxMessage, protStruct.checksum);
    messageLength = strlen(message);
    return messageLength;
}

int ProtocolEncodeChaMessage(char *message, const NegotiationData *data) {
    int messageLength;

    sprintf(protStruct.maxMessage, PAYLOAD_TEMPLATE_CHA, data->encryptedGuess, data->hash);
    protStruct.checksum = CheckSum(protStruct.maxMessage);
    sprintf(message, MESSAGE_TEMPLATE, protStruct.maxMessage, protStruct.checksum);
    messageLength = strlen(message);
    return messageLength;
}

int ProtocolEncodeDetMessage(char *message, const NegotiationData *data) {
    int messageLength;

    sprintf(protStruct.maxMessage, PAYLOAD_TEMPLATE_DET, data->guess, data->encryptionKey);
    protStruct.checksum = CheckSum(protStruct.maxMessage);
    sprintf(message, MESSAGE_TEMPLATE, protStruct.maxMessage, protStruct.checksum);
    messageLength = strlen(message);
    return messageLength;
}

void ProtocolGenerateNegotiationData(NegotiationData *data) {
    data->encryptionKey = rand();
    data->guess = rand();
    data->encryptedGuess = data->encryptionKey ^ data->guess;
    data->hash = (data->guess & 0xFF) ^ (data->guess >> 8) ^ (data->encryptionKey & 0xFF) ^
            (data->encryptionKey >> 8);
}

uint8_t ProtocolValidateNegotiationData(const NegotiationData *data) {
    if (data->hash == ((data->guess & 0xFF) ^ (data->guess >> 8) ^ (data->encryptionKey & 0xFF) ^
            (data->encryptionKey >> 8)) && ((data->encryptionKey ^ data->guess) ==
            data->encryptedGuess)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

TurnOrder ProtocolGetTurnOrder(const NegotiationData *myData, const NegotiationData *oppData) {
    if ((myData->encryptedGuess ^ oppData->encryptedGuess) & 0x01) {
        if (myData->guess > oppData->guess) {
            return TURN_ORDER_START;
        } else if (myData->guess < oppData->guess) {
            return TURN_ORDER_DEFER;
        } else {
            return TURN_ORDER_TIE;
        }
    }
    return 0;
}

/*Static Helper Functions*************************************************************************/

int ConvertAscii(char asciiChar) {
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
            return undefined;
            break;
    }
}

uint8_t CheckSum(char *stringChecksum) {
    int i;
    unsigned char checksum = 0;

    for (i = 0; stringChecksum[i] != NULL; i++) {
        checksum ^= stringChecksum[i];
    }
    return checksum;
}
