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
int counter = 0;
uint64_t hist = 0;
int confident = FALSE; // this confidence flag will determine whether ButtonsCheckEvents is confident all 64 bits are the same.
int histFilled = FALSE;

// **** Declare function prototypes ****
uint8_t EightBitButtonChecker(void);

void ButtonsInit() {
    TRISD |= 0x00E0;
    TRISF |= 0x0002;
}

uint8_t ButtonsCheckEvents(void) {
    uint8_t unconfidentReturnState = 0; // will store the current state of LEDs
    uint8_t confidentReturnState;
    confident = FALSE;


    while (!confident) {
        // The following if-statements will OR the proper BUTTON_EVENT to
        // the unconfidentReturnState. This will be returned at the bottom.
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

            if (counter == 8) {
                histFilled = TRUE;
            }
        }

        unconfidentReturnState = EightBitButtonChecker();
        hist = hist << 8;
        hist |= unconfidentReturnState;

        // now check for confidence.
        uint8_t a[8];
        int i;
        for (i = 0; i < 8; i++) {
            uint8_t shifted = hist >> i * 8;
            a[i] = shifted;
        }

        // this for loop will check each element in array 'a' with a[0].
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
    }
    return confidentReturnState;
}

uint8_t EightBitButtonChecker(void) {
    uint8_t unconfident = 0;
    if (BUTTON_STATES() & BUTTON_STATE_1) {
        unconfident |= BUTTON_EVENT_1DOWN;
    } else {
        unconfident |= BUTTON_EVENT_1UP;
    }
    if (BUTTON_STATES() & BUTTON_STATE_2) {
        unconfident |= BUTTON_EVENT_2DOWN;
    } else {
        unconfident |= BUTTON_EVENT_2UP;
    }
    if (BUTTON_STATES() & BUTTON_STATE_3) {
        unconfident |= BUTTON_EVENT_3DOWN;
    } else {
        unconfident |= BUTTON_EVENT_3UP;
    }
    if (BUTTON_STATES() & BUTTON_STATE_4) {
        unconfident |= BUTTON_EVENT_4DOWN;
    } else {
        unconfident |= BUTTON_EVENT_4UP;
    }
    return unconfident;
}