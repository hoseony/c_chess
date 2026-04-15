#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "bitboards.h"
#include "move_generation.h"
#include "parse.h"
#include "legal_move.h"

State currentState;
State prevState;
State prevprevState; 

static int king[64] = {
    -80, -80, -80, -80, -80, -80, -80, -80,
    -80, -80, -80, -80, -80, -80, -80, -80,
    -80, -80, -80, -80, -80, -80, -80, -80,
    -80, -80, -80, -80, -80, -80, -80, -80,
    -60, -60, -60, -60, -60, -60, -60, -60,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -20, -20, -20, -20, -20, -20, -20, -20,
     20,  30,  10, -30, -10, -20,  30,  20,
};

static int pawn[64] = {
    00, 00, 00,  00,  00, 00, 00, 00,
    00, 00, 00,  00,  00, 00, 00, 00,
    00, 00, 00,  10,  10, 00, 00, 00,
    00, 00, 10,  20,  20, 10, 00, 00,
    00, 00, 10,  20,  20, 10, 00, 00,
    05, 00, 00,  10,  10, 00, 00, 05,
    00, 00, 00, -15, -15, 00, 00, 00,
    00, 00, 00,  00,  00, 00, 00, 00,
};

static int knight[64] = {
    -10,   10,  -10, -10, -10,  -10, -10,  -10,
    -10,   00,   00,  00,  00,   00,  00,  -10,
    -10,   00,   10,  15,  15,   10,  00,  -10,
    -10,   00,   15,  20,  20,   15,  00,  -10,
    -10,   00,   15,  20,  20,   15,  00,  -10,
    -10,   00,   25,  15,  15,   25,  00,  -10,
    -10,   00,   00,  00,  00,   00,  00,  -10,
    -10,  -30,  -20,  00,  00,  -20, -30,  -10,
};

static int bishop[64] = {
    -10, -10, -10, -10, -10, -10, -10, -10,
    -10,  00,  10,  10,  10,  10,  00, -10,
    -10,  10,  15,  15,  15,  15,  10, -10,
    -10,  10,  15,  20,  20,  15,  10, -10,
    -10,  10,  15,  25,  25,  15,  10, -10,
    -10,  10,  15,  15,  15,  15,  10, -10,
    -10,  00,  10,  10,  10,  10,  00, -10,
    -10, -10, -30, -10, -10, -30, -10, -10,
};

static int rook[64] = {
     00, 00, 00, 00, 00, 00, 00,  00,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
     00, 00, 00, 20, 20, 00, 00,  00,
};

static int queen[64] = {
     00, 00, 00, 00, 00, 00, 00,  00,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
     00, 00, 00, 10, 10, 00, 00,  00,
};

int pieceSquareTable(U64 board, int pieceSquareTable[64], int turn) {
    int score = 0;
    while(board) {
        int square = popLSB(&board);
        int index = (turn == 1) ? (square) : (63 - square);
        score += pieceSquareTable[index];
    }
    return score;
}


int positionEvaluation(State p) {
    int score = 0;

    // piece value
    // https://gcc.gnu.org/onlinedocs/gcc/Bit-Operation-Builtins.html
    // more funny things

    score += __builtin_popcountll(p.wp) * 100 + pieceSquareTable(p.wp, pawn, WHITE);
    score += __builtin_popcountll(p.wn) * 300 + pieceSquareTable(p.wn, knight, WHITE);
    score += __builtin_popcountll(p.wb) * 300 + pieceSquareTable(p.wb, bishop, WHITE);
    score += __builtin_popcountll(p.wr) * 500 + pieceSquareTable(p.wr, rook, WHITE);
    score += __builtin_popcountll(p.wq) * 900 + pieceSquareTable(p.wq, queen, WHITE);
    score += pieceSquareTable(p.wk, king, WHITE);

    score -= __builtin_popcountll(p.bp) * 100 + pieceSquareTable(p.bp, pawn, BLACK);
    score -= __builtin_popcountll(p.bn) * 300 + pieceSquareTable(p.bn, knight, BLACK);
    score -= __builtin_popcountll(p.bb) * 300 + pieceSquareTable(p.bb, bishop, BLACK);
    score -= __builtin_popcountll(p.br) * 500 + pieceSquareTable(p.br, rook, BLACK);
    score -= __builtin_popcountll(p.bq) * 900 + pieceSquareTable(p.bq, queen, BLACK);
    score -= pieceSquareTable(p.bk, king, BLACK);

    return (p.turn == WHITE) ? (score) : (-score);
}


/*
int negaMax( int depth ) {
    if ( depth == 0 ) return evaluate();
    int max = -oo;
    for ( all moves)  {
        score = -negaMax( depth - 1 );
        if( score > max )
            max = score;
    }
    return max;
}
*/

int negamax(State p, State prev, int depth, int alpha, int beta) {
    // beta: opponent's upper bound
    // alpha: your lower bound

    // base case
    if (depth == 0) {
        return positionEvaluation(p);
        // this should be qsaerch
    }

    // get possible moves
    Move moves[218];
    int moveCount = generateLegalMove(p, prev, moves, 218);

    // checkmate or stalemate
    if (moveCount == 0) {
        if (isInCheck(p)) {
            // being mated is bad
            return -9999999;
        } else {
            // draw is mid
            return -150;
        }
    }

    for(int i = 0; i < moveCount; i++) {
        State temp = p;
        // you do the move
        doMove(&temp, &prev, moves[i].from, moves[i].to, false);

        // then, you pass the next turns
        // This score is basically telling you how good this position is
        int score = -negamax(temp, p, depth - 1, -beta, -alpha);
       
        // some pruning
        
        if (score >= beta) {
            return score;
        }

        // you found a better move. Nice!
        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}

Move negmaxBestMove(State p, State prev, int depth) {
    Move moves[218];
    int moveCount = generateLegalMove(p, prev, moves, 218);

    int numPieces = __builtin_popcountll(allOccupied(p));

    if (numPieces <= 15) {
        depth = 7;
    } else if (numPieces < 10){
        depth = 8;
    }

    Move best = moves[0];
    int alpha = -999999;
    int beta = 999999;

    for(int i = 0; i < moveCount; i++) {
        State temp = p;
        doMove(&temp, &prev, moves[i].from, moves[i].to, false);

        int score = -negamax(temp, p, depth - 1, -beta, -alpha);

        // you found a better move. Nice!
        if (score > alpha) {
            alpha = score;
            best = moves[i];
        }
    }
    return best;
}



int main() {
    currentState = prevState = prevprevState = initializeState();
   
    // list of possible moves
    Move moves[218];

    while(1) {

        int moveCount = generateLegalMove(currentState, prevState, moves, 218);
        
        if (moveCount == 0) { //if there's no move, it's either mate or stalemate, for now, I do not care
            if (isInCheck(currentState)) {
                printf("Checkmate!\n");
            } else {
                printf("Stalemate\n");
            }
            break;
        }

        if (currentState.fiftyMoveRule == 100) {
            printf("Draw by fiftyMoveRule\n");
            break;
        }

        Move m = negmaxBestMove(currentState, prevState, 5);

        State oldState = currentState;
        printf("playing: %d -> %d\n", m.from, m.to);
        doMove(&currentState, &prevState, m.from, m.to, true);

        prevprevState = prevState;
        prevState = oldState;

        printGameBoard(currentState);

    }

    return 0;
}

