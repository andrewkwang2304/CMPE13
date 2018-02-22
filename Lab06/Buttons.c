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
    uint8_t sample1;
    uint8_t sample2;
    uint8_t sample3;
    uint8_t sample4;
    uint8_t returnState;

    if (BUTTON_STATES() & BUTTON_STATE_1) {
        returnState |= BUTTON_EVENT_1DOWN;
        puts("Button 1 is pressed\n");
    }
    if (BUTTON_STATES() & BUTTON_STATE_2) {
        returnState |= BUTTON_EVENT_2DOWN;
        puts("Button 2 is pressed\n");
    }
    if (BUTTON_STATES() & BUTTON_STATE_3) {
        returnState |= BUTTON_EVENT_3DOWN;
        puts("Button 3 is pressed\n");
    }  
    if (BUTTON_STATES() & BUTTON_STATE_4) {
        returnState |= BUTTON_EVENT_4DOWN;
        puts("Button 4 is pressed\n");
    }
    
    return returnState;
}