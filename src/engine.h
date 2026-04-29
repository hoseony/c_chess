#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "bitboards.h"
#include "move_generation.h"
#include "legal_move.h"
#include "finding_magic.h"

int pieceSquareTable(U64 board, int pieceSquareTable[64], int turn);
int positionEvaluation(State p);
int negamax(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic);
Move negmaxBestMove(State *p, State prev, int depth, RookMagic *rookMagic, BishopMagic *bishopMagic);
int qsearch(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic);
int MVVLVA(Move m, State *p);

#endif
