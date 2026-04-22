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
#include "finding_magic.h"


U64 generateMoveFromTargetSquare(State *p, State *prev, int targetSquare, U64 occupied, U64 attackBoard, RookMagic *rookMagic, BishopMagic *bishopMagic);
void doMove(State *p, State *prev, int from, int to, bool perftQuestionMark);
int areYouMated(State *p, State *prev, RookMagic *rookMagic, BishopMagic *bishopMagic, U64 occupied);
int generateLegalMove(State state, State prevState, Move *moves, int maxMoves, RookMagic *rookMagic, BishopMagic *bishopMagic);
PerftResult perft(State p, State prev, int depth, RookMagic *rookMagic, BishopMagic *bishopMagic);
void runPerft(RookMagic *rookMagic, BishopMagic *bishopMagic);

#endif
