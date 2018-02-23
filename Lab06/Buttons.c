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

// **** Declare function prototypes ****

void ButtonsInit() {
    TRISD |= 0x00E0;
    TRISF |= 0x0002;
}

uint8_t ButtonsCheckEvents(void) {
    uint8_t returnState; // will store the current state of LEDs

    // The following if-statements will OR the proper BUTTON_EVENT to
    // the returnState. This will be returned at the bottom.
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