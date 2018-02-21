// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Leds.h"

// **** Set macros and preprocessor directives ****
#define LEFT 1
#define RIGHT 0
#define TRUE 1
#define FALSE 0

// **** Declare any datatypes here ****
typedef struct {
    uint8_t event;
    uint8_t value;
}TimerResult;

// **** Define global, module-level, or external variables here ****
TimerResult timerData;

// **** Declare function prototypes ****

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
    // still needs to implement switches and stuff.
    
    LEDS_INIT();
    int LED = 0x0001; // position
    int direction = RIGHT; // sets LED direction to the right. This will help
                           // determine the direction of the LED in if statements.
    timerData.event = FALSE;
    timerData.value = 0;
    while(1) {
        LEDS_SET(LED); // sets LED to the current one.
        // check left side
        if(direction == LEFT && LED == 0x0001) { 
            direction = RIGHT;
            LED = 0x0002;
        }
        // check right side
        // 0x0080 is the hex equivalent of 0b10000000
        else if(direction == RIGHT && LED == 0x0080) { 
            direction = LEFT;
            LED = 0x0040;
        }
        // else, just shift left or right
        else if(timerData.event = TRUE) {
            if(direction == LEFT) {
                LED >>= 1;
            } else {
                LED <<= 1;
            }
            timerData.event--;
        }
    }
    
    

    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/

    while (1);
}

/**
 * This is the interrupt for the Timer1 peripheral. During each call it increments a counter (the
 * value member of a module-level TimerResult struct). This counter is then checked against the
 * binary values of the four switches on the I/O Shield (where SW1 has a value of 1, SW2 has a value
 * of 2, etc.). If the current counter is greater than this switch value, then the event member of a
 * module-level TimerResult struct is set to true and the value member is cleared.
 */
void __ISR(_TIMER_1_VECTOR, IPL4AUTO) Timer1Handler(void) {
    // Clear the interrupt flag.
    INTClearFlag(INT_T1);
    (timerData.value)++;
    if(timerData.value > SWITCH_STATES()) {
        timerData.event = TRUE;
        timerData.value = 0;
    }
}