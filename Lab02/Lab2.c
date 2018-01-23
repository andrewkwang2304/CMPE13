// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <math.h>

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****

// **** Declare function prototypes ****
double Round(double operand);

// If this code is used for testing, rename main to something we can call from our testing code.
#ifndef LAB2_TESTING

int main(void)
{
    BOARD_Init();
    
    char i;
    
    printf("================================\n");
    printf("Welcome to the Lab2 Calculator!\n");
    printf("================================\n");
    printf("\n");
    
    printf("Choose a mathematical operation:\n");
    printf("   '*' - Performs multiplication on"
            " two operands.\n");
    printf("   '/' - Performs division on two operands, operand 1 divided by operand 2.\n");
    printf("   '+' - Performs addition on two operands.\n");
    printf("   '-' - Performs subtraction on two operands, subtracting the second operand from the first.\n");
    printf("   'v' - The Average function returns the average of its two arguments.\n");
    printf("   'a' - Absolute value calculates the absolute value of its argument.\n");
    printf("   'c' - The Celsius function treats its argument as a value in degrees Fahrenheit and converts it to degrees Celsius.\n");
    printf("   'f' - The Fahrenheit function treats its argument as a value in degrees Celsius and converts it to degrees Fahrenheit.\n");
    printf("   't' - The Tangent function takes in a value in degrees and calculates the tangent value and returns the result.\n");
    printf("   'r' - The Round function rounds a number down if its fractional part is less than 0.5, and rounds up otherwise.\n");
    printf("\n");
    scanf("Enter an operation: %c\n", &i);
    printf("You put %c as your input.\n", i);
#else

int their_main(void)
{
#endif // LAB2_TESTING

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    




    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     *****************************************************************************/
    while (1);
}

/********************************************************************************
 * Define the Absolute Value function here.
 ********************************************************************************/



/*********************************************************************************
 * Define the Fahrenheit to Celsius function here.
 ********************************************************************************/



/*********************************************************************************
 * Define the Celsius to Fahrenheit function here.
 ********************************************************************************/



/********************************************************************************
 * Define the Average function here.
 *******************************************************************************/



/*********************************************************************************
 * Define the Tangent function that takes input in degrees (Note: Your tangent 
 * function relies on a call from the tangent function of math.h which uses 
 * radians).
 ********************************************************************************/

/*********************************************************************************
 * Define the Round function here.
 * In order to receive the extra credit your calculator MUST ALSO CORRECTLY utilize
 * this function.
 ********************************************************************************/
double Round(double operand)
{
    return operand;
}

