// QUESTIONS:
// Is there an issue setting gData->hit = SomeHitStatus? I'm getting an assignment
// error despite seeing the comments tell me to assign gData->hit to a HitStatus.


// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library

// Microchip libraries

// User libraries
#include "Field.h"
#include "Protocol.h"

// **** Set any macros or preprocessor directives here ****
#define TRUE 1
#define FALSE 0

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****

// **** Declare any function prototypes here ****
uint8_t FieldDirectionHandler(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type, int l);

/**
 * FieldInit() will fill the passed field array with the data specified in positionData. Also the
 * lives for each boat are filled according to the `BoatLives` enum.
 * @param f The field to initialize.
 * @param p The data to initialize the entire field to, should be a member of enum
 *                     FieldPosition.
 */
void FieldInit(Field *f, FieldPosition p) {
    f->field[FIELD_ROWS][FIELD_COLS] = p;
    f->smallBoatLives = FIELD_BOAT_LIVES_SMALL;
    f->mediumBoatLives = FIELD_BOAT_LIVES_MEDIUM;
    f->largeBoatLives = FIELD_BOAT_LIVES_LARGE;
    f->hugeBoatLives = FIELD_BOAT_LIVES_HUGE;
}

/**
 * Retrieves the value at the specified field position.
 * @param f The Field being referenced
 * @param row The row-component of the location to retrieve
 * @param col The column-component of the location to retrieve
 * @return
 */
FieldPosition FieldAt(const Field *f, uint8_t row, uint8_t col) {
    return f->field[row][col];
}

/**
 * This function provides an interface for setting individual locations within a Field struct. This
 * is useful when FieldAddBoat() doesn't do exactly what you need. For example, if you'd like to use
 * FIELD_POSITION_CURSOR, this is the function to use.
 * 
 * @param f The Field to modify.
 * @param row The row-component of the location to modify
 * @param col The column-component of the location to modify
 * @param p The new value of the field location
 * @return The old value at that field location
 */
FieldPosition FieldSetLocation(Field *f, uint8_t row, uint8_t col, FieldPosition p) {
    f->field[row][col] = p;
    return f->field[row][col]; // why do you want to return the old location?
}

/**
 * FieldAddBoat() places a single ship on the player's field based on arguments 2-5. Arguments 2, 3
 * represent the x, y coordinates of the pivot point of the ship.  Argument 4 represents the
 * direction of the ship, and argument 5 is the length of the ship being placed. All spaces that
 * the boat would occupy are checked to be clear before the field is modified so that if the boat
 * can fit in the desired position, the field is modified as SUCCESS is returned. Otherwise the
 * field is unmodified and STANDARD_ERROR is returned. There is no hard-coded limit to how many
 * times a boat can be added to a field within this function.
 *
 * So this is valid test code:
 * {
 *   Field myField;
 *   FieldInit(&myField,FIELD_POSITION_EMPTY);
 *   FieldAddBoat(&myField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_MEDIUM);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_HUGE);
 *   FieldAddBoat(&myField, 0, 6, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_SMALL);
 * }
 *
 * should result in a field like:
 *  _ _ _ _ _ _ _ _
 * [ 3 3 3       3 ]
 * [ 4 4 4 4     3 ]
 * [             3 ]
 *  . . . . . . . .
 *
 * @param f The field to grab data from.
 * @param row The row that the boat will start from, valid range is from 0 and to FIELD_ROWS - 1.
 * @param col The column that the boat will start from, valid range is from 0 and to FIELD_COLS - 1.
 * @param dir The direction that the boat will face once places, from the BoatDirection enum.
 * @param boatType The type of boat to place. Relies on the FIELD_POSITION_*_BOAT values from the
 * FieldPosition enum.
 * @return TRUE for success, FALSE for failure
 */
uint8_t FieldAddBoat(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type) {
    switch (type) {
        case FIELD_BOAT_SMALL:
            return FieldDirectionHandler(f, row, col, dir, type, 3);
        case FIELD_BOAT_MEDIUM:
            return FieldDirectionHandler(f, row, col, dir, type, 4);
            break;
        case FIELD_BOAT_LARGE:
            return FieldDirectionHandler(f, row, col, dir, type, 5);
        case FIELD_BOAT_HUGE:
            return FieldDirectionHandler(f, row, col, dir, type, 6);
        default:
            return FALSE;
    }
}

