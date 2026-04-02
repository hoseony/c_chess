#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include "constants.h"
#include "types.h"
#include <stdbool.h>

U64 generateKnightMove(int square);
U64 generateRookMove(int square, U64 occupied);
U64 generateBishopMove(int square, U64 occupied);
U64 generateQueenMove(int square, U64 occupied);
U64 generateKingMove(int square);
U64 generateEnPassant();
U64 generateWhitePawnMove(int square, U64 occupied);
U64 generateBlackPawnMove(int square, U64 occupied);
U64 pawnPromotion(U64 board);
U64 generateWhitePawnAttack(int square);
U64 generateBlackPawnAttack(int square);
U64 blackAttackBoard(State p);
U64 whiteAttackBoard(State p);

#endif
