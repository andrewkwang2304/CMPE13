#include "UnixBOARD.h"
#include "Game.h"
#include "Player.h"
#include <stdio.h>
#include <string.h>

#define NULLCHAR '\0'

typedef struct {
  char roomTitle[GAME_MAX_ROOM_TITLE_LENGTH+1];
  char roomDesc[GAME_MAX_ROOM_DESC_LENGTH+1];
  uint8_t exitInfo[4]; // checks whether other rooms are available or not!
} RoomInfo;

// prototypes
void emptyOutRooms(void);
void PrintRoom(void);
void PrintCompass(void);
int ReadRoomFile(int roomBit);
void PrintDirectionHelper(void);


// Global variables
static RoomInfo thisRoom;

/**
 * These function transitions between rooms. Each call should return SUCCESS if the current room has
 * an exit in the correct direction and the new room was able to be loaded, and STANDARD_ERROR
 * otherwise.
 * @return SUCCESS if the room CAN be navigated to and changing the current room to that new room
 *         succeeded.
 */
int GameGoNorth(void) {
    if(GameGetCurrentRoomExits() & GAME_ROOM_EXIT_NORTH_EXISTS) {
      uint8_t northRoom = thisRoom.exitInfo[0];
      ReadRoomFile(northRoom);
      PrintDirectionHelper();
      return SUCCESS;
    }
    return STANDARD_ERROR;
}

/**
 * @see GameGoNorth
 */
int GameGoEast(void) {
  if(GameGetCurrentRoomExits() & GAME_ROOM_EXIT_EAST_EXISTS) {
    uint8_t eastRoom = thisRoom.exitInfo[1];
    ReadRoomFile(eastRoom);
    PrintDirectionHelper();
    return SUCCESS;
  }
  return STANDARD_ERROR;
}

/**
 * @see GameGoNorth
 */
int GameGoSouth(void) {
  if(GameGetCurrentRoomExits() & GAME_ROOM_EXIT_SOUTH_EXISTS) {
    uint8_t southRoom = thisRoom.exitInfo[2];
    ReadRoomFile(southRoom);
    PrintDirectionHelper();
    return SUCCESS;
  }
  return STANDARD_ERROR;
}

/**
 * @see GameGoNorth
 */
int GameGoWest(void) {
  if(GameGetCurrentRoomExits() & GAME_ROOM_EXIT_WEST_EXISTS) {
    uint8_t westRoom = thisRoom.exitInfo[3];
    ReadRoomFile(westRoom);
    PrintDirectionHelper();
    return SUCCESS;
  }
  return STANDARD_ERROR;
}

/**
 * This function sets up anything that needs to happen at the start of the game. This is just
 * setting the current room to STARTING_ROOM and loading it. It should return SUCCESS if it succeeds
 * and STANDARD_ERROR if it doesn't.
 * @return SUCCESS or STANDARD_ERROR
 */
int GameInit(void) {
  int roomStatus = ReadRoomFile(STARTING_ROOM);
  PrintDirectionHelper();
  return roomStatus;
}

/**
 * Copies the current room title as a NULL-terminated string into the provided character array.
 * Only a NULL-character is copied if there was an error so that the resultant output string
 * length is 0.
 * @param title A character array to copy the room title into. Should be GAME_MAX_ROOM_TITLE_LENGTH+1
 *             in length in order to allow for all possible titles to be copied into it.
 * @return The length of the string stored into `title`. Note that the actual number of chars
 *         written into `title` will be this value + 1 to account for the NULL terminating
 *         character.
 */
int GameGetCurrentRoomTitle(char *title) {
  strcpy(title, thisRoom.roomTitle);
	return(strlen(thisRoom.roomTitle));
}

/**
 * GetCurrentRoomDescription() copies the description of the current room into the argument desc as
 * a C-style string with a NULL-terminating character. The room description is guaranteed to be less
 * -than-or-equal to GAME_MAX_ROOM_DESC_LENGTH characters, so the provided argument must be at least
 * GAME_MAX_ROOM_DESC_LENGTH + 1 characters long. Only a NULL-character is copied if there was an
 * error so that the resultant output string length is 0.
 * @param desc A character array to copy the room description into.
 * @return The length of the string stored into `desc`. Note that the actual number of chars
 *          written into `desc` will be this value + 1 to account for the NULL terminating
 *          character.
 */
