#include "UnixBOARD.h"
#include "Game.h"
#include "Player.h"

typedef struct {
	uint8_t inv[INVENTORY_SIZE]; // stores inventory
} Player;

static Player player;

/**
 * Adds the specified item to the player's inventory if the inventory isn't full.
 * @param item The item number to be stored: valid values are 0-255.
 * @return SUCCESS if the item was added, STANDARD_ERRROR if the item couldn't be added.
 */
int AddToInventory(uint8_t item) {
  int i = 0;
  while(i < INVENTORY_SIZE) {
 	  if(player.inv[i]==0 || player.inv[i]==item) {
 		  player.inv[i]=item;
 		  return SUCCESS;
 	  }
		i++;
  }
  return STANDARD_ERROR;
}

/**
 * Check if the given item exists in the player's inventory.
 * @param item The number of the item to be searched for: valid values are 0-255.
 * @return SUCCESS if it was found or STANDARD_ERROR if it wasn't.
 */
int FindInInventory(uint8_t item) {
  int i = 0;
  while(i<INVENTORY_SIZE) {
 	  if(player.inv[i] == item) {
 		  return SUCCESS;
 	  }
		i++;
  }
  return STANDARD_ERROR;
}