/**
 * This function registers an attack at the gData coordinates on the provided field. This means that
 * 'f' is updated with a FIELD_POSITION_HIT or FIELD_POSITION_MISS depending on what was at the
 * coordinates indicated in 'gData'. 'gData' is also updated with the proper HitStatus value
 * depending on what happened AND the value of that field position BEFORE it was attacked. Finally
 * this function also reduces the lives for any boat that was hit from this attack.
 * @param f The field to check against and update.
 * @param gData The coordinates that were guessed. The HIT result is stored in gData->hit as an
 *               output.
 * @return The data that was stored at the field position indicated by gData before this attack.
 */
FieldPosition FieldRegisterEnemyAttack(Field *f, GuessData *gData) {
    FieldPosition guessedPosition = f->field[gData->row][gData->col];
    switch (guessedPosition) {
        case FIELD_POSITION_EMPTY:
            f->field[gData->row][gData->col] = FIELD_POSITION_MISS;
            gData->hit = HIT_MISS;
            return FIELD_POSITION_EMPTY;
        case FIELD_POSITION_SMALL_BOAT:
            if(f->smallBoatLives == 1) {
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                gData->hit = HIT_SUNK_SMALL_BOAT;
                f->smallBoatLives--;
                return FIELD_POSITION_SMALL_BOAT;
            } else if(f->smallBoatLives > 1) {
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                gData->hit = HIT_HIT;
                f->smallBoatLives--;
                return FIELD_POSITION_SMALL_BOAT;
            }
        case FIELD_POSITION_MEDIUM_BOAT:
            if(f->mediumBoatLives == 1) {
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                gData->hit = HIT_SUNK_MEDIUM_BOAT;
                f->mediumBoatLives--;
                return FIELD_POSITION_MEDIUM_BOAT;
            } else if(f->mediumBoatLives > 1) {
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                gData->hit = HIT_HIT;
                f->mediumBoatLives--;
                return FIELD_POSITION_MEDIUM_BOAT;
            }
        case FIELD_POSITION_LARGE_BOAT:
            if(f->largeBoatLives == 1) {
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                gData->hit = HIT_SUNK_LARGE_BOAT;
                f->largeBoatLives--;
                return FIELD_POSITION_LARGE_BOAT;
            } else if(f->largeBoatLives > 1) {
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                gData->hit = HIT_HIT;
                f->largeBoatLives--;
                return FIELD_POSITION_LARGE_BOAT;
            }
        case FIELD_POSITION_HUGE_BOAT:
            if(f->hugeBoatLives == 1) {
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                gData->hit = HIT_SUNK_SMALL_BOAT;
                f->hugeBoatLives--;
                return FIELD_POSITION_HUGE_BOAT;
            } else if(f->hugeBoatLives > 1) {
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                gData->hit = HIT_HIT;
                f->hugeBoatLives--;
                return FIELD_POSITION_HUGE_BOAT;
            }
        default:
            return FIELD_POSITION_UNKNOWN;
    }
}

/**
 * This function updates the FieldState representing the opponent's game board with whether the
 * guess indicated within gData was a hit or not. If it was a hit, then the field is updated with a
 * FIELD_POSITION_HIT at that position. If it was a miss, display a FIELD_POSITION_EMPTY instead, as
 * it is now known that there was no boat there. The FieldState struct also contains data on how
 * many lives each ship has. Each hit only reports if it was a hit on any boat or if a specific boat
 * was sunk, this function also clears a boats lives if it detects that the hit was a
 * HIT_SUNK_*_BOAT.
 * @param f The field to grab data from.
 * @param gData The coordinates that were guessed along with their HitStatus.
 * @return The previous value of that coordinate position in the field before the hit/miss was
 * registered.
 */
