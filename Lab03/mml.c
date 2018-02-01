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

// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****

// **** Declare function prototypes ****

void MatrixPrint(float mat[3][3]) {
    for(int i = 1; i <= 3; i++) {
        for(int j = 1; j <= 3; j++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    BOARD_Init();

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    float M1[3][3] = {
        { 10, 4, 5 },
        { 1, 1, 2 },
        { 0, 0, 3 }
    };
    
    /*float M2[3][3] = {
        { 9, 3, 2 },
        { 0, 7, 8 },
        { 3, 2, 0 }
    };*/
    
    //float result[3][3];
    
    MatrixPrint(M1);

    

    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     *****************************************************************************/

    // Returning from main() is bad form in embedded environments. So we sit and spin.
    while (1);
}
