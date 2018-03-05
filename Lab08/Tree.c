// **** Include libraries here ****
// Standard C libraries
#include <Math.h>

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
    Node *root = malloc(sizeof(Node));
    
    root->data = *data;
    
    if(level == 1) {
        root->leftChild = NULL;
        root->rightChild = NULL;
        return root;
    }
    
    if(level > 1) {
        root->leftChild = TreeCreate(level-1, data+1);
        root->rightChild = TreeCreate(level-1, pow(2, level - 1) + data);
        if(root->leftChild == NULL || root->rightChild == NULL) {
            return NULL;
        } else {
            return root;
        }
    }
}