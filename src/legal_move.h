#ifndef LEGAL_MOVE_H
#define LEGAL_MOVE_H

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "types.h"
#include "move_generation.h"
#include "parse.h"
#include "bitboards.h"


U64 generateMoveFromTargetSquare(State *p, State *prev, int targetSquare, U64 occupied);
void doMove(State *p, State *prev, int from, int to, bool perftQuestionMark);
int areYouMated(State *p, State *prev);
int generateLegalMove(State state, State prevState, Move *moves, int maxMoves);
PerftResult perft(State p, State prev, int depth);
void runPerft();

#endif
