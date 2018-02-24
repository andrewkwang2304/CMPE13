// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Buttons.h"
#include "Leds.h"

// **** Set macros and preprocessor directives ****
#define TRUE 1
#define FALSE 0

// **** Declare any datatypes here ****

typedef struct {
    uint8_t event;
    uint8_t value;
} ButtonResult;

// **** Define global, module-level, or external variables here ****
ButtonResult buttonData;

// **** Declare function prototypes ****
void SetLED(uint8_t bitmask);

int main(void) {
    BOARD_Init();

    // Configure Timer 1 using PBCLK as input. This default period will make the LEDs blink at a
    // pretty reasonable rate to start.
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_8, 0xFFFF);

    // Set up the timer interrupt with a priority of 4.
    INTClearFlag(INT_T1);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T1, INT_ENABLED);

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/

    // Initialize necessary components.
    ButtonsInit();
    LEDS_INIT();
    buttonData.event = FALSE;
    buttonData.value = 0;
    int switchPosition;
    int sPosTemp;
    int mask;

    while (1) {
        if (buttonData.event) {


            switchPosition = SWITCH_STATES();
            sPosTemp = SWITCH_STATE_SW1 & switchPosition; // compares the SWITCH STATE with the current switch position.

            if (sPosTemp) { // if switch is up.
                if ((buttonData.value & 0b11) > 1) { // when > 1, it is on.
                    mask = 0b00000011;
                    SetLED(mask);
                }
            } else if (!sPosTemp) { // if switch is down.
                if ((buttonData.value & 0b11) > 1) {
                    mask = 0b00000011;
                    SetLED(mask);
                }
            }

            sPosTemp = SWITCH_STATE_SW2 & switchPosition; // adjust sPosTemp for the next switch.

            if (sPosTemp) { // if switch is up.
                if ((buttonData.value & 0b1100) > 0b100) {
                    mask = 0b00001100;
                    SetLED(mask);
                }
            } else if (!sPosTemp) {
                if ((buttonData.value & 0b1100) > 0b100) {
                    mask = 0b00001100;
                    SetLED(mask);
                }
            }

            sPosTemp = SWITCH_STATE_SW3 & switchPosition; // adjust sPosTemp for the next switch.

            if (sPosTemp) { // if switch is up.
                if ((buttonData.value & 0b110000) > 0b10000) {
                    mask = 0b00110000;
                    SetLED(mask);
                }
            } else if (!sPosTemp) {
                if ((buttonData.value & 0b110000) > 0b10000) {
                    mask = 0b00110000;
                    SetLED(mask);
                }
            }

            sPosTemp = SWITCH_STATE_SW4 & switchPosition; // adjust sPosTemp for the next switch.

            if (sPosTemp) { // if switch is up.
                if ((buttonData.value & 0b11000000) > 0b1000000) {
                    mask = 0b11000000;
                    SetLED(mask);
                }
            } else if (!sPosTemp) { // if switch is down.
                if ((buttonData.value & 0b11000000) > 0b1000000) {
                    mask = 0b11000000;
                    SetLED(mask);
                }
            }

            buttonData.event = FALSE;
        }
    }


    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/

    while (1);
}

/**
 * This is the interrupt for the Timer1 peripheral. It checks for button events and stores them in a
 * module-level variable.
 */
void __ISR(_TIMER_1_VECTOR, IPL4AUTO) Timer1Handler(void) {
    // Clear the interrupt flag.
    INTClearFlag(INT_T1);
    buttonData.value = ButtonsCheckEvents();

    if (buttonData.value) {
        buttonData.event = TRUE;
    }

}

void SetLED(uint8_t bitmask) {
    uint8_t ledGet = LEDS_GET(); // get LEDS.
    uint8_t temp = ledGet | bitmask;
    if (temp == ledGet) {
        temp ^= bitmask; // XOR temp with bitmask. This will allow for toggle.
    }
    LEDS_SET(temp);
}