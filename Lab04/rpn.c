// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Stack.h"

// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****

// **** Declare function prototypes ****
/*
 * Function prototype for ProcessBackspaces() - This function should be defined
 * below main after the related comment.
 */
int ProcessBackspaces(char *rpn_sentence);

// If this code is used for testing, rename main to something we can call from our testing code. We
// also skip all processor configuration and initialization code.
#ifndef LAB4_TESTING


int main() {
    BOARD_Init();
#else
int their_main(void)
{
#endif // LAB4_TESTING

    /******************************** Your custom code goes below here *******************************/
    
    //struct Stack s;
    
    // TEST CONSTANTS UNDER HERE
    //int counter = 0;
        // StackInit
    /*StackInit(*s);
    if(s.currentItemIndex == -1 && s.initialized == TRUE) {
        printf("StackInit PASSED: (2/2)");
    } else if(s.currentItemIndex != -1 && s.initialized == TRUE) {
        printf("StackInit PASSED: (1/2)");
    } else if(s.currentItemIndex == -1 && s.initialized != TRUE) {
        printf("StackInit PASSED: (1/2)");
    } else {
        printf("StackInit PASSED: (0/2)");
    }*/
    
        // StackPush
    /*float SPInput1 = 23.4;
    float SPInput2 = 76.1;
    StackPush(&s, SPInput1);
    if(*(&s + s.currentItemIndex) == 23.4) {
        counter++;
    }
    StackPush(&s, SPInput2);
    if(s[s.currentItemIndex + 2] == 76.1) {
        counter++;
    }
    printf("StackPush PASSED: (%d/2)", counter);
    */
        // StackPop
    
    
    // CALCULATE RESULT
    
    
    // OUTPUT TEST RESULTS


    // RPN
    
    printf("Welcome to the RPN Calculator!\n");
    int i = 0, size;
    char arr[500];
    //char temp;
    while(TRUE) {
        scanf("%[^\n]", &arr);
    }
    /*************************************************************************************************/

    // You can never return from main() in an embedded system (one that lacks an operating system).
    // This will result in the processor restarting, which is almost certainly not what you want!
    while (1);
}

/**
 * Extra Credit: Define ProcessBackspaces() here - This function should read through an array of
 * characters and when a backspace character is read it should replace the preceding character with
 * the following character. It should be able to handle multiple repeated backspaces and also
 * strings with more backspaces than characters. It should be able to handle strings that are at
 * least 256 characters in length.
 * @param rpn_sentence The string that will be processed for backspaces. This string is modified in
 *                     place.
 * @return Returns the size of the resultant string in `rpn_sentence`.
 */
int ProcessBackspaces(char *rpn_sentence) {
    return 0;
}



