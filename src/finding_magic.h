#ifndef FINDINGMAGIC_H
#define FINDINGMAGIC_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "types.h"
#include "bitboards.h"
#include "move_generation.h"

U64 generateRookMask(int square);
U64 generateBishopMask(int square);
U64 generateQueenMask(int square);
U64 randomU64();
U64 smallRandomU64();
int magicIndex(MagicEntry *entry, U64 blockers);
U64 getRookMove(RookMagic *rookMagic, int square, U64 occupied);
U64 getBishopMove(BishopMagic *bishopMagic, int square, U64 occupied);
int indexedMask(U64 mask, int *positions);
U64 indexedBlcokers(int index, int n, int *positions);
void makeAttackTable(U64 (*ptr_moveGen)(int, U64), int square, int count, int *positions, U64 *occupancies, U64 *attacks);
bool isThisMagical(U64 magic, int count, U64 *occupancies, U64 *attacks);
int prepareMagic(RookMagic *rookMagic, BishopMagic *bishopMagic);

#endif
