#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "constants.h" 
#include "types.h" 

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

//------- Simple Bit Manipulation ---------
inline void putBit(U64 *board, Board_pos pos) {
    *board |= (1ULL << pos);
}

inline void removeBit(U64 *board, Board_pos pos) {
    *board &= ~(1ULL << pos);
}

inline void moveBit(U64 *board, Board_pos from, Board_pos to) {
    *board &= ~(1ULL << from); 
    putBit(board, to);
}

inline int indexShift(int r, int f) {
    return f + r * 8; 
}

inline int popLSB(U64 *board) {
    // this is a funny operation from gnu compiler that gets the LSB index
    // if you are interested in, check gnu documentation
    // https://gcc.gnu.org/onlinedocs/gcc/Bit-Operation-Builtins.html 
    int square = __builtin_ctzll(*board);

    // some DLD type thing
    *board &= *board - 1;
    return square;
}

// ------------- Print Board ----------------
// it prints the bit board
void printBitboard(U64 board) {
    printf("---------- bitboard ---------\n\n");

    for (int rank = 7; rank >= 0; rank--) {
        printf("  %1d   ", rank + 1);

        for (int file = 0; file < 8; file++) {
            U64 square = rank * 8 + file;
            int number = ((1ULL << square) & board)? 1 : 0;
            printf(" %d ", number);
        }
        printf("\n");
    }
    printf("\n      ");

    for (int file = 0; file < 8; file ++) {
        printf("%2c ", 'a' + file);
    }
    printf("\n\n");
}

// --------- Initialize Board ------------
// Initialize the starting position of the board;
State initializeState() {
    State p;
    p.wp = 0x000000000000FF00;
    p.wn = 0x0000000000000042;
    p.wb = 0x0000000000000024;
    p.wr = 0x0000000000000081;
    p.wq = 0x0000000000000008;
    p.wk = 0x0000000000000010;

    p.bp = 0x00FF000000000000;
    p.bn = 0x4200000000000000;
    p.bb = 0x2400000000000000;
    p.br = 0x8100000000000000;
    p.bq = 0x0800000000000000;
    p.bk = 0x1000000000000000;

    p.turn = SIDE_WHITE;

    p.castleState = 0b1111;
    p.fiftyMoveRule = 0;
    p.threeMoveRepetition = 0;

    return p;
}

// ---------Combined Board ------------
// These make occupied bitboards
U64 blackOccupied(State p) {
    return (p.bp | p.bn | p.bb | p.br | p.bq | p.bk);
}

U64 whiteOccupied(State p) {
    return (p.wp | p.wn | p.wb | p.wr | p.wq | p.wk);
}

U64 allOccupied(State p) {
    return (p.bp | p.bn | p.bb | p.br | p.bq | p.bk | p.wp | p.wn | p.wb | p.wr | p.wq | p.wk);
}

// -------------------------------

void printGameBoard(State p) {

    printf("------- chess -------\n");

    for(int i = 7; i >= 0; i--) {
        printf("      ");
        for(int j = 0; j < 8; j++) {
            if (p.wp & (1ULL << (i * 8 + j))) printf("♟︎"); 
            else if (p.wn & (1ULL << (i * 8 + j))) printf("♞");
            else if (p.wb & (1ULL << (i * 8 + j))) printf("♝");
            else if (p.wr & (1ULL << (i * 8 + j))) printf("♜");
            else if (p.wq & (1ULL << (i * 8 + j))) printf("♛");
            else if (p.wk & (1ULL << (i * 8 + j))) printf("♚");

            else if (p.bp & (1ULL << (i * 8 + j))) printf("♙");
            else if (p.bn & (1ULL << (i * 8 + j))) printf("♘");
            else if (p.bb & (1ULL << (i * 8 + j))) printf("♗");
            else if (p.br & (1ULL << (i * 8 + j))) printf("♖");
            else if (p.bq & (1ULL << (i * 8 + j))) printf("♕");
            else if (p.bk & (1ULL << (i * 8 + j))) printf("♔");
            else printf("·");
        }
    printf("\n");

    }

    printf("--- ");
    if(p.turn == SIDE_WHITE) {
        printf("White to move");
    } else {
        printf("Black to move");
    }
    printf(" ---\n\n");

/*
    printf("************ debug infos ***********\n");
    printf("    p.castleState: 0X%X\n", p.castleState);
    printf("    p.turn: %d\n", p.turn);
    printf("    p.fiftyMoveRule: %d\n", p.fiftyMoveRule);
    printf("    p.threeMoveRepetition: %d\n", p.threeMoveRepetition);
    printf("************************************\n");
*/
}
