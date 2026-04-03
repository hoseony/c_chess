#ifndef TYPES_H
#define TYPES_H

#define WHITE 0
#define BLACK 1

#include <stdbool.h>

typedef unsigned long long int U64;
typedef unsigned int U32; 

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
    bool turn;
    U32 fiftyMoveRule;
    U32 threeMoveRepetition; 
    U32 castleState; 
} State;

typedef struct{
    State before;
    State after;
    int type;
} Move;

typedef union {
    State s; 
    U64 pieces[12]; 
} stateUnion; 

State currentState;
State prevState;
State prevprevState; 

#endif
