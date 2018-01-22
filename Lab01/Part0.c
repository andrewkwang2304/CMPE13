/*
 * File:   Part0.c
 * Author: andrewkwang
 *
 * Created on January 14, 2018, 3:08 PM
 */


#include "xc.h"
#include "BOARD.h"
#include <stdio.h>

int main(void) {
    BOARD_Init();
    printf("Hello World\n");
    while(1);
}