FieldPosition FieldUpdateKnowledge(Field *f, const GuessData *gData) {
    FieldPosition guess = FieldAt(f, gData->row, gData->col);
    switch(gData->hit) {
        case HIT_MISS:
            f->field[gData->row][gData->col] = FIELD_POSITION_EMPTY;
            gData->hit = HIT_MISS;
            break;
        case HIT_HIT:
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            gData->hit = HIT_HIT;
            break;
        case HIT_SUNK_SMALL_BOAT:
            f->field[gData->row][gData->col] = FIELD_POSITION_SMALL_BOAT;
            gData->hit = HIT_SUNK_SMALL_BOAT;
            break;
        case HIT_SUNK_MEDIUM_BOAT:
            f->field[gData->row][gData->col] = FIELD_POSITION_MEDIUM_BOAT;
            gData->hit = HIT_SUNK_MEDIUM_BOAT;
            break;
        case HIT_SUNK_LARGE_BOAT:
            f->field[gData->row][gData->col] = FIELD_POSITION_LARGE_BOAT;
            gData->hit = HIT_SUNK_LARGE_BOAT;
            break;
        case HIT_SUNK_HUGE_BOAT:
            f->field[gData->row][gData->col] = FIELD_POSITION_HUGE_BOAT;
            gData->hit = HIT_SUNK_HUGE_BOAT;
            break;
        default:
            f->field[gData->row][gData->col] = FIELD_POSITION_UNKNOWN;
            break;
    }
    return guess;
}

/**
 * This function returns the alive states of all 4 boats as a 4-bit bitfield (stored as a uint8).
 * The boats are ordered from smallest to largest starting at the least-significant bit. So that:
 * 0b00001010 indicates that the small boat and large boat are sunk, while the medium and huge boat
 * are still alive. See the BoatStatus enum for the bit arrangement.
 * @param f The field to grab data from.
 * @return A 4-bit value with each bit corresponding to whether each ship is alive or not.
 */
uint8_t FieldGetBoatStates(const Field *f) {
    uint8_t returnState = 0b0000;
    
    if(f->smallBoatLives != 0) {
        returnState |= FIELD_BOAT_STATUS_SMALL;
    }
    if(f->mediumBoatLives != 0) {
        returnState |= FIELD_BOAT_STATUS_MEDIUM;
    }
    if(f->largeBoatLives != 0) {
        returnState |= FIELD_BOAT_STATUS_LARGE;
    }
    if(f->hugeBoatLives != 0) {
        returnState |= FIELD_BOAT_STATUS_HUGE;
    } 
    
    return returnState;
}

/**
 * This is a helper function for FieldAddBoat().
 * 
 * @param f
 * @param row
 * @param col
 * @param dir
 * @param type
 * @return 
 */

uint8_t FieldDirectionHandler(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type, int l) {
    int length;
    length = l;
    int i, j;
    switch (dir) {
        case FIELD_BOAT_DIRECTION_NORTH:
            for (i = row; i > (row - length); i--) {
                if (f->field[i][col] != FIELD_POSITION_EMPTY) {
                    return FALSE;
                }
            }
            for (i = row; i > (row - length); i--) {
                f->field[i][col] = type;
            }
        case FIELD_BOAT_DIRECTION_EAST:
            for (j = col; j < (col + length); j++) {
                if (f->field[row][j] != FIELD_POSITION_EMPTY) {
                    return FALSE;
                }
            }
            for (j = col; j < (col + length); j++) {
                f->field[row][j] = type;
            }
            break;
        case FIELD_BOAT_DIRECTION_SOUTH:
            for (i = row; i < (row + length); i++) {
                if (f->field[i][col] != FIELD_POSITION_EMPTY) {
                    return FALSE;
                }
            }
            for (i = row; i < (row + length); i++) {
                f->field[i][col] = type;
            }
        case FIELD_BOAT_DIRECTION_WEST:
            for (j = col; j > (col - length); j--) {
                if (f->field[row][j] != FIELD_POSITION_EMPTY) {
                    return FALSE;
                }
            }
            for (j = col; j > (col + length); j--) {
                f->field[row][j] = type;
            }
        default:
            return FALSE;
    }
    return TRUE;
}