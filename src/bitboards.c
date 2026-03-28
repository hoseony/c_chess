#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "constants.h" 
#include "types.h" 

//-------- function prototype ----------
void put_bit(U64 *board, Board_pos pos);
void remove_bit(U64 *board, Board_pos pos);
void move_bit(U64 *board, Board_pos start_pos, Board_pos target_pos);
// so you are inputing board as &board type
void print_bb(U64 board);
int index_shift(int, int); 

//------- Simple Bit Manipulation ---------
inline void put_bit(U64 *board, Board_pos pos) {
    *board |= (1ULL << pos);
}

inline void remove_bit(U64 *board, Board_pos pos) {
    *board &= ~(1ULL << pos);
}

inline void move_bit(U64 *board, Board_pos from, Board_pos to) {
    *board &= ~(1ULL << from); 
    put_bit(board, to);
}

inline int index_shift(int r, int f) {
    return f + r * 8; 
}

// ------------- Print Board ----------------
void print_bb(U64 board) {
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
State initialize_state() {
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
U64 black_occ(State *p) {
    return (p->bp | p->bn | p->bb | p->br | p->bq | p->bk);
}

U64 white_occ(State *p) {
    return (p->wp | p->wn | p->wb | p->wr | p->wq | p->wk);
}

U64 all_occ(State *p) {
    return (p->bp | p->bn | p->bb | p->br | p->bq | p->bk | p->wp | p->wn | p->wb | p->wr | p->wq | p->wk);
}

// --------- move gen ----------

// *MG for move generation
// ** also we should be able to like check move for one specific piece
//
// I ma not sure yet if we should make this for each piece or integrate this on our MG functions
// you can do this like this:
//
// U64 knights = p.wn
// while (knights) {
//      U64 oneKnight = knights & (-knights);
//      U64 moves = MG_knights(oneKnight);
//      remove that bit;
// }


// Could be more efficient
U64 MG_knight(U64 board) {
    U64 MG_knight = 0;
    U64 shifter = 0; 

    for(int i = 0; i < 64; i++) {
        if ( ((shifter = (1ULL << i)) | board) == board )  {
            // Generates moves for knight dependent on row and column 
            // (does not add invalid moves if knight is on edge of board) 

            MG_knight |= (shifter << 17) * !(((BIT_8_RANK | BIT_7_RANK) & shifter) || (BIT_H_FILE & shifter)); 
            MG_knight |= (shifter << 15) * !(((BIT_8_RANK | BIT_7_RANK) & shifter) || (BIT_A_FILE & (shifter))); 
            MG_knight |= (shifter << 6) * !((BIT_8_RANK & shifter) || ((BIT_A_FILE | BIT_B_FILE) & (shifter))); 
            MG_knight |= (shifter << 10) * !((BIT_8_RANK & shifter) || ((BIT_G_FILE | BIT_H_FILE) & (shifter)));
            MG_knight |= (shifter >> 17) * !(((BIT_1_RANK | BIT_2_RANK) & shifter) || (BIT_A_FILE & (shifter)));
            MG_knight |= (shifter >> 15) * !(((BIT_1_RANK | BIT_2_RANK) & shifter) || (BIT_H_FILE & (shifter))); 
            MG_knight |= (shifter >> 6) * !(((BIT_1_RANK & shifter) || ((BIT_G_FILE | BIT_H_FILE) & (shifter)))); 
            MG_knight |= (shifter >> 10) * !((BIT_1_RANK & shifter) || ((BIT_A_FILE | BIT_B_FILE) & (shifter))); 
        }
    }
    return MG_knight;
}

U64 MG_rook(U64 board) {
    U64 MG_rook = 0;
    U64 shifter = 0;
    for(int i = 0; i < 64; i++) {
        if( ((shifter = (1ULL << i)) | board) == board ) {
            MG_rook |= (BIT_A_FILE << (i % 8));
            MG_rook |= (BIT_1_RANK << (i / 8) * 8);
        }
    }

    MG_rook &= ~board;
    return MG_rook;
}

U64 MG_bishop(U64 board) {
    U64 MG_bishop = 0;

    int r, f;

    for (int i = 0; i < 64; i ++) {
        if( ((1ULL << i) | board) == board ) {
            int file = i % 8;
            int rank = i / 8;

            for (r = rank + 1, f = file + 1; f < 8 && r < 8; r++, f++) {
                MG_bishop |= (1ULL << (r * 8 + f));
            }

            for (r = rank + 1, f = file - 1; f >= 0 && r < 8; r++, f--) {
                MG_bishop |= (1ULL << (r * 8 + f));
            }

            for (r = rank - 1, f = file + 1; f < 8 && r >= 0; r--, f++) {
                MG_bishop |= (1ULL << (r * 8 + f));
            }

            for (r = rank - 1, f = file - 1; f >= 0 && r >= 0; r--, f--) {
                MG_bishop |= (1ULL << (r * 8 + f));
            }
        }
    }
    return MG_bishop;
}

U64 MG_king(U64 board) {
    

}

U64 MG_queen(U64 board) {
    return (MG_rook(board) | MG_bishop(board);
}

U64 MG_wpawn(U64 board, Move lastmove) {
    U64 black_occ = black_occ(&board);



    // capture
    // en-passan
    // direction
    // two move initial
}

U64 MG_bpawn(U64 board) {
    // capture
    // en-passan
    // direction
    // two move initial
}

U64 pawn_promotion(U64 board) {
    // I think this can be combined because white pawn 
    // cannot go to the rank 1 and black pawn cannot go to the 
    // rank 8 
    // but idk if you want to make it so that you can also play really weird Position
    // maybe you need to separate this.
}

// -------------------------------

int main() {
    State p = initialize_state();

    U64 testBit1 = 0x0000000800000000;
    U64 testBit2 = 0x1000000000000000;

    MG_rook(testBit1);
    MG_bishop(testBit1);

    U64 MGQueen = MG_queen(testBit1);
    print_bb(testBit1);
    print_bb(MGQueen);

    //printf("♔,♕,♖,♗,♘,♙,♚,♛,♜,♝,♞,♟︎ \n");
    return 0;
}

