#include "MatrixMath.h"

// Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/******************************************************************************
 * Matrix - Matrix Operations
 *****************************************************************************/

/**
 * MatrixMultiply performs a matrix-matrix multiplication operation on two 3x3
 * matrices and returns the result in the third argument.
 */
void MatrixMultiply(float mat1[3][3], float mat2[3][3], float result[3][3]) {
    for(int i = 1; i <= 3; i++) {
        for(int j = 1; j <= 3; j++) {
            for(int k = 1; k <= 3; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

/**
 * MatrixAdd performs a matrix addition operation on two 3x3 matrices and 
 * returns the result in the third argument.
 */
void MatrixAdd(float mat1[3][3], float mat2[3][3], float result[3][3]) {
    for(int i = 1; i <= 3; i++) {
        for(int j = 1; j <= 3; j++) {
            result[i][j] += mat1[i][j] + mat2[i][j];
        }
    }
}

/**
 * MatrixEquals checks if the two matrix arguments are equal. The result is
 * 0 if FALSE and 1 if TRUE to follow the standard C conventions of TRUE and
 * FALSE.
 */
int MatrixEquals(float mat1[3][3], float mat2[3][3]) {
    for(int i = 1; i <= 3; i++) {
        for(int j = 1; j <= 3; j++) {
            if(mat1[i][j] != mat2[i][j]) {
                return 0;
            }
        }
    }
    return 1;
}

/******************************************************************************
 * Matrix - Scalar Operations
 *****************************************************************************/

/**
 * MatrixScalarMultiply performs the multiplication of a matrix by a scalar.
 * The result is returned in the third argument.
 */
void MatrixScalarMultiply(float x, float mat[3][3], float result[3][3]) {
    for(int i = 1; i <= 3; i++) {
        for(int j = 1; j <= 3; j++) {
            result[i][j] = x * mat[i][j];
        }
    }
}

/**
 * MatrixScalarAdd performs the addition of a matrix by a scalar. The result
 * is returned in the third argument.
 */
void MatrixScalarAdd(float x, float mat[3][3], float result[3][3]) {
    
}

/******************************************************************************
 * Unary Matrix Operations
 *****************************************************************************/

/**
 * MatrixDeterminant calculates the determinant of a matrix and returns the
 * value as a float.
 */
float MatrixDeterminant(float mat[3][3]) {
    
}

/**
 * MatrixTrace calculates the trace of a matrix. The result is returned as a
 * float.
 */
float MatrixTrace(float mat[3][3]) {
    
}

/**
 * MatrixTranspose calculates the transpose of a matrix and returns the
 * result through the second argument
 */
void MatrixTranspose(float mat[3][3], float result[3][3]) {
    
}

/**
 * MatrixInverse calculates the inverse of a matrix and returns the
 * result through the second argument.
 */
void MatrixInverse(float mat[3][3], float result[3][3]) {
    
}

/**
 * MatrixPrint sends a 3x3 array to standard output with clean formatting.
 * The formatting does not need to look like the expected output given to you
 * in MatricMath.c but each element of the matrix must be separated and have
 * distinguishable position (more than a single line of output).
 */
void MatrixPrint(float mat[3][3]) {
    for(int i = 1; i <= 3; i++) {
        for(int j = 1; j <= 3; j++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

/******************************************************************************
 * Extra Credit Matrix Operations
 *****************************************************************************/

/**
 * Calculate the adjugate matrix of a 3x3 matrix. This is the transpose of the cofactor matrix.
 * @param mat The input 3x3 matrix.
 * @param result The output 3x3 matrix.
 */
void MatrixAdjugate(float mat[3][3], float result[3][3]) {
    
}