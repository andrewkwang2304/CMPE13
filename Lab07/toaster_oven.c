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
#define MAX_SIZE 500

// **** Declare any function prototypes here ****
void printOven(void);
void LEDOvenTimer(void);

// **** Declare any datatypes here ****

typedef enum {
    ON,
    OFF
} OnOrOff;

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
    OnOrOff onOrOff; // on or off.
} ovenData;

typedef struct {
    int event2Hz; // like the previous lab, each event has its own true/false
    int event5Hz; // " "
    uint8_t counter5Hz;
} TimerInterrupt;



// **** Define any module-level, global, or external variables here ****
static ovenData oven;
static TimerInterrupt interrupts;
static ButtonEventFlags buttonEvent;
static uint32_t FRC = 0; // acronym for free running counter. It's for the 5Hz interrupt.
static uint32_t prevFRC = 0; // the previous FRC, to be used in START to PENDING_SELECTOR_CHANGE
static int Hz2Counter = 0; // this prevents double time in the timer. See 2Hz Interrupt.


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

    while (1) {
        switch (oven.ovenState) {
            case RESET:
                oven.inputSelect = TIME;
                oven.temperature = 300;
                OledClear(OLED_COLOR_BLACK);
                oven.ovenState = START;
                oven.onOrOff = OFF;
                printOven();
                break;
            case START:
                if (AdcChanged()) {
                    if (oven.inputSelection == TIME) {
                        oven.cookInitTime = (AdcRead() >> 2) + 1; // by bitshifting left 2 and adding 1, we get time.
                        oven.cookTimeLeft = oven.cookInitTime;
                    } else if (oven.inputSelection == TEMP) {
                        oven.temperature = (AdcRead() >> 2) + 300; // " " but for temperature.
                    }
                    printOven();
                    break;
                }
                if (buttonEvent & BUTTON_EVENT_4DOWN) {
                    TIMER_2HZ_RESET(); // Reset 2Hz Timer
                    interrupts.event2Hz = FALSE; // reset interrupt
                    oven.cookInitTime = (AdcRead() >> 2) + 1;
                    oven.cookTimeLeft = oven.cookInitTime;
                    oven.onOrOff = ON; // Turn oven on.
                    oven.ovenState = COUNTDOWN;
                    buttonEvent = BUTTON_EVENT_NONE; // Clear button event.
                    printOven();
                } else if (buttonEvent & BUTTON_EVENT_3DOWN) {
                    prevFRC = FRC; // store free running time
                    buttonEvent = BUTTON_EVENT_NONE;
                    oven.ovenState = PENDING_SELECTOR_CHANGE;
                }
                // eventually, oven.ovenState will equal COUNTDOWN or PENDING_SELECTOR_CHANGE
                break;
            case COUNTDOWN:
                if (interrupts.event2Hz && oven.cookTimeLeft > 0) {
                    oven.cookTimeLeft--;
                    LEDOvenTimer();
                    printOven();
                    interrupts.event2Hz = FALSE;
                } else if (buttonEvent & BUTTON_EVENT_4DOWN) {
                    prevFRC = FRC;
                    buttonEvent = BUTTON_EVENT_NONE;
                    oven.ovenState = PENDING_RESET;
                }
                break;
            case PENDING_SELECTOR_CHANGE:
                // if button counter < LONG_PRESS && BUTTON_EVENT_3UP
                if ((FRC - prevFRC < LONG_PRESS) && (BUTTON_EVENT_3UP & buttonEvent)) {
                    oven.cookInitTime = 1;
                    oven.temperature = 350;

                    // rotate between the cooking modes.
                    switch (oven.cookMode) {
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

                    printOven();

                    buttonEvent = BUTTON_EVENT_NONE; // reset button state
                    oven.ovenState = START; // return to START
                } else if (FRC - prevFRC >= LONG_PRESS && (BUTTON_EVENT_3UP & buttonEvent)) {
                    switch (oven.inputSelection) { // this will rotate between TIME and TEMP
                        case TIME:
                            if (oven.cookMode == TOAST) { // TOAST doesn't have a TEMP, so it should just ADCRead.
                                oven.cookInitTime = (AdcRead() >> 2) + 1;
                                break;
                            } else {
                                oven.inputSelection = TEMP;
                                oven.cookInitTime = (AdcRead() >> 2) + 1;
                            }
                            break;
                        case TEMP:
                            oven.inputSelection = TIME;
                            oven.temperature = (AdcRead() >> 2) + 300;
                            break;
                        default:
                            printf("Something went wrong in input selection.\n");
                            break;
                    }

                    printOven();

                    buttonEvent = BUTTON_EVENT_NONE;
                    oven.ovenState = START;
                }
                break;
            case PENDING_RESET:
                if (interrupts.event2Hz && oven.cookTimeLeft > 0) {
                    oven.cookTimeLeft--;
                    // update LEDS
                    printOven(); // update the display
                    interrupts.event2Hz = FALSE;
                } else if ((FRC - prevFRC >= LONG_PRESS) && (buttonEvent & BUTTON_EVENT_4UP)) {
                    buttonEvent = BUTTON_EVENT_NONE;
                    oven.ovenState = RESET;
                    LEDS_SET(0b00000000); // this will reset the LEDs if user holds down BUTTON 4.
                }
                break;
            default:
                printf("Error in main switch statement.");
                break;
        }
        //        oven.cookMode = BAKE;
        //        printOven();
    }


    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/
    while (1);
}

