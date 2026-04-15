#include "types.h"

U64 generateRookMask(int square) {
    U64 rookMask = 0;
    int r, f;

    int file = square % 8;
    int rank = square / 8;

    for (r = rank + 1; r < 8; r++) {
        rookMask |= (1ULL << (r * 8 + file));
    }
    
    for (r = rank - 1; r >= 0; r--) {
        rookMask |= (1ULL << (r * 8 + file));
    }

    for (f = file + 1; f < 8; f++) {
        rookMask |= (1ULL << (rank * 8 + f));
    }

    for (f = file - 1; f >= 0; f--) {
        rookMask |= (1ULL << (rank * 8 + f));
    }

    return rookMask;
}

U64 generateBishopMask(int square) {
    U64 bishopMask = 0;
    int r, f;

    int file = square % 8;
    int rank = square / 8;

    for (r = rank + 1, f = file + 1; f < 8 && r < 8; r++, f++) {
        bishopMask |= (1ULL << (r * 8 + f));
    }

    for (r = rank + 1, f = file - 1; f >= 0 && r < 8; r++, f--) {
        bishopMask |= (1ULL << (r * 8 + f));
    }

    for (r = rank - 1, f = file + 1; f < 8 && r >= 0; r--, f++) {
        bishopMask |= (1ULL << (r * 8 + f));
    }

    for (r = rank - 1, f = file - 1; f >= 0 && r >= 0; r--, f--) {
        bishopMask |= (1ULL << (r * 8 + f));
    }

    return bishopMask;
}

U64 generateQueenMask(int square, U64 occupied) {
    return (generateBishopMask(square) | generateRookMask(square));
}
