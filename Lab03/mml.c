// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//CMPE13 Support Library
#include "BOARD.h"


// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "MatrixMath.h"
// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****
int score = 0;
// **** Declare function prototypes ****
void MatrixSetZeroMML(float mat[3][3]);

int main() {
    BOARD_Init();

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    float M1[3][3] = {
        { 10, 4, 5},
        { 1, 1, 2},
        { 0, 0, 3}
    };

    float M2[3][3] = {
        { 9, 3, 2},
        { 0, 7, 8},
        { 3, 2, 0}
    };

    float M3[3][3] = {
        { 6, 1, 1},
        { 4, -2, 5},
        { 2, 8, 7}
    };

    float M4[3][3] = {
        { 10, 4, 5},
        { 1, 1, 2},
        { 0, 0, 3}
    };
    
    float A1[3][3] = {
        { 105, 68, 52},
        { 15, 14, 10},
        { 9, 6, 0}
    };
    
    float A2[3][3] = {
        { 57, 27, 20},
        { 51, 8, -8},
        { 39, 76, 68}
    };
    
    

    float result[3][3];

    // MatrixPrint test: CHECK
    printf("MatrixPrint test: \n");
    printf("\n");
    printf("M1:\n");
    MatrixPrint(M1);
    printf("M2:\n");
    MatrixPrint(M2);
    printf("M3:\n");
    MatrixPrint(M3);
    printf("M4:\n");
    MatrixPrint(M4);
    printf("========================\n");

    // MatrixMultiply test: CHECK
    
    MatrixMultiply(M1, M2, result);
    
    MatrixPrint(A1);
    printf("\n");
    MatrixPrint(result);
    printf("\n");
    
    if(result == A1) {
        score++;
    }

    MatrixMultiply(M2, M3, result);
    
    if(result == A2) {
        score++;
    }
    
    printf("PASSED (%d/2): MatrixMultiply()\n", score);
    score = 0;
    
    printf("========================\n");

    // SetZero test: CHECK
    printf("MatrixSetZero test: \n");
    printf("\n");
    MatrixSetZeroMML(result);
    MatrixPrint(result);
    printf("========================\n");

    // MatrixAdd test: CHECK
    printf("MatrixAdd test: \n");
    printf("\n");
    MatrixAdd(M1, M2, result);
    printf("Adding M1+M2:\n");
    MatrixPrint(result);
    printf("\n");
    MatrixAdd(M3, M4, result);
    printf("Adding M3+M4:\n");
    MatrixPrint(result);
    printf("========================\n");

    // MatrixEquals test: CHECK
    printf("MatrixEquals test: \n");
    printf("\n");
    printf("M1 ?= M2:\n");
    printf("Expected response 1: 0\n");
    printf("Actual response 1: %d\n", MatrixEquals(M1, M2));
    printf("\n");
    printf("M1 ?= M4:\n");
    printf("Expected response 2: 1\n");
    printf("Actual response 2: %d\n", MatrixEquals(M1, M4));
    printf("========================\n");

    // MatrixScalarMultiply test: CHECK
    printf("MatrixScalarMultiply test: \n");
    printf("\n");
    MatrixScalarMultiply(0.5, M3, result);
    printf("0.5 * M3:\n");
    MatrixPrint(result);
    printf("\n");
    MatrixScalarMultiply(-1.5, M2, result);
    printf("-1.5 * M2:\n");
    MatrixPrint(result);
    printf("========================\n");

    // MatrixScalarAdd test: CHECK
    printf("MatrixScalarAdd test: \n");
    printf("\n");
    MatrixScalarAdd(1, M1, result);
    printf("1 + M1:\n");
    MatrixPrint(result);
    printf("\n");
    MatrixScalarAdd(10, M2, result);
    printf("10 + M2:\n");
    MatrixPrint(result);
    printf("========================\n");

    // MatrixDeterminant test: CHECK
    printf("MatrixDeterminant test: \n");
    printf("\n");
    printf("Determinant of M3:\n");
    printf("%f\n", (double) MatrixDeterminant(M3));
    printf("\n");
    printf("Determinant of M2:\n");
    printf("%f\n", (double) MatrixDeterminant(M2));
    printf("========================\n");

    // MatrixTrace test: CHECK
    printf("MatrixTrace test: \n");
    printf("\n");
    printf("Trace of M1:\n");
    printf("%f\n", (double) MatrixTrace(M1));
    printf("\n");
    printf("Trace of M3:\n");
    printf("%f\n", (double) MatrixTrace(M3));
    printf("========================\n");

    // MatrixTranspose test: CHECK
    printf("MatrixTranspose test: \n");
    printf("\n");
    printf("Transpose of M2:\n");
    MatrixTranspose(M2, result);
    MatrixPrint(result);
    printf("\n");
    printf("Transpose of M1:\n");
    MatrixTranspose(M1, result);
    MatrixPrint(result);
    printf("========================\n");

    // MatrixInverse test:
    printf("MatrixInverse test: \n");
    printf("\n");
    MatrixInverse(M1, result);
    printf("Inverse of M1:\n");
    MatrixPrint(result);
    printf("\n");
    MatrixInverse(M2, result);
    printf("Inverse of M2:\n");
    MatrixPrint(result);
    printf("========================\n");

    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     *****************************************************************************/

    // Returning from main() is bad form in embedded environments. So we sit and spin.
    while (1);
}

void MatrixSetZeroMML(float mat[3][3]) {
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            mat[i][j] = 0;
        }
    }
}