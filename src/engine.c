#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "bitboards.h"
#include "move_generation.h"
#include "legal_move.h"
#include "finding_magic.h"

// ---------------------------------------------------------------
int pieceSquareTable(U64 board, int pieceSquareTable[64], int turn);
int positionEvaluation(State p);
int negamax(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic);
Move negmaxBestMove(State *p, State prev, int depth, RookMagic *rookMagic, BishopMagic *bishopMagic);
int qsearch(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic);
int MVVLVA(Move m, State *p);

void quicksortMoves(Move *moves, int low, int high, State *state);
// ---------------------------------------------------------------

// Piece Square Table
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

static int kingEnd[64] = {
    -10,   10,  -10, -10, -10,  -10, -10,  -10,
    -10,   00,   00,  00,  00,   00,  00,  -10,
    -10,   00,   10,  15,  15,   10,  00,  -10,
    -10,   00,   15,  20,  20,   15,  00,  -10,
    -10,   00,   15,  20,  20,   15,  00,  -10,
    -10,   00,   25,  15,  15,   25,  00,  -10,
    -10,   00,   00,  00,  00,   00,  00,  -10,
    -10,  -30,  -20,  00,  00,  -20, -30,  -10,
};

static int pawn[64] = {
    900, 900, 900,  900,  900, 900, 900, 900, // promotion goated
    00,  00,  00,   00,   00,  00,  00,  00,
    00,  00,  00,   10,   10,  00,  00,  00,
    00,  00,  10,   20,   20,  10,  00,  00,
    00,  00,  10,   20,   20,  10,  00,  00,
    05,  00,  00,   00,   00,  00,  00,  05,
    00,  00,  00,  -15,  -15,  00,  00,  00,
    00,  00,  00,   00,   00,  00,  00,  00,
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
    -10,  10,  15,  10,  10,  15,  10, -10,
    -10,  10,  15,  15,  15,  15,  10, -10,
    -10,  10,  15,  05,  05,  15,  10, -10,
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

// ---------------------------------------------------------------

int pieceSquareTable(U64 board, int pieceSquareTable[64], int turn) {
    int score = 0;
    while(board) {
        int square = popLSB(&board);
        int index = (turn == 1) ? (square) : (square ^ 56); 
        // 63 in binary is 111111
        // 63 should become 7 which is 000111
        // 53 in binary is 111000
        // and xor will do the job. 
        // This holds true for any pattern.
        // more can be found here: https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
        
        score += pieceSquareTable[index];
    }
    return score;
}


int positionEvaluation(State p) {
    int score = 0;

    // piece value
    // https://gcc.gnu.org/onlinedocs/gcc/Bit-Operation-Builtins.html
    // more funny things

    score += __builtin_popcountll(p.wp) * 100 + pieceSquareTable(p.wp, pawn, SIDE_WHITE);
    score += __builtin_popcountll(p.wn) * 300 + pieceSquareTable(p.wn, knight, SIDE_WHITE);
    score += __builtin_popcountll(p.wb) * 300 + pieceSquareTable(p.wb, bishop, SIDE_WHITE);
    score += __builtin_popcountll(p.wr) * 500 + pieceSquareTable(p.wr, rook, SIDE_WHITE);
    score += __builtin_popcountll(p.wq) * 900 + pieceSquareTable(p.wq, queen, SIDE_WHITE);
    score += pieceSquareTable(p.wk, king, SIDE_WHITE);

    score -= __builtin_popcountll(p.bp) * 100 + pieceSquareTable(p.bp, pawn, SIDE_BLACK);
    score -= __builtin_popcountll(p.bn) * 300 + pieceSquareTable(p.bn, knight, SIDE_BLACK);
    score -= __builtin_popcountll(p.bb) * 300 + pieceSquareTable(p.bb, bishop, SIDE_BLACK);
    score -= __builtin_popcountll(p.br) * 500 + pieceSquareTable(p.br, rook, SIDE_BLACK);
    score -= __builtin_popcountll(p.bq) * 900 + pieceSquareTable(p.bq, queen, SIDE_BLACK);
    score -= pieceSquareTable(p.bk, king, SIDE_BLACK);

    return (p.turn == SIDE_WHITE) ? (score) : (-score);
}


/* I did not came up with this algorithm..
 * 
 * Pseudo code:
 *
 * int negaMax( int depth ) {
 *     if ( depth == 0 ) return evaluate();
 *     int max = -oo;
 *     for ( all moves)  {
 *         score = -negaMax( depth - 1 );
 *         if( score > max )
 *             max = score;
 *     }
 *     return max;
 * }
 */

// https://en.wikipedia.org/wiki/Negamax
int negamax(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic) {
    // alpha and beta are lower and upper bound for a child node.
    // The idea is you start from alpha -infinity, beta +infinity, and 
    // if your score gets out of this bound, you cut off the search

    // This is for setting timeout for the search (since of course)
    // https://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
    // I do this to check the timeout condition less... 
    // Hope this helps a little bit in terms of performance
    if (searchAborted) {
        return 0;
    }

    if ((++nodeCount % 1024) == 0) {
        if (((clock() - searchStartTime) * 1000 / CLOCKS_PER_SEC) > searchTimeLimit) {
            // printf("search timeout\n");
            searchAborted = true;
            return 0;
        }
    }
    
    // base case
    if (depth == 0) {
        // return positionEvaluation(*p);
        return qsearch(p, prev, 6, alpha, beta, rookMagic, bishopMagic);
        // https://www.dogeystamp.com/chess5/
    }

    // get possible moves
    Move moves[218];
    int moveCount = generateLegalMove(*p, prev, moves, 218, rookMagic, bishopMagic);

    // move ordering
    quicksortMoves(moves, 0, moveCount - 1, p);

    int value = -999999; // this will keep track of the best score

    // ideally, move ordering should go here.

    // checkmate or stalemate
    if (moveCount == 0) {
        if (isInCheck(*p, rookMagic, bishopMagic)) {
            // being mated is bad
            return -999999;
        } else {
            // draw is mid
            return -100;
            // I don't really want it to go for the draw, so I gave some -
        }
    }

    for(int i = 0; i < moveCount; i++) {
        State temp = *p;
        // you do the move

        doMove(&temp, &prev, moves[i].from, moves[i].to, false);

        int score = -negamax(&temp, *p, depth - 1, -beta, -alpha, rookMagic, bishopMagic);
       
        // This pruning way is called fail-soft variation of alpha-beta prunning
        
        // update the best score to value
        if (score > value) {
            value = score;
        }

        // when you found a move that's better, you don't need to search moves that
        // are worse than that
        if (value > alpha) {
            alpha = value;
        }

        // If your alpha is greater than beta, meaning your lower bound is higher than opponent's upperbound,
        // Your opponent will be smart enough to not let that happen.
        // prune the branch
        if (alpha >= beta) {
            break;
        }
    }
    return value;
}

Move negmaxBestMove(State *p, State prev, int depth, RookMagic *rookMagic, BishopMagic *bishopMagic) {
    Move moves[218]; // 218 is the max possible moves,  
                     // this stores every legal move
                     
    int moveCount = generateLegalMove(*p, prev, moves, 218, rookMagic, bishopMagic);
    int numPieces = __builtin_popcountll(allOccupied(*p));

    quicksortMoves(moves, 0, moveCount - 1, p);

    // hardcoded search depth
    if (numPieces <= 10) {
        depth = 8;
    } else if (numPieces < 15){
        depth = 7;
    }

    Move best = moves[0];

    for (int searchDepth = 3; searchDepth <= depth; searchDepth += 2) {
        nodeCount = 0;
        searchAborted = false;

        Move bestMoveCurrentDepth = moves[0];
        int alpha = -999999;
        int beta = 999999;

        for(int i = 0; i < moveCount; i++) {
            if (searchAborted) {
                break;
            }
            State temp = *p;
            doMove(&temp, &prev, moves[i].from, moves[i].to, false);

            int score = -negamax(&temp, *p, searchDepth - 1, -beta, -alpha, rookMagic, bishopMagic);
            //printf("negamax: root move %d -> %d score %d\n", moves[i].from, moves[i].to, score);

            // you found a better move. Nice!
            // (basically, it is trying every square (and its branch), to see if its a good move)
            if (!searchAborted && (score > alpha) ) {
                alpha = score;
                bestMoveCurrentDepth = moves[i];
            }
        }

        if (!searchAborted) {
            best = bestMoveCurrentDepth;
        } else {
            break;
        }
    }

    return best;
}

// https://www.chessprogramming.org/Quiescence_Search
int qsearch(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic) {
    int staticEval = positionEvaluation(*p);
   /* 
    if (depth == 0) {
        return positionEvaluation(*p);
    }
    */

    int best_value = staticEval;
    
    if (best_value >= beta) {
        return best_value;
    }

    if (best_value > alpha) {
        alpha = best_value;
    }

    Move moves[218];
    int moveCount = generateLegalMove(*p, prev, moves, 218, rookMagic, bishopMagic);
    U64 enemyBoard = (p->turn == SIDE_WHITE) ? (blackOccupied(*p)) : (whiteOccupied(*p));

    for (int i = 0; i < moveCount; i++) {
        U64 toBoard = (1ULL << moves[i].to);

        // you want to keep searching the captures (moves that are not quite)
        if ( (toBoard & enemyBoard) == 0 ) {
            continue;
        }

        State temp = *p;
        doMove(&temp, &prev, moves[i].from, moves[i].to, false);

        int score = -qsearch(&temp, *p, depth - 1, -beta, -alpha, rookMagic, bishopMagic);
        //printf("qsearch: root move %d -> %d score %d\n", moves[i].from, moves[i].to, score);

        if (score >= beta) {
            return score;
        } 

        if (score > best_value) {
            best_value = score;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

int pieceAt(State *p, int square) {
    StateUnion su;
    su.s = *p;
    U64 mask = 1ULL << square;
    
    for (int i = 0; i < 12; i++) {
        if (su.pieces[i] & mask) {
            return i % 6;
        }
    }

    return 0;
}


// -------------------- MOVE ORDERING ---------------------
// Most Valuable Victim - Least Valuable Agressor
// The idea here is that befoer searching through the node, you search through
// moves that are most likely to be best.
// 
int MVVLVA(Move m, State *p) {
    static int victimValue[] = {1, 3, 3, 5, 9, 0};
    static int attackerValue[] = {1, 3, 3, 5, 9 ,0};

    int victim = pieceAt(p, m.to);
    int attacker = pieceAt(p, m.from);

    return victimValue[victim] - attackerValue[attacker] + 8;
}


void swapMoves(Move *moves, int i, int j) {
    Move temp = moves[i];
    moves[i] = moves[j];
    moves[j] = temp;
}

int partition(Move *moves, int low, int high, State *state) {
    Move pivot = moves[high];

    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (MVVLVA(moves[j], state) >= MVVLVA(pivot, state)) {
            i++;
            swapMoves(moves, i, j);
        }
    }
    swapMoves(moves, i + 1, high);
    return i + 1;
}

void quicksortMoves(Move *moves, int low, int high, State *state) {
    if (low >= high || low < 0) {
        return;
    }
    int p = partition(moves, low, high, state);

    quicksortMoves(moves, low, p - 1, state);
    quicksortMoves(moves, p+1, high, state);
}


// -------------------- TRANSPOSITION TABLE ---------------------
// https://en.wikipedia.org/wiki/Zobrist_hashing
void zobristHash(State *state) {
    StateUnion su;
    su.s = *state;
    U64 hash = 0;

    for (int i = 0; i < 12; i++) {
        U64 pieceBoard = su.pieces[i];
        while(pieceBoard) {
            int index = popLSB(&pieceBoard);

        }
    }
};
