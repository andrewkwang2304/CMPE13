#include "xc.h"
#include "LinkedList.h"
#include "BOARD.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int CompareStrings(ListItem *firstItem, ListItem *secondItem);

/**
 * This function starts a new linked list. Given an allocated pointer to data it will return a
 * pointer for a malloc()ed ListItem struct. If malloc() fails for any reason, then this function
 * returns NULL otherwise it should return a pointer to this new list item. data can be NULL.
 *
 * @param data The data to be stored in the first ListItem in this new list. Can be any valid 
 *             pointer value.
 * @return A pointer to the malloc()'d ListItem. May be NULL if an error occured.
 */
ListItem *LinkedListNew(char *data) {
    ListItem *newList = (ListItem*) malloc(sizeof (ListItem));
    if (newList == NULL) {
        return NULL;
    }
    newList->data = data;
    newList->nextItem = NULL;
    return newList;
}

/**
 * This function will remove a list item from the linked list and free() the memory that the
 * ListItem struct was using. It doesn't, however, free() the data pointer and instead returns it
 * so that the calling code can manage it.  If passed a pointer to NULL, LinkedListRemove() should
 * return NULL to signal an error.
 *
 * @param item The ListItem to remove from the list.
 * @return The data pointer from the removed item. May be NULL.
 */
char *LinkedListRemove(ListItem *item) {
    if (item == NULL) {
        return NULL;
    } else if (item->nextItem == NULL) {
        item->nextItem->previousItem = item->previousItem;
    } else if (item->previousItem == NULL) {
        item->previousItem->nextItem = item->nextItem;
    }
    char *c = item->data;
    free(item);
    return c;
}

/**
 * This function returns the total size of the linked list. This means that even if it is passed a
 * ListItem that is not at the head of the list, it should still return the total number of
 * ListItems in the list. A NULL argument will result in 0 being returned.
 *
 * @param list An item in the list to be sized.
 * @return The number of ListItems in the list (0 if `list` was NULL).
 */
int LinkedListSize(ListItem *list) {
    //have a temp to hold the list
    //initialize a counter to 1.
    //loop until /0, add to counter.
    //loop until head, add to counter.

    if (list == NULL) {
        return 0;
    }

    int counter = 1; // we set at 1 to account for the current position.
    ListItem *tempList = list;
    while (tempList->nextItem != NULL) {
        tempList = tempList->nextItem;
        counter++;
    }

    tempList = list; // reset the list position.

    while (tempList->previousItem != NULL) {
        tempList = tempList->previousItem;
        counter++;
    }
    return counter;
}

/**
 * This function returns the head of a list given some element in the list. If it is passed NULL,
 * it will just return NULL. If given the head of the list it will just return the pointer to the
 * head anyways for consistency.
 *
 * @param list An element in a list.
 * @return The first element in the list. Or NULL if provided an invalid list.
 */
ListItem *LinkedListGetFirst(ListItem *list) {
    ListItem *tempList = list;
    if (list == NULL) {
        return NULL;
    }

    while (tempList->previousItem != NULL) {
        tempList = tempList->previousItem;
        if (tempList == NULL && tempList->previousItem != NULL) {
            tempList = tempList->previousItem;
        }
    }
    return tempList;
}

/**
 * This function allocates a new ListItem containing data and inserts it into the list directly
 * after item. It rearranges the pointers of other elements in the list to make this happen. If
 * passed a NULL item, InsertAfter() should still create a new ListItem, just with no previousItem.
 * It returns NULL if it can't malloc() a new ListItem, otherwise it returns a pointer to the new
 * item. The data parameter is also allowed to be NULL.
 *
 * @param item The ListItem that will be before the newly-created ListItem.
 * @param data The data the new ListItem will point to.
 * @return A pointer to the newly-malloc()'d ListItem.
 */
ListItem *LinkedListCreateAfter(ListItem *item, char *data) {
    if (item->data == NULL) {
        return NULL;
    }
    if (item == NULL) { // if no items are on the list
        item = LinkedListNew(data);
        if (item == NULL) {
            return NULL;
        }
        return item;
    } else if (item->nextItem == NULL) { // if tail
        item->nextItem = LinkedListNew(data);
        if (item == NULL) {
            return NULL;
        }
        item->nextItem->previousItem = item;
        item->nextItem->nextItem = NULL;
        return item->nextItem;
    } else { // if in between
        ListItem *appendedItem = LinkedListNew(data);

        // rearranging the pointers...
        appendedItem->nextItem = item->nextItem;
        appendedItem->previousItem = item;
        item->nextItem = appendedItem;
        appendedItem->nextItem->previousItem = appendedItem;
        return appendedItem;
    }

}

/**
 * LinkedListSwapData() switches the data pointers of the two provided ListItems. This is most
 * useful when trying to reorder ListItems but when you want to preserve their location. It is used
 * within LinkedListSort() for swapping items, but probably isn't too useful otherwise. This
 * function should return STANDARD_ERROR if either arguments are NULL, otherwise it should return
 * SUCCESS. If one or both of the data pointers are NULL in the given ListItems, it still does
 * perform the swap and returns SUCCESS.
 *
 * @param firstItem One of the items whose data will be swapped.
 * @param secondItem Another item whose data will be swapped.
 * @return SUCCESS if the swap worked or STANDARD_ERROR if it failed.
 */
int LinkedListSwapData(ListItem *firstItem, ListItem *secondItem) {
    if (firstItem == NULL || secondItem == NULL || (firstItem == NULL && secondItem == NULL)) {
        return STANDARD_ERROR;
    }
    char *temp;
    temp = firstItem->data;
    firstItem->data = secondItem->data;
    secondItem->data = temp;
    return SUCCESS;
}

/**
 * LinkedListSort() performs a selection sort on list to sort the elements into descending order. It
 * makes no guarantees of the addresses of the list items after sorting, so any ListItem referenced
 * before a call to LinkedListSort() and after may contain different data as only the data pointers
 * for the ListItems in the list are swapped. This function sorts the strings in ascending order
 * first by size (with NULL data pointers counting as 0-length strings) and then alphabetically
 * ascending order. So the list [dog, cat, duck, goat, NULL] will be sorted to [NULL, cat, dog,
 * duck, goat]. LinkedListSort() returns SUCCESS if sorting was possible. If passed a NULL pointer
 * for either argument, it will do nothing and return STANDARD_ERROR.
 *
 * @param list Any element in the list to sort.
 * @return SUCCESS if successful or STANDARD_ERROR is passed NULL pointers.
 */

int LinkedListSort(ListItem *list) {
    list = LinkedListGetFirst(list);
    if (list == NULL) {
        return STANDARD_ERROR;
    }
    if (list->nextItem == NULL) {
        return SUCCESS;
    } else {
        ListItem *temp = list->nextItem; // two vars to compare; list and temp
        for (; list->nextItem != NULL; list = list->nextItem) { // does this move forward prematurely?
            for (temp = list->nextItem; temp != NULL; temp = temp->nextItem) {
                if (CompareStrings(list, temp) > 0) {
                    LinkedListSwapData(list, temp);
                }
            }
        }
        return SUCCESS;
    }
    return SUCCESS;
}

/**
 * LinkedListPrint() prints out the complete list to stdout. This function prints out the given
 * list, starting at the head if the provided pointer is not the head of the list, like "[STRING1,
 * STRING2, ... ]" If LinkedListPrint() is called with a NULL list it does nothing, returning
 * STANDARD_ERROR. If passed a valid pointer, prints the list and returns SUCCESS.
 *
 * @param list Any element in the list to print.
 * @return SUCCESS or STANDARD_ERROR if passed NULL pointers.
 */
int LinkedListPrint(ListItem *list) {
    if (list == NULL) {
        return STANDARD_ERROR;
    }
    list = LinkedListGetFirst(list);
    ListItem *temp = list->nextItem;
    fprintf(stdout, "[%s", list->data);
    while (temp != NULL) {
        fprintf(stdout, " %s", temp->data);
        temp = temp->nextItem;
    }
    fprintf(stdout, "]\n");
    return SUCCESS;
}

// This is the CompareStrings helper function.

static int CompareStrings(ListItem *firstItem, ListItem *secondItem) {
    if (secondItem->data == NULL) {
        return 1;
    } else if (firstItem->data != NULL && secondItem->data != NULL) {
        if (strlen(firstItem->data) > strlen(secondItem->data)) {
            return 1;
        }
        else if (strlen(firstItem->data) == strlen(secondItem->data)) {
            if (strcmp(firstItem->data, secondItem->data) > 0) {
                return 1;
            } else if (strcmp(firstItem->data, secondItem->data) < 0) {
                return -1;
            }
        }
    } else {
        return -1;
    }
    return -1;
}