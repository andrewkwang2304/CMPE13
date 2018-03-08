// **** Include libraries here ****
// Standard C libraries
#include <Math.h>
#include <stdio.h>

//CMPE13 Support Library
#include "BOARD.h"
#include "Tree.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set any macros or preprocessor directives here ****

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****

// **** Declare any function prototypes here ****

Node *TreeCreate(int level, const char *data) {
    Node *root = malloc(sizeof (Node));

    root->data = *data;



    if (level == 1) { // base case.
        root->leftChild = NULL;
        root->rightChild = NULL;
        return root;
    }

    if (level > 1) { // while not at the very bottom of the BST.
        root->leftChild = TreeCreate(level - 1, data + 1); // traverse the left side
        root->rightChild = TreeCreate(level - 1, (int) pow(2, level - 1) + data); // traverse the right side
        if (root->leftChild == NULL || root->rightChild == NULL) {
            return NULL;
        } else {
            return root;
        }
    }
    return root;
}