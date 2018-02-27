// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Adc.h"
#include "Ascii.h"
#include "Buttons.h"
#include "Oled.h"
#include "OledDriver.h"
#include "Leds.h"

// **** Set any macros or preprocessor directives here ****
// Set a macro for resetting the timer, makes the code a little clearer.
#define TIMER_2HZ_RESET() (TMR1 = 0)
#define LONG_PRESS 5
#define TRUE 1
#define FALSE 0

// **** Declare any datatypes here ****

typedef enum {
    RESET,
    START,
    COUNTDOWN,
    PENDING_SELECTOR_CHANGE,
    PENDING_RESET
} OvenStates; // these define the oven states

typedef enum {
    BAKE,
    TOAST,
    BROIL
} CookStates; // these define the cook states

typedef enum {
    TIME,
    TEMP
} InputStates;

typedef struct {
    uint16_t cookTimeLeft; // stores the amount of cooking time left
    uint16_t cookInitTime; // stores the initial cooking time
    uint16_t temperature; // stores temperature
    uint16_t buttonPressCounter; // will check for long presses 
    uint16_t inputSelect; // stores the selection

    CookStates cookMode; // stores the cook states
    OvenStates ovenState; // stores the oven states
    InputStates inputSelection; // stores the input state (TIME/TEMP)
} ovenData;

typedef struct {
    int event2Hz; // like the previous lab, each event has its own true/false
    int event5Hz; // " "
    int event100Hz; // " "

    uint8_t counter5Hz;
} TimerInterrupt;



// **** Define any module-level, global, or external variables here ****
ovenData oven;
TimerInterrupt interrupts;
ButtonEventFlags buttonEvent;
uint32_t FRC = 0; // acronym for free running counter. It's for the 5Hz interrupt.
uint32_t prevFRC = 0; // the previous FRC, to be used in START to PENDING_SELECTOR_CHANGE


// Configuration Bit settings

int main() {
    BOARD_Init();

    // Configure Timer 1 using PBCLK as input. We configure it using a 1:256 prescalar, so each timer
    // tick is actually at F_PB / 256 Hz, so setting PR1 to F_PB / 256 / 2 yields a 0.5s timer.
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, BOARD_GetPBClock() / 256 / 2);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T1);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T1, INT_ENABLED);

    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a .01s timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    // Configure Timer 3 using PBCLK as input. We configure it using a 1:256 prescalar, so each timer
    // tick is actually at F_PB / 256 Hz, so setting PR3 to F_PB / 256 / 5 yields a .2s timer.
    OpenTimer3(T3_ON | T3_SOURCE_INT | T3_PS_1_256, BOARD_GetPBClock() / 256 / 5);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T3);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_3_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T3, INT_ENABLED);

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/
    OledInit();
    ButtonsInit();
    LEDS_INIT();
    AdcInit();
    oven.ovenState = RESET; // initializes oven from nowhere to RESET


    LEDS_SET(0x01);

    while (1) {
        switch (oven.ovenState) {
            case RESET:
                oven.inputSelect = TIME;
                oven.temperature = 300;
                OledClear(OLED_COLOR_BLACK);
                oven.ovenState = START;
                break;
            case START:
                if (AdcChanged()) {
                    if(oven.inputSelection == TIME) {
                        oven.cookInitTime = (AdcRead() >> 2) + 1; // by bitshifting left 2 and adding 1, we get time.
                    } else if(oven.inputSelection == TEMP) {
                        oven.temperature = (AdcRead() >> 2) + 300; // " " but for temperature.
                    }
                    // UPDATE LEDS HERE.
                    break;
                }
                if (buttonEvent & BUTTON_EVENT_4DOWN) {
                    TIMER_2HZ_RESET(); // Reset 2Hz Timer
                    interrupts.event2Hz = FALSE; // reset interrupt
                    oven.cookInitTime = (AdcRead() >> 2) + 1; // how do you save the initial start time? AdcRead??
                    oven.ovenState = COUNTDOWN; // Turn oven on.
                    buttonEvent = BUTTON_EVENT_NONE; // Clear button event.
                    OledUpdate(); // update display...?
                    break;
                } else if (buttonEvent & BUTTON_EVENT_3DOWN) {
                    prevFRC = FRC; // store free running time
                    buttonEvent = BUTTON_EVENT_NONE;
                    oven.ovenState = PENDING_SELECTOR_CHANGE;
                    break;
                }
                // eventually, oven.ovenState will equal COUNTDOWN or PENDING_SELECTOR_CHANGE
                break;
            case COUNTDOWN:
                
                
                break;
            case PENDING_SELECTOR_CHANGE:
                // if button counter < LONG_PRESS && BUTTON_EVENT_3UP
                if(FRC - prevFRC < LONG_PRESS && BUTTON_EVENT_3UP) {
                    oven.cookInitTime = 1;
                    oven.temperature = 350;
                    
                    // rotate between the cooking modes.
                    switch(oven.cookMode) {
                        case BAKE:
                            oven.cookMode = TOAST;
                            break;
                        case TOAST:
                            oven.cookMode = BROIL;
                            break;
                        case BROIL:
                            oven.cookMode = BAKE;
                            break;
                        default:
                            printf("Something went wrong.\n");
                            break;
                    }
                    
                    // UPDATE THE DISPLAY
                    
                    buttonEvent = BUTTON_EVENT_NONE;
                    
                } else if(FRC - prevFRC >= LONG_PRESS && BUTTON_EVENT_3UP) {
                    switch(oven.inputSelection) {
                        case TIME:
                            oven.inputSelection = TEMP;
                            break;
                        case TEMP:
                            oven.inputSelection = TIME;
                            break;
                        default:
                            printf("Something went wrong in input selection.\n");
                            break;
                    }
                    
                    // UPDATE THE DISPLAY
                    
                    buttonEvent = BUTTON_EVENT_NONE;
                }
                break;
            case PENDING_RESET:

                break;
            default:
                printf("boo");
                break;
        }
    }


    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/
    while (1);
}

void printOven(void) {
    // three if statements followed by just a regular print.
    
}

void __ISR(_TIMER_1_VECTOR, ipl4auto) TimerInterrupt2Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 4;
    if(AdcChanged()) {
        interrupts.event2Hz = TRUE;
    }
    
}

void __ISR(_TIMER_3_VECTOR, ipl4auto) TimerInterrupt5Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 12;
    interrupts.event5Hz = TRUE;
    FRC++;
}

void __ISR(_TIMER_2_VECTOR, ipl4auto) TimerInterrupt100Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;
    buttonEvent = ButtonsCheckEvents();
}