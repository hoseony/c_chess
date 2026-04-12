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


int positionEvaluation(State p) {
    int score = 0;

    // piece value;
    score += __builtin_popcountll(p.wp) * 1;
    score += __builtin_popcountll(p.wn) * 3;
    score += __builtin_popcountll(p.wb) * 3;
    score += __builtin_popcountll(p.wr) * 5;
    score += __builtin_popcountll(p.wq) * 9;

    score -= __builtin_popcountll(p.bp) * 1;
    score -= __builtin_popcountll(p.bn) * 3;
    score -= __builtin_popcountll(p.bb) * 3;
    score -= __builtin_popcountll(p.br) * 5;
    score -= __builtin_popcountll(p.bq) * 9;

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
    }

    // get possible moves
    Move moves[218];
    int moveCount = generateLegalMove(p, prev, moves, 218);

    // checkmate or stalemate
    if (moveCount == 0) {
        if (isInCheck(p)) {
            // being mated is bad
            return -999999;
        } else {
            // draw is mid
            return 0;
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
            return beta;
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

    if (moveCount < 10) {
        depth = 7;
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

