#include <stdio.h>

typedef unsigned long long int U64;

typedef enum {
    a1, b1, c1, d1, e1, f1, g1, h1, 
    a2, b2, c2, d2, e2, f2, g2, h2, 
    a3, b3, c3, d3, e3, f3, g3, h3, 
    a4, b4, c4, d4, e4, f4, g4, h4, 
    a5, b5, c5, d5, e5, f5, g5, h5, 
    a6, b6, c6, d6, e6, f6, g6, h6, 
    a7, b7, c7, d7, e7, f7, g7, h7, 
    a8, b8, c8, d8, e8, f8, g8, h8
} Board_pos;

typedef struct{
    U64 wp, wn, wb, wr, wq, wk;
    U64 bp, bn, bb, br, bq, bk;
} Position;

typedef struct{
    Board_pos from;
    Board_pos to;
} Move;

//-------- function prototype ----------
void put_bit(U64 *board, Board_pos pos);
void remove_bit(U64 *board, Board_pos pos);
void move_bit(U64 *board, Board_pos start_pos, Board_pos target_pos);
// so you are inputing board as &board type
void print_bb(U64 board);


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
Position initialize_position() {
    Position p;
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
    return p;
}
// ---------Combined Board ------------
U64 black_occ(Position *p) {
    return (p->bp | p->bn | p->bb | p->br | p->bq | p->bk);
}

U64 white_occ(Position *p) {
    return (p->wp | p->wn | p->wb | p->wr | p->wq | p->wk);
}

U64 all_occ(Position *p) {
    return (p->bp | p->bn | p->bb | p->br | p->bq | p->bk | p->wp | p->wn | p->wb | p->wr | p->wq | p->wk);
}

// --------- move check ---------
int p_move_check(Position *p, Move *m) {
    U64 occ = all_occ(p);
    U64 b_occ = black_occ(p);
    U64 w_occ = white_occ(p);

    U64 target_mask = 1ULL << m->to; // Target to bitboard
    U64 diff = (m->to - m->from); // This gives you where the pawn will move

    if ( ((1ULL << m->from) & p->wp) == 0 ) { // check if the pawn exist
        printf("pawn no there\n");
        return -1;
    } else if (diff == 8) { // forward 1
        if ( ((m->from % 8) == (m->to % 8)) && ((target_mask & occ) == 0) ) { 
            //check the same file, check one front
            printf("valid one forward move\n");
            return 1;
        };
    } else if (diff == 16) { // forward 2
        U64 middle_mask = (1ULL << (m->from + 8)); //one in front;

        if (m->from / 8 == 1) {
            if( ((m->from % 8) == (m->to %8)) && ((middle_mask & occ) == 0) && ((target_mask & occ) == 0) ) {
                printf("valid two forward move\n");
                return 1;
            } else {
                printf("invalid two forward move\n");
                return -1;
            }
        } else {
            printf("invalid two forward move\n");
            return -1;
        }
    } else if (diff == 7) { // capture left
        if ( (m->from % 8 != 0) && ((target_mask & b_occ) != 0) ) { 
            // checks if it is not a a-file & checks if there is smt to capture
            printf("can capture\n");
            return 1;
        }
    } else if (diff == 9) { //capture right
         if ( (m->from % 8 != 7) && ((target_mask & b_occ) != 0) ) {
            // checks if it is not a h-file
            // checks if there is smt to capture
            printf("can capture\n");
            return 1;
        }
    } else {
        return -1;
    }
    return 1;
}

// -------------------------------
int main() {
    Position p = initialize_position();

    //print_bb(black_occ(&p));
    //print_bb(white_occ(&p));
    print_bb(all_occ(&p));

    // checking capture check
    Move m1;
    m1.from = e2;
    m1.to = d3;
    put_bit(&(p.bp), d3);
    print_bb(all_occ(&p));
    p_move_check(&p, &m1);
    
    // checking one forward
    Move m2;
    m2.from = a2;
    m2.to = a3;
    p_move_check(&p, &m2);

    // checking two forward
    m2.from = a2;
    m2.to = a4;
    p_move_check(&p, &m2);
    
    // checking two forward blocked middle
    m1.to = d4;
    m1.from = d2;
    p_move_check(&p, &m1);

    put_bit(&(p.bp), h4);
    m1.to = h4;
    m1.from = h2;
    p_move_check(&p, &m1);
    //printf("♔,♕,♖,♗,♘,♙,♚,♛,♜,♝,♞,♟︎ \n");

    return 0;
}
