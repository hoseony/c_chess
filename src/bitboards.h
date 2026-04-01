#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "constants.h" 
#include "types.h" 
#include "move_generation.h"

//-------- function prototype ----------
void putBit(U64 *board, Board_pos pos);
void removeBit(U64 *board, Board_pos pos);
void moveBit(U64 *board, Board_pos start_pos, Board_pos target_pos);
int indexShift(int rank, int file); 
int popLSB(U64 *board);

void printBitboard(U64 board);
State initializeState();
U64 blackOccupied(State p);
U64 whiteOccupied(State p);
U64 allOccupied(State p);
void printGameBoard(State p);


#endif
