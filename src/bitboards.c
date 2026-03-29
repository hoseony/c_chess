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

int popLSB(U64 *board) {
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
// This initialize the starting position of the board;
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
    p.turn = WHITE;
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

// --------- move gen ----------

// Todo: magic bitboard

// Could be more efficient
// There should be better way of doing this
U64 generateKnightMove(int square, U64 occupied) {
    U64 knightMove = 0;
    U64 shifter = (1ULL << square); 

    // does not add invalid moves if knight is on edge of board 
    knightMove |= (shifter << 17) * !(((BIT_8_RANK | BIT_7_RANK) & shifter) || (BIT_H_FILE & shifter)); 
    knightMove |= (shifter << 15) * !(((BIT_8_RANK | BIT_7_RANK) & shifter) || (BIT_A_FILE & (shifter))); 
    knightMove |= (shifter << 6) * !((BIT_8_RANK & shifter) || ((BIT_A_FILE | BIT_B_FILE) & (shifter))); 
    knightMove |= (shifter << 10) * !((BIT_8_RANK & shifter) || ((BIT_G_FILE | BIT_H_FILE) & (shifter)));
    knightMove |= (shifter >> 17) * !(((BIT_1_RANK | BIT_2_RANK) & shifter) || (BIT_A_FILE & (shifter)));
    knightMove |= (shifter >> 15) * !(((BIT_1_RANK | BIT_2_RANK) & shifter) || (BIT_H_FILE & (shifter))); 
    knightMove |= (shifter >> 6) * !(((BIT_1_RANK & shifter) || ((BIT_G_FILE | BIT_H_FILE) & (shifter)))); 
    knightMove |= (shifter >> 10) * !((BIT_1_RANK & shifter) || ((BIT_A_FILE | BIT_B_FILE) & (shifter))); 

    return knightMove;
}

U64 generateRookMove(int square, U64 occupied) {
    U64 rookMove = 0;
    int r, f;

    int file = square % 8;
    int rank = square / 8;

    for (r = rank + 1; r < 8; r++) {
        rookMove |= (1ULL << (r * 8 + file));
        if (occupied & (1ULL << (r * 8 + file))) {
            break;
        }
    }
    
    for (r = rank - 1; r >= 0; r--) {
        rookMove |= (1ULL << (r * 8 + file));
        if (occupied & (1ULL << (r * 8 + file))) {
            break;
        }
    }

    for (f = file + 1; f < 8; f++) {
        rookMove |= (1ULL << (rank * 8 + f));
        if (occupied & (1ULL << (rank * 8 + f))) {
            break;
        }
    }

    for (f = file - 1; f >= 0; f--) {
        rookMove |= (1ULL << (rank * 8 + f));
        if (occupied & (1ULL << (rank * 8 + f))) {
            break;
        }
    }

    return rookMove;
}

U64 generateBishopMove(int square, U64 occupied) {
    U64 bishopMove = 0;
    int r, f;

    int file = square % 8;
    int rank = square / 8;

    for (r = rank + 1, f = file + 1; f < 8 && r < 8; r++, f++) {
        bishopMove |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) {
            break;
        }
    }

    for (r = rank + 1, f = file - 1; f >= 0 && r < 8; r++, f--) {
        bishopMove |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) {
            break;
        }
    }

    for (r = rank - 1, f = file + 1; f < 8 && r >= 0; r--, f++) {
        bishopMove |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) {
            break;
        }
    }

    for (r = rank - 1, f = file - 1; f >= 0 && r >= 0; r--, f--) {
        bishopMove |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) {
            break; }
    }

    return bishopMove;
}

U64 generateQueenMove(int square, U64 occupied) {
    return (generateBishopMove(square, occupied) | generateRookMove(square, occupied));
}


U64 generateKingMove(int square, U64 occupied) {
    U64 kingMove = 0;
    int file = square % 8;
    int rank = square /8;
    return kingMove;
}

U64 generateWhitePawnMove(U64 board, Move lastmove) {
    U64 whitePawnMove = 0;
    //U64 black_occ = black_occ(board);

    // capture
    // en-passan
    // direction
    // two move initial
    return whitePawnMove = 0;
}

U64 generateBlackPawnMove(U64 board) {
    U64 blackPawnMove = 0;
    // capture
    // en-passan
    // direction
    // two move initial
    return blackPawnMove;
}

U64 pawnPromotion(U64 board) {
    // I think this can be combined because white pawn 
    // cannot go to the rank 1 and black pawn cannot go to the 
    // rank 8 
    // but idk if you want to make it so that you can also play really weird Position
    // maybe you need to separate this.

    return 0;
}

// -------------------------------

void printGameBoard(State p) {
    for(int i = 0; i < 64; i++) {
        if (p.bp & (1ULL << i)) printf("♟︎"); 
        else if (p.bn & (1ULL << i)) printf("♞");
        else if (p.bb & (1ULL << i)) printf("♝");
        else if (p.br & (1ULL << i)) printf("♜");
        else if (p.bq & (1ULL << i)) printf("♛");
        else if (p.bk & (1ULL << i)) printf("♚");

        else if (p.wp & (1ULL << i)) printf("♙");
        else if (p.wn & (1ULL << i)) printf("♘");
        else if (p.wb & (1ULL << i)) printf("♗");
        else if (p.wr & (1ULL << i)) printf("♖");
        else if (p.wq & (1ULL << i)) printf("♕");
        else if (p.wk & (1ULL << i)) printf("♔");

        else printf(".");

        if ((i % 8) == 7) {
            printf("\n");
        }
    }
}

// ------------------------------
int main() {
    State p = initializeState();
    U64 occ = 0;
        //allOccupied(p);

    U64 testBit1 = 0x0000000800000000;
    U64 testBit2 = 0x1000000000000000;

    U64 temp = testBit1;
    int square = popLSB(&temp);

    printBitboard(generateRookMove(square, occ));
    printBitboard(generateBishopMove(square, occ));
    printBitboard(generateQueenMove(square, occ));

    // so the general idea will be 
    // U64 allMoves = 0;
    // while (temp)
    //      square = popLSB(temp)
    //      allMoves |= generateRookMove(square)
    // This will be computationally faster than the way we were doing previously
    return 0;
}


