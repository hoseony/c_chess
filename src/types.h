#ifndef TYPES_H
#define TYPES_H

#define SIDE_WHITE 0
#define SIDE_BLACK 1

#include <stdbool.h>
#include <time.h>

clock_t searchStartTime;
int searchTimeLimit;
static int nodeCount;
static bool searchAborted;

typedef unsigned long long int U64;
typedef unsigned int U32; 
typedef unsigned char U8;

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
    U8 castleState; 
} State;

typedef struct{
    int from;
    int to;
} Move;

typedef union {
    State s; 
    U64 pieces[12]; 
} StateUnion; 

typedef struct{
    U64 nodes;
    U64 captures;
    U64 enPassant;
    U64 castle;
    U64 promotion;
} PerftResult;

typedef struct {
    U64 mask;
    U64 magic;
    U64 shift;
} MagicEntry;

typedef struct {
    MagicEntry entry[64];
    U64 attacks[64][512];
} BishopMagic;

typedef struct {
    MagicEntry entry[64];
    U64 attacks[64][4096];
} RookMagic;

extern State currentState;
extern State prevState;
extern State prevprevState; 

typedef struct {
    State currentState;
    State prevState;
    Move bestMove;
    bool searching;
    bool doneSearching;
    RookMagic *rookMagic;
    BishopMagic *bishopMagic;
} EngineThreadData;

#endif
