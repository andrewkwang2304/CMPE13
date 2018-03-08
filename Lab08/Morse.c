// **** Include libraries here ****
// Standard C libraries
#include <stdio.h>

//CMPE13 Support Library
#include "BOARD.h"
#include "Tree.h"
#include "Morse.h"
#include "Buttons.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set any macros or preprocessor directives here ****

// **** Declare any data types here ****

typedef enum {
    WAITING,
    DOT,
    DASH,
    INTER_LETTER
} MorseState;

// **** Define any module-level, global, or external variables here ****
static const char morseSetup[63] = {
    '#', 'E', 'I', 'S', 'H', '5', '4',
    'V', '#', '3', 'U', 'F', '#', '#',
    '#', '#', '2', 'A', 'R', 'L', '#',
    '#', '#', '#', '#', 'W', 'P', '#',
    '#', 'J', '#', '1', 'T', 'N', 'D',
    'B', '#', '6', 'X', '#', '#', 'K',
    'C', '#', '#', 'Y', '#', '#', 'M',
    'G', 'Z', '7', '#', 'Q', '#', '#',
    'O', '#', '8', '#', '#', '9', '0'
};
static Node *head;
static Node *current;
static Node *temp;
MorseState morseState; // stores the state of the MorseState.
static int c = 0; // counter
static char tempData; // 


// **** Declare any function prototypes here ****

int MorseInit(void) {
    ButtonsInit();
    head = TreeCreate(6, morseSetup);
    temp = head;
    if (head == NULL) {
        return STANDARD_ERROR;
    }
    current = head;
    return SUCCESS;
}

char MorseDecode(MorseChar in) {
    switch (in) {
        case MORSE_CHAR_DOT:
            if (current->leftChild) {
                current = current->leftChild; // decode to the left
                return SUCCESS;
            }
            return STANDARD_ERROR;
            break;
        case MORSE_CHAR_DASH:
            if (current->rightChild) {
                current = current->rightChild; // decode to the right
                return SUCCESS;
            }
            return STANDARD_ERROR;
        case MORSE_CHAR_END_OF_CHAR:
            tempData = current->data; // store temporary data for safety
            if (tempData != '#')
                return tempData;
            else
                return STANDARD_ERROR;
            break;
        case MORSE_CHAR_DECODE_RESET:
            current = head; // reset head to the beginning
            return SUCCESS;
        default:
            return STANDARD_ERROR;
    }
}

MorseEvent MorseCheckEvents(void) {
    uint16_t buttonEvent = ButtonsCheckEvents();
    c++; // increment counter
    switch (morseState) {
        case WAITING:
            if (buttonEvent == BUTTON_EVENT_4DOWN) {
                morseState = DOT; // change state to DOT
                c = 0; // reset counter
            }
            return MORSE_EVENT_NONE;
        case DOT:
            if (c >= MORSE_EVENT_LENGTH_DOWN_DOT) {
                morseState = DASH; // change state to DASH
            }
            if (buttonEvent == BUTTON_EVENT_4UP) {
                morseState = INTER_LETTER; // change state to INTER_LETTER
                c = 0; // reset counter
                return MORSE_EVENT_DOT;
            }
            return MORSE_EVENT_NONE;
        case DASH:
            if (buttonEvent == BUTTON_EVENT_4UP) {
                morseState = INTER_LETTER; // change state to INTER_LETTER
                c = 0;
                return MORSE_EVENT_DASH;
            }
            return MORSE_EVENT_NONE;
        case INTER_LETTER:
            if (c >= MORSE_EVENT_LENGTH_UP_INTER_LETTER_TIMEOUT) {
                morseState = WAITING; // change state to WAITING
                return MORSE_EVENT_INTER_WORD;
            } else if (buttonEvent == BUTTON_EVENT_4DOWN) {
                c = 0; // reset counter
                if (c >= MORSE_EVENT_INTER_LETTER) {
                    morseState = DOT; // change state to DOT
                    return MORSE_EVENT_INTER_LETTER;
                } else {
                    morseState = DOT;
                    return MORSE_EVENT_NONE;
                }
            }
        default:
            return NULL;
    }
    return NULL;
}
