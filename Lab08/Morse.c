// **** Include libraries here ****
// Standard C libraries
#include <stdio.h>

//CMPE13 Support Library
#include "BOARD.h"
#include "Tree.h"
#include "Morse.h"

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
} MorseEvents;

// **** Define any module-level, global, or external variables here ****
static const char *morseSetup[63] = {
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
static Node head;
static Node current;
MorseEvents morseEvent;
static int c = 0; // counter


// **** Declare any function prototypes here ****

int MorseInit(void) {
    head = TreeCreate(6, morseSetup);
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
                current = current->leftChild;
                return SUCCESS;
            }
            return STANDARD_ERROR;
            break;
        case MORSE_CHAR_DASH:
            if (current->rightChild) {
                current = current->rightChild;
                return SUCCESS;
            }
            return STANDARD_ERROR;
            break;
        case MORSE_CHAR_END_OF_CHAR:
            current = head;
            char temp = current->data;
            if (temp != '#')
                return temp;
            else
                return STANDARD_ERROR;
            break;
        case MORSE_CHAR_DECODE_RESET:
            current = head;
            return SUCCESS;
        default:
            return STANDARD_ERROR;
    }
}

MorseEvent MorseCheckEvents(void) {
    uint16_t buttonEvent = ButtonsCheckEvents();
    switch (morseEvent) {
        case WAITING:
            c++;
            if (buttonEvent == BUTTON_EVENT_4DOWN) {
                morseEvent = WAITING;
                c = 0;
            }
            return MORSE_EVENT_NONE;
        case DOT:
            c++;
            if (c >= MORSE_EVENT_LENGTH_DOWN_DOT) {
                morseEvent = DASH;
            }
            if (buttonEvent == BUTTON_EVENT_4UP) {
                morseEvent = INTER_LETTER;
                c = 0;
                return MORSE_EVENT_DOT;
            }
            return MORSE_EVENT_NONE;
        case DASH:
            if (buttonEvent == BUTTON_EVENT_4UP) {
                morseEvent = INTER_LETTER;
                c = 0;
                return MORSE_EVENT_DASH;
            }
            return MORSE_EVENT_NONE;
            break;
        case INTER_LETTER:
            if (c >= MORSE_EVENT_LENGTH_DOWN_DOT) {
                morseEvent = WAITING;
                return MORSE_EVENT_INTER_WORD;
            }
            if (buttonEvent == BUTTON_EVENT_4DOWN) {
                c = 0;
                if (c >= MORSE_EVENT_INTER_LETTER) {
                    morseEvent = DOT;
                    return MORSE_EVENT_INTER_LETTER;
                }
                morseEvent = DOT;
                return MORSE_EVENT_NONE;
            }
        default:
            return MORSE_EVENT_NONE;
    }
    return MORSE_EVENT_NONE;
}
