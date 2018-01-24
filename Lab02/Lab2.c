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

#else

int their_main(void)
{
#endif // LAB2_TESTING

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    BOARD_Init();
    
    char command;
    
    printf("====================================\n");
    printf("Welcome to Andrew's Lab2 Calculator!\n");
    printf("====================================\n");
    printf("\n");
    printf("Choose a mathematical operation to perform (*,/,+,-,v,a,c,f,t,r): ");
    scanf("%c", &command);




    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     *****************************************************************************/
    while (1);
}

/********************************************************************************
 * Define the Absolute Value function here.
 ********************************************************************************/
double myabs(double i) {
    if(i < 0) {
        i = -i;
    }
    return i;
}

/*********************************************************************************
 * Define the Fahrenheit to Celsius function here.
 ********************************************************************************/
double cToF(double celsius) {
    double fahren = celsius * (9.0/5.0) + 32.0;
    return fahren;
}


/*********************************************************************************
 * Define the Celsius to Fahrenheit function here.
 ********************************************************************************/
double fToC(double fahren) {
    double celsius = (fahren - 32.0) * (5/9);
    return celsius;
}


/********************************************************************************
 * Define the Average function here.
 *******************************************************************************/
double average(double op1, double op2) {
    double avg = (op1 + op2) / 2;
    return avg;
}

/*********************************************************************************
 * Define the Tangent function that takes input in degrees (Note: Your tangent 
 * function relies on a call from the tangent function of math.h which uses 
 * radians).
 ********************************************************************************/

double tanDegrees(double degrees) {
    double radians = degrees * (M_PI / 180.0);
    return tan(radians);
}

/*********************************************************************************
 * Define the Round function here.
 * In order to receive the extra credit your calculator MUST ALSO CORRECTLY utilize
 * this function.
 ********************************************************************************/
double Round(double operand)
{
    return operand;
}

