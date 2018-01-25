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
double AbsoluteValue(double i);
double CelsiusToFahrenheit(double celsius);
double FahrenheitToCelsius(double fahren);
double Round(double operand);
double Average(double op1, double op2);
double Tangent(double degrees);
double Round(double operand);

// If this code is used for testing, rename main to something we can call from our testing code.
#ifndef LAB2_TESTING

int main(void) {

#else

int their_main(void) {
#endif // LAB2_TESTING

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    BOARD_Init();

    char command;
    double operand1;
    double operand2;
    char dummyNewLine;

    printf("====================================\n");
    printf("Welcome to Andrew's Lab2 Calculator!\n");
    printf("====================================\n");

    while (1) {
        printf("Choose a mathematical operation to perform (*,/,+,-,v,a,c,f,t,r): ");
        scanf("%c%c", &command, &dummyNewLine);

        switch (command) {
            case '*':
                // code to be executed if command is equal to '*'
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Enter the second operand: ");
                scanf("%lf%c", &operand2, &dummyNewLine);
                printf("Result of (%f * %f): %f\n", operand1, operand2, (operand1 * operand2));
                break;

            case '/':
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Enter the second operand: ");
                scanf("%lf%c", &operand2, &dummyNewLine);
                printf("Result of (%f / %f): %f\n", operand1, operand2, (operand1 / operand2));
                break;

            case '+':
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Enter the second operand: ");
                scanf("%lf%c", &operand2, &dummyNewLine);
                printf("Result of (%f + %f): %f\n", operand1, operand2, (operand1 + operand2));
                break;

            case '-':
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Enter the second operand: ");
                scanf("%lf%c", &operand2, &dummyNewLine);
                printf("Result of (%f - %f): %f\n", operand1, operand2, (operand1 - operand2));
                break;

            case 'v':
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Enter the second operand: ");
                scanf("%lf%c", &operand2, &dummyNewLine);
                printf("Result of Average(%f, %f): %f\n", operand1, operand2, Average(operand1, operand2));
                break;

            case 'a':
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Result of AbsoluteValue(%f): %f\n", operand1, AbsoluteValue(operand1));
                break;

            case 'c':
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Result of %f deg F -> C: %f\n", operand1, FahrenheitToCelsius(operand1));
                break;

            case 'f':
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Result of %f deg C -> F: %f\n", operand1, CelsiusToFahrenheit(operand1));
                break;

            case 't':
                printf("Enter the first operand: ");
                scanf("%lf%c", &operand1, &dummyNewLine);
                printf("Result of tan(%f): %f\n", operand1, Tangent(operand1));
                break;

            case 'r':
                break;

            default:
                fprintf(stderr, "\'%c\' is not one of the listed commands.\n", command);
        }
    }





    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     *****************************************************************************/
    while (1);
}

/********************************************************************************
 * Define the Absolute Value function here.
 ********************************************************************************/
double AbsoluteValue(double i) {
    if (i < 0) {
        i = -i;
    }
    return i;
}

/*********************************************************************************
 * Define the Fahrenheit to Celsius function here.
 ********************************************************************************/
double CelsiusToFahrenheit(double celsius) {
    double fahren = celsius * (9.0 / 5.0) + 32.0;
    return fahren;
}

/*********************************************************************************
 * Define the Celsius to Fahrenheit function here.
 ********************************************************************************/
double FahrenheitToCelsius(double fahren) {
    double celsius = (fahren - 32.0)*(5.0 / 9.0);
    return celsius;
}

/********************************************************************************
 * Define the Average function here.
 *******************************************************************************/
double Average(double op1, double op2) {
    double avg = (op1 + op2) / 2;
    return avg;
}

/*********************************************************************************
 * Define the Tangent function that takes input in degrees (Note: Your tangent 
 * function relies on a call from the tangent function of math.h which uses 
 * radians).
 ********************************************************************************/
double Tangent(double degrees) {
    double radians = degrees * (M_PI / 180.0);
    return tan(radians);
}

/*********************************************************************************
 * Define the Round function here.
 * In order to receive the extra credit your calculator MUST ALSO CORRECTLY utilize
 * this function.
 ********************************************************************************/
double Round(double operand) {
    return operand;
}