int GameGetCurrentRoomDescription(char *desc) {
  strcpy(desc, thisRoom.roomDesc);
	return(strlen(thisRoom.roomDesc));
}

/**
 * This function returns the exits from the current room in the lowest-four bits of the returned
 * uint8 in the order of NORTH, EAST, SOUTH, and WEST such that NORTH is in the MSB and WEST is in
 * the LSB. A bit value of 1 corresponds to there being a valid exit in that direction and a bit
 * value of 0 corresponds to there being no exit in that direction. The GameRoomExitFlags enum
 * provides bit-flags for checking the return value.
 *
 * @see GameRoomExitFlags
 *
 * @return a 4-bit bitfield signifying which exits are available to this room.
 */
uint8_t GameGetCurrentRoomExits(void) {
  uint8_t returnValue = 0;
  if(thisRoom.exitInfo[3] > 0) {
    returnValue |= GAME_ROOM_EXIT_WEST_EXISTS;
  }
  if(thisRoom.exitInfo[2] > 0) {
    returnValue |= GAME_ROOM_EXIT_SOUTH_EXISTS;
  }
  if(thisRoom.exitInfo[1] > 0) {
    returnValue |= GAME_ROOM_EXIT_EAST_EXISTS;
  }
  if(thisRoom.exitInfo[0] > 0) {
    returnValue |= GAME_ROOM_EXIT_NORTH_EXISTS;
  }
  return returnValue;
}

void emptyOutRooms(void) {
  int i = 0;
  while(i < GAME_MAX_ROOM_TITLE_LENGTH) {
		thisRoom.roomTitle[i] = NULLCHAR;
    i++;
	}
  i = 0;
	while(i < GAME_MAX_ROOM_DESC_LENGTH) {
		thisRoom.roomDesc[i] = NULLCHAR;
    i++;
	}
  i = 0;
	while(i < 4) {
		thisRoom.exitInfo[i] = 0;
    i++;
	}
}

void PrintRoom(void) {
  printf("\033[42m");
	printf("Title: %s\n", thisRoom.roomTitle);
	printf("\033[0m");
	printf("Description: %s\n", thisRoom.roomDesc);
}

void PrintCompass(void) {
  if (thisRoom.exitInfo[0]>0) {
    printf("\033[32m");
  } else {
    printf("\033[31m");
 	  printf("       NORTH\n");
  }

  if (thisRoom.exitInfo[3]>0) {
    printf("\033[32m");
  } else {
    printf("\033[31m");
  	printf("WEST");
  }

	if (thisRoom.exitInfo[1]>0) {
    printf("\033[32m");
  } else {
    printf("\033[31m");
	  printf("           EAST \n");
  }

	if (thisRoom.exitInfo[2]>0) {
    printf("\033[32m");
  } else {
    printf("\033[31m");
  	printf("       SOUTH \n");
  	printf("\033[0m");
  }
}

// ReadRoomFile will take in an int, which returns SUCCESS or STANDARD_ERROR

