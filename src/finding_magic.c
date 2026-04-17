#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "bitboards.h"
#include "move_generation.h"

U64 generateRookMask(int square) {
    U64 rookMask = 0;
    int r, f;

    int file = square % 8;
    int rank = square / 8;

    for (r = rank + 1; r < 7; r++) {
        rookMask |= (1ULL << (r * 8 + file));
    }
    
    for (r = rank - 1; r >= 1; r--) {
        rookMask |= (1ULL << (r * 8 + file));
    }

    for (f = file + 1; f < 7; f++) {
        rookMask |= (1ULL << (rank * 8 + f));
    }

    for (f = file - 1; f >= 1; f--) {
        rookMask |= (1ULL << (rank * 8 + f));
    }

    return rookMask;
}

U64 generateBishopMask(int square) {
    U64 bishopMask = 0;
    int r, f;

    int file = square % 8;
    int rank = square / 8;

    for (r = rank + 1, f = file + 1; f < 7 && r < 7; r++, f++) {
        bishopMask |= (1ULL << (r * 8 + f));
    }

    for (r = rank + 1, f = file - 1; f >= 1 && r < 7; r++, f--) {
        bishopMask |= (1ULL << (r * 8 + f));
    }

    for (r = rank - 1, f = file + 1; f < 7 && r >= 1; r--, f++) {
        bishopMask |= (1ULL << (r * 8 + f));
    }

    for (r = rank - 1, f = file - 1; f >= 1 && r >= 1; r--, f--) {
        bishopMask |= (1ULL << (r * 8 + f));
    }

    return bishopMask;
}

U64 generateQueenMask(int square) {
    return (generateBishopMask(square) | generateRookMask(square));
}

// we need 64 random number
U64 randomU64() {
    return ( ((U64)rand()) << 32) | ((U64)(rand()) );
}

// we do this because it gives you less percentage of 1
U64 smallRandomU64() {
    return (randomU64() & randomU64() & randomU64() );
}


// This is where the magic begin (A lot of cool stuff!)
int magicIndex(MagicEntry *entry, U64 blockers) {
    blockers = blockers & entry->mask;
    return ((blockers * entry->magic) >> entry->shift);
}


U64 getRookMove(RookMagic *rookMagic, int square, U64 occupied) {
    MagicEntry entry = rookMagic->entry[square];
    U64 blockers = occupied & entry.mask;
    int index = (blockers * entry.magic) >> entry.shift;

    return rookMagic->attacks[square][index];
}

U64 getBishopMove(BishopMagic *bishopMagic, int square, U64 occupied) {
    MagicEntry entry = bishopMagic->entry[square];
    U64 blockers = occupied & entry.mask;
    int index = (blockers * entry.magic) >> entry.shift;

    return bishopMagic->attacks[square][index];
}

// this converts mask into number (index on the bitboards)
int indexedMask(U64 mask, int *positions) {
    int i = 0;
    while(mask) {
        positions[i++] = popLSB(&mask);
    }
    return i;
}

// the reason why indexedMask is cool is that you can do this thing
// index & (1 << i) and as an index.
// In my opinion, very clean way of doing it.
// The point is, now it gives you ability to index the mask in U64
U64 indexedBlockers(int index, int n, int *positions) {
    U64 blockers = 0;

    for (int i = 0; i < n; i++) {
        if ((index & (1 << i)) > 0) {
            blockers |= (1ULL << positions[i]);
        }
    }
    return blockers;
}

// I figured this is a perfect place to use a function pointer :o
void makeAttackTable(U64 (*ptr_moveGen)(int, U64), int square, int count, int *positions, U64 *occupancies, U64 *attacks) {
    int limit = 1 << count;

    for (int i = 0; i < limit; i++) {
        occupancies[i] = indexedBlockers(i, count, positions);
        attacks[i] = ptr_moveGen(square, occupancies[i]);
    }
}

// https://www.chessprogramming.org/Looking_for_Magics
// felt like mentioning here too
// This is basically a hashmap
// Occupancies becomes a key --> hash it (make it a index) --> save attack accordingly
bool isThisMagical(U64 magic, int count, U64 *occupancies, U64 *attacks) {
    int limit = 1 << count;
    int shift = 64 - count;
    // this is how many bits that you need (for instance, if you have 9bits, then you can have 2^9 bits)
    
    U64 array[4096] = {0}; // this will store the thingy
    
    for (int i = 0; i < limit; i++) {
        int index = ((occupancies[i] * magic) >> shift); // extracting the relevant bits

        if (array[index] == 0) { // if that index is not used, store some data
            array[index] = attacks[i];
        } else if(array[index] != attacks[i]) { // if it is used and is not the same attack (meaning, it returns none unique index) break
            return 0;
        }
    }
    return 1;
}

// Note that bishop max: 2^9 = 512, rook max: 2^12 = 4096
int prepareMagic(RookMagic *rookMagic, BishopMagic *bishopMagic) {
    srand(time(NULL)); 

    for (int i = 0; i < 64; i++) {
        int bishopPositions[64] = {0};
        int rookPositions[64] = {0};

        U64 bishopOccupancies[512] = {0};
        U64 rookOccupancies[4096] = {0};


        bishopMagic->entry[i].mask = generateBishopMask(i);
        int bishopCount = indexedMask(bishopMagic->entry[i].mask, bishopPositions);
        bishopMagic->entry[i].shift = 64 - bishopCount;
        U64 bishopAttack[512] = {0};

        rookMagic->entry[i].mask = generateRookMask(i);
        int rookCount = indexedMask(rookMagic->entry[i].mask, rookPositions);
        rookMagic->entry[i].shift = 64 - rookCount;
        U64 rookAttack[4096] = {0};

        makeAttackTable(generateBishopMove, i, bishopCount, bishopPositions, bishopOccupancies, bishopAttack);
        makeAttackTable(generateRookMove, i, rookCount, rookPositions, rookOccupancies, rookAttack);

        while (1) {
            U64 magic = smallRandomU64();
            if ( (isThisMagical(magic, bishopCount, bishopOccupancies, bishopAttack)) ) {
                bishopMagic->entry[i].magic = magic;
                break;
            }
        }
        
        while (1) {
            U64 magic = smallRandomU64();
            if ( (isThisMagical(magic, rookCount, rookOccupancies, rookAttack)) ) {
                rookMagic->entry[i].magic = magic;
                break;
            }
        }

        // you need to reorder the index according to the hash...

        for (int j = 0; j < (1 << bishopCount); j++) {
            int index = (bishopOccupancies[j] * bishopMagic->entry[i].magic) >> bishopMagic->entry[i].shift;
            bishopMagic->attacks[i][index] = generateBishopMove(i, bishopOccupancies[j]);
        }

        for (int j = 0; j < (1 << rookCount); j++) {
            int index = (rookOccupancies[j] * rookMagic->entry[i].magic) >> rookMagic->entry[i].shift;
            rookMagic->attacks[i][index] = generateRookMove(i, rookOccupancies[j]);
        }

    }
    
    return 0;
}
