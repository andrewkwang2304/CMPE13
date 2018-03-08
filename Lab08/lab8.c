// **** Include libraries here ****
// Standard C libraries


//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Morse.h"
#include "Oled.h"
#include "Tree.h"


// **** Set any macros or preprocessor directives here ****
// Specify a bit mask for setting/clearing the pin corresponding to BTN4. Should only be used when
// unit testing the Morse event checker.
#define BUTTON4_STATE_FLAG (1 << 7)
#define MAX_STRING_SIZE 200

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
static MorseEvent checker; // for use in MorseCheckEvents
static char decodeOutput; // decodeOutput stores the returned value of MorseDecode

static char topLine[21]; // topLine will store the string value of the top line of the OLED.
static char bottomLine[MAX_STRING_SIZE]; // vice versa.
static int topCharTracker = 0; // this will track the location of topLine
static int bottomCharTracker = 0; // vice versa.

// **** Declare any function prototypes here ****
static void clearAndUpdateTop(void);
static void appendCharToTop(char c);
static void appendCharToBottom(char c);

int main() {
    BOARD_Init();


    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a .01s timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    OledInit();

    if (MorseInit() == STANDARD_ERROR) {
        printf("ERROR: MorseInit() failed to initialize.\n");
        FATAL_ERROR();
    }

    while (1) {
        switch (checker) { // checker is a MorseEvent.
            case MORSE_EVENT_NONE:
                break;
            case MORSE_EVENT_DOT:
                decodeOutput = MorseDecode(MORSE_CHAR_DOT);
                appendCharToTop(MORSE_CHAR_DOT); // print out to OLED
                checker = MORSE_EVENT_NONE;
                break;
            case MORSE_EVENT_DASH:
                decodeOutput = MorseDecode(MORSE_CHAR_DASH); // decode
                appendCharToTop(MORSE_CHAR_DASH); // append dash character to top line.
                checker = MORSE_EVENT_NONE; // reset the checker.
                break;
            case MORSE_EVENT_INTER_LETTER:
                decodeOutput = MorseDecode(MORSE_CHAR_END_OF_CHAR);
                clearAndUpdateTop(); // clear the top line.
                if (decodeOutput == '\0') {
                    appendCharToBottom(MORSE_CHAR_END_OF_CHAR);
                } else {
                    appendCharToBottom(decodeOutput);
                }
                MorseDecode(MORSE_CHAR_DECODE_RESET); // this will reset the head to the very top of the BST.
                checker = MORSE_EVENT_NONE; // reset the checker.
                break;
            case MORSE_EVENT_INTER_WORD:
                decodeOutput = MorseDecode(MORSE_CHAR_END_OF_CHAR);
                clearAndUpdateTop();
                if (decodeOutput == '\0') {
                    appendCharToBottom(MORSE_CHAR_END_OF_CHAR);
                } else {
                    appendCharToBottom(decodeOutput);
                }
                appendCharToBottom(' '); // includes a space in between words.
                MorseDecode(MORSE_CHAR_DECODE_RESET);
                checker = MORSE_EVENT_NONE; // reset the checker.
                break;
            default:
                checker = MORSE_EVENT_NONE;
                break;
        }
    }


    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks.
     *****************************************************************************/

    while (1);
}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) TimerInterrupt100Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;

    //******** Put your code here *************//
    checker = MorseCheckEvents(); // simply checks events.
}

static void clearAndUpdateTop(void) {
    for (topCharTracker = 0; topCharTracker < 20; topCharTracker++) {
        topLine[topCharTracker] = ' '; // although REALLY unlikely this will ever occur, this will cause the top line to wrap around to the beginning if it exceeds 20.
    }
    topCharTracker = 0;
    OledDrawString(topLine);
    OledUpdate();
}

static void appendCharToTop(char c) {
    topLine[topCharTracker++] = c; // store character to current location in topLine array.
    if (topCharTracker > 20) {
        topCharTracker = 0;
    }
    OledDrawString(topLine);
    OledUpdate();
}

static void appendCharToBottom(char c) {
    bottomLine[bottomCharTracker++] = c; // store character to current location in bottomLine array.
    OledDrawString(bottomLine);
    OledUpdate();
}