int ReadRoomFile(int roomBit) {
  	FILE *file;
  	char XOR, ch, fileArr[80];
    uint8_t XORi, i, j;
  	emptyOutRooms();
  	uint8_t MASTER_DECRYPTION = DECRYPTION_BASE_KEY + roomBit;

     	sprintf(fileArr,"RoomFiles/room%d.txt", roomBit);
     	file = fopen(fileArr,"rb");
     	if(file == NULL) {
        		perror("Error in opening file");
        		return STANDARD_ERROR;
     	}
     	fread(&i,1,1,file); XORi = i ^ MASTER_DECRYPTION; // title
  	for (j = 0; j < XORi; j++) {
  		ch = fgetc(file);
  		XOR = ch ^ MASTER_DECRYPTION;
  		thisRoom.roomTitle[j] = XOR;
  	}
  	thisRoom.roomTitle[j] = NULLCHAR;
  	fread(&i,1,1,file); XORi = i ^ MASTER_DECRYPTION; // # of items required
  	uint8_t num = XORi;
  	int temp = 1;
  	for (j = 0; j < num;j++) {
  		fread(&i,1,1,file);
  		XORi = i ^ MASTER_DECRYPTION; // items
  		if (FindInInventory(XORi) == STANDARD_ERROR) {
  			temp=0;
  		}
  	}
  	fread(&i,1,1,file); XORi = i ^ MASTER_DECRYPTION; // room length description
  	for (j = 0; j < XORi;j++) {
  		ch = fgetc(file);
  		XOR = ch ^ MASTER_DECRYPTION;
  		if (temp == 1) {
  			thisRoom.roomDesc[j] = XOR;
  		}
  	}
  	if (temp == 1) {
  		thisRoom.roomDesc[j] = NULLCHAR;
  	}
  	fread(&i, 1, 1, file); XORi = i ^ MASTER_DECRYPTION; // # of items
  	num = XORi;
  	for (j = 0; j < num; j++) {
  		fread(&i, 1, 1, file);
  		XORi = i ^ MASTER_DECRYPTION; // items
  		printf("Item contain %d\n",XORi);
  		if (temp == 1) {
  			AddToInventory(XORi);
  		}
  	}
  	fread(&i,1,1,file);
    XORi = i ^ MASTER_DECRYPTION;
    if (temp==1) {
      thisRoom.exitInfo[0]=XORi;
    }
  	fread(&i,1,1,file);
    XORi = i ^ MASTER_DECRYPTION;
    if (temp==1) {
      thisRoom.exitInfo[1]=XORi;
    }
  	fread(&i,1,1,file);
    XORi = i ^ MASTER_DECRYPTION;
    if (temp==1) {
      thisRoom.exitInfo[2]=XORi;
    }
  	fread(&i,1,1,file);
    XORi = i ^ MASTER_DECRYPTION;
    if (temp==1) {
      thisRoom.exitInfo[3]=XORi;
    }
  	if (temp == 0) {
  	fread(&i, 1, 1, file); XORi = i ^ MASTER_DECRYPTION; // # of items that are needed
  	num = XORi;
  	for (j = 0; j < num;j++) {
  		fread(&i, 1, 1, file);
  		XORi = i ^ MASTER_DECRYPTION; // items
  	}
  	fread(&i, 1, 1, file); XORi = i ^ MASTER_DECRYPTION; // description of room length
  	for (j = 0; j < XORi;j++) {
  		ch = fgetc(file);
  		XOR = ch ^ MASTER_DECRYPTION;
  		thisRoom.roomDesc[j]=XOR;
  	}
  	thisRoom.roomDesc[j]='\0';
  	fread(&i,1,1,file); XORi = i ^ MASTER_DECRYPTION; // number of items
  	num = XORi;
  	for (j = 0; j < num;j++) {
  		fread(&i,1,1,file);
  		XORi = i ^ MASTER_DECRYPTION; // items
  		AddToInventory(XORi);
  	}
  	fread(&i,1,1,file);
    XORi = i ^ MASTER_DECRYPTION;
    thisRoom.exitInfo[0]=XORi;

  	fread(&i,1,1,file);
    XORi = i ^ MASTER_DECRYPTION;
    thisRoom.exitInfo[1]=XORi;

  	fread(&i,1,1,file);
    XORi = i ^ MASTER_DECRYPTION;
    thisRoom.exitInfo[2]=XORi;

  	fread(&i,1,1,file);
    XORi = i ^ MASTER_DECRYPTION;
    thisRoom.exitInfo[3]=XORi;

  	}
  	fclose(file);
  	return SUCCESS;
  }

void PrintDirectionHelper(void) {
  printf("\033[2J"); // clear screen
  printf("\033[H"); // go to top line.
  printf("\033[1B"); // go down one line.
  PrintRoom();
  printf("\033[99B"); // go to bottom line.
  printf("\033[5A"); // go up five lines.
  PrintCompass();
}
