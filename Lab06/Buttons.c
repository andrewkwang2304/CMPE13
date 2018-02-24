// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Buttons.h"

// **** Set macros and preprocessor directives ****
#define TRUE 1
#define FALSE 0

// **** Declare any datatypes here ****

// **** Define global, module-level, or external variables here ****
int counter = 0; // this counter works for the initial stage when hist isn't filled yet.
uint64_t hist = 0; // hist (history) is 64 bits because my I use 8 bits per sample. See EightBitButtonChecker() at the bottom.
int confident = FALSE; // this confidence flag will determine whether ButtonsCheckEvents is confident all 64 bits are the same.
int histFilled = FALSE; // history is not filled yet.

// **** Declare function prototypes ****
uint8_t EightBitButtonChecker(void);

void ButtonsInit() {
    TRISD |= 0x00E0;
    TRISF |= 0x0002;
}

uint8_t ButtonsCheckEvents(void) {
    uint8_t unconfidentReturnState = 0; // I use 'unconfident' because we don't know if the return state is correct or not. 
    uint8_t confidentReturnState; // we will store unconfidentReturnState into here when we are confident the return state is accurate.
    confident = FALSE;

    // The idea behind these two while loops is to:
    //      a). fill the history.
    //      b). determine confidence in the LED's return state based on it's history.
    while (!confident) {
        while (!histFilled) {
            unconfidentReturnState = EightBitButtonChecker();

            if (counter == 0) {
                hist = unconfidentReturnState;
                counter++;
            } else { // will bit shift left and store next 8 bits.
                hist = hist << 8;
                hist |= unconfidentReturnState;
                counter++;
            }

            // each unconfidentReturnState added to hist is eight bits.
            // Since hist is uint64_t, we must repeat the filling process 8 times.
            if (counter == 8) {
                histFilled = TRUE;
            }
        } // END OF HISTFILLED

        unconfidentReturnState = EightBitButtonChecker(); // now hist is completely filled.
        hist = hist << 8;
        hist |= unconfidentReturnState;

        // Now we must check for confidence. The following will put the bit-shifted
        // hist values into a uint8_t array called 'a', 8 bits at a time.
        uint8_t a[8];
        int i;
        for (i = 0; i < 8; i++) {
            uint8_t shifted = hist >> i * 8;
            a[i] = shifted;
        }

        // This for loop will check each element in array 'a' with a[0].
        int j;
        for (j = 0; j < 8; j++) {
            if (a[0] == a[j]) {
                if (j == 7) {
                    confident = TRUE;
                    confidentReturnState = unconfidentReturnState;
                }
            } else {
                confident = FALSE;
                break;
            }
        }
    } // END OF CONFIDENT
    return confidentReturnState;
}

// The following function is a series of if else statements that ultimately
// provide an 8-bit output of the current state of the buttons.

uint8_t EightBitButtonChecker(void) {
    uint8_t returnState = 0;
    if (BUTTON_STATES() & BUTTON_STATE_1) {
        returnState |= BUTTON_EVENT_1DOWN;
    } else {
        returnState |= BUTTON_EVENT_1UP;
    }
    if (BUTTON_STATES() & BUTTON_STATE_2) {
        returnState |= BUTTON_EVENT_2DOWN;
    } else {
        returnState |= BUTTON_EVENT_2UP;
    }
    if (BUTTON_STATES() & BUTTON_STATE_3) {
        returnState |= BUTTON_EVENT_3DOWN;
    } else {
        returnState |= BUTTON_EVENT_3UP;
    }
    if (BUTTON_STATES() & BUTTON_STATE_4) {
        returnState |= BUTTON_EVENT_4DOWN;
    } else {
        returnState |= BUTTON_EVENT_4UP;
    }
    return returnState;
}