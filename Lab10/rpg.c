<<<<<<< HEAD
// **** Include libraries here ****
// Standard libraries
#include <string.h>
#include <math.h>
#include <stdio.h>

//CMPE13 Support Library
#include "UnixBOARD.h"



// User libraries
#include "Game.h"
#include "Player.h"

// **** Set any macros or preprocessor directives here ****
#define RESET_DIRECTION ' ';
// **** Declare any data types here ****

// **** Define any global or external variables here ****
char input, direction;
int status = 0;

// **** Declare any function prototypes here ****

int main() {


    /******************************** Your custom code goes below here ********************************/
    printf("\033[2J"); // clear the screen.

    if (GameInit() != SUCCESS) { // Init check
      FATAL_ERROR();
    }

    while(1) {
      status = 0;
      direction = RESET_DIRECTION;
      printf("\033[5B");
    	printf("\nEnter Direction to TRAVEL (n, e, s, w) or q to quit followed by enter:");
      while(input != EOF && (input = getchar()) != '\n' ) {
    		if(input == 'n' || input == 's' || input == 'e' || input == 'w' || input == 'q') {
           direction = input;
        }
    	}
      switch(direction) {
        case 'n' :
    			status = GameGoNorth();
    			break;
        case 's' :
      		status = GameGoSouth();
      		break;
    		case 'e' :
    			status = GameGoEast();
    			break;
    		case 'w' :
    			status = GameGoWest();
    			break;
    		case 'q' :
    		  exit(0);
          break;
        default:
          printf("Direction case unsuccessful in main() switch statement.\n");
          break;
      }
      if(status != SUCCESS) {
        printf("Invalid direction.\n");
      }
    }


    /**************************************************************************************************/
}