void printOven(void) {
    int cooking = FALSE; // cooking is a boolean that determines whether something is cooking or not.

    // if the oven is in either COUNTDOWN or in PENDING_RESET
    if ((oven.ovenState == COUNTDOWN) || (oven.ovenState == PENDING_RESET)) {
        cooking = TRUE;
    }

    char arr[MAX_SIZE];
    int i = 0;
    for (; i <= MAX_SIZE; i++) { // this will just clear the array.
        arr[i] = '\0';
    }
    int time = (int) (oven.cookTimeLeft);
    int min = time / 60;
    int sec = time % 60;

    // this marks the first switch statement, which checks for BAKE, BROIL, and TOAST.
    switch (oven.cookMode) {
        case BAKE:
            if (cooking) {
                sprintf(arr, "|\x1\x1\x1\x1\x1|");
            } else {
                sprintf(arr, "|\x2\x2\x2\x2\x2|");
            }

            sprintf(strlen(arr) + arr, "  Mode: BAKE");

            if (!cooking) {
                // this second switch statement manages TIME and TEMP
                switch (oven.inputSelection) {
                    case TIME:
                        sprintf(strlen(arr) + arr, "\n|     | >Time: %2d:%02d", min, sec);
                        sprintf(strlen(arr) + arr, "\n|-----|  Temp: %3d\370F", oven.temperature);
                        break;
                    case TEMP:
                        sprintf(strlen(arr) + arr, "\n|     |  Time: %2d:%02d", min, sec);
                        sprintf(strlen(arr) + arr, "\n|-----| >Temp: %3d\370F", oven.temperature);
                        break;
                }
            } else {
                sprintf(strlen(arr) + arr, "\n|     |  Time: %2d:%02d", min, sec);
                sprintf(strlen(arr) + arr, "\n|-----|  Temp: %3d\370F", oven.temperature);
            }
            break;
        case BROIL:
            if (cooking) {
                sprintf(arr, "|\x1\x1\x1\x1\x1|");
            } else if (!cooking) {
                sprintf(arr, "|\x2\x2\x2\x2\x2|");
            }

            sprintf(strlen(arr) + arr, "  Mode: BROIL");
            sprintf(strlen(arr) + arr, "\n|     |  Time: %2d:%02d", min, sec);
            sprintf(strlen(arr) + arr, "\n|-----|  Temp: %3d\370F", 500);
            break;
        case TOAST:
            if (cooking) {
                sprintf(arr, "|\x1\x1\x1\x1\x1|");
            } else if (!cooking) {
                sprintf(arr, "|\x2\x2\x2\x2\x2|");
            }

            sprintf(strlen(arr) + arr, "  Mode: TOAST");
            sprintf(strlen(arr) + arr, "\n|     |  Time: %2d:%02d", min, sec);
            sprintf(strlen(arr) + arr, "\n|-----|");
            break;
    }

    if (!cooking) { // this will include the bottom special characters
        sprintf(strlen(arr) + arr, "\n|\x4\x4\x4\x4\x4|");
    } else {
        sprintf(strlen(arr) + arr, "\n|\x3\x3\x3\x3\x3|");
    }

    OledClear(OLED_COLOR_BLACK);
    OledDrawString(arr);
    OledUpdate();
}

// I multiply by a large number here because in its original base form won't
// give an accurate enough number--hence, I scale it up by multiplying by
// 10000. This gives enough accuracy for the timer.

void LEDOvenTimer(void) {
    int time = (int) (oven.cookInitTime);
    int eighthTime = (10000 * time) / 8;
    if (time == 0) {
        LEDS_SET(0b00000000);
    } else if ((10000 * oven.cookTimeLeft) <= eighthTime) {
        LEDS_SET(0b10000000);
    } else if ((10000 * oven.cookTimeLeft) <= 2 * eighthTime) {
        LEDS_SET(0b11000000);
    } else if ((10000 * oven.cookTimeLeft) <= 3 * eighthTime) {
        LEDS_SET(0b11100000);
    } else if ((10000 * oven.cookTimeLeft) <= 4 * eighthTime) {
        LEDS_SET(0b11110000);
    } else if ((10000 * oven.cookTimeLeft) <= 5 * eighthTime) {
        LEDS_SET(0b11111000);
    } else if ((10000 * oven.cookTimeLeft) <= 6 * eighthTime) {
        LEDS_SET(0b11111100);
    } else if ((10000 * oven.cookTimeLeft) <= 7 * eighthTime) {
        LEDS_SET(0b11111110);
    } else {
        LEDS_SET(0b11111111);
    }
}

void __ISR(_TIMER_1_VECTOR, ipl4auto) TimerInterrupt2Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 4;

    // this statement will prevent the timer from running at double speed.
    if (oven.ovenState != COUNTDOWN) {
        interrupts.event2Hz = TRUE;
    } else {
        Hz2Counter++;
        if (Hz2Counter == 2) {
            interrupts.event2Hz = TRUE;
            Hz2Counter = 0;
        }
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