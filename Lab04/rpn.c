// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Stack.h"

// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****
#define TRUE 1
#define FALSE 0

// **** Declare function prototypes ****
/*
 * Function prototype for ProcessBackspaces() - This function should be defined
 * below main after the related comment.
 */
int ProcessBackspaces(char *rpn_sentence);
float calculate(char* operand, float O1, float O2);

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
    struct Stack s;
    char arr[60];
    char operands[] = {'+', '-', '/', '*', '\0'};
    int errorBool;
    float O1, O2, result;
    //char dlim[] = {' ', '-'};
    
    printf("Welcome to the RPN Calculator!\n");
    //char temp;
    while(TRUE) {
        printf("Type your RPN string using (+, -, /, *): \n");
        fgets(arr, 59, stdin);
        // Initialize the stack:
        StackInit(&s);
        // Tokenize the thing
        char* token = strtok(arr, " ");
        // Continue looping until token is NULL.
        while(token != NULL) {
            // the meat of the code goes under here.
            if(isdigit(token[0])) {
                errorBool = StackPush(&s, atof(token));
                if(errorBool != 1) {
                    printf("ERROR: No more room on the stack.");
                    break;
                }
            } else if(strchr(token[0], operands)) { // token is an operand, which means pop, pop, calculate, push
                if(StackGetSize(&s) >= 2) { // error checking for two elements
                    if(StackPop(&s, &O1) == 1 && StackPop(&s, &O2) == 1) {
                        errorBool = StackPush(&s, calculate(token[0], O1, O2));
                    } else {
                        printf("ERROR: Not enough operands before operator.\n");
                        break;
                    }
                }
            } else {
                printf("ERROR: Invalid character in RPN string.");
            }
            token = strtok(NULL, " ");
        }
        
        
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

float calculate(char* operand, float O1, float O2) {
    if(operand == '*') {
        return O1 * O2;
    } else if(operand == '/') {
        return O1/O2;
    } else if(operand == '+') {
        return O1 + O2;
    } else if(operand == '-') {
        return O1 - O2;
    }
}



