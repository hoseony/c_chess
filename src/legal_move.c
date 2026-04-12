#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "move_generation.h"
#include "bitboards.h"

// -------- function prototypes ------
U64 generateMoveFromTargetSquare(State *p, State *prev, int targetSquare, U64 occupied);
void doMove(State *p, State *prev, int from, int to, bool perftQuestionMark);
int areYouMated(State *p, State *prev);
int generateLegalMove(State state, State prevState, Move *moves, int maxMoves);
PerftResult perft(State p, State prev, int depth);
void runPerft();
// -----------------------------------

// Note to Thomas: Make more efficient 
U64 generateMoveFromTargetSquare(State *p, State *prev, int targetSquare, U64 occupied) {
    
    U64 square = (1ULL << targetSquare);
    bool turn = p->turn;

    if((square & p->wp) && (turn == WHITE)) {
        return generateWhitePawnMove(targetSquare, occupied, *p, *prev);
    }
    else if ((square & p->bp) && (turn == BLACK)) {
        return generateBlackPawnMove(targetSquare, occupied, *p, *prev);
    }
    else if((square & p->wn) || (square & p->bn))  {
        return generateKnightMove(targetSquare);
    }
    else if((square & p->wb) || (square & p->bb)) {
        return generateBishopMove(targetSquare, occupied);
    }
    else if((square & p->wr) || (square & p->br)) {
        return generateRookMove(targetSquare, occupied);
    }
    else if((square & p->wq) || (square & p->bq)) {
        return generateQueenMove(targetSquare, occupied);
    }
    else if((square & p->wk) || (square & p->bk)) {
        return generateKingMove(targetSquare) | ((generateWhiteKingCastleMove(*p) * ((square & p->wk) > 0) + generateBlackKingCastleMove(*p) * ((square & p->bk) > 0)));
    } else {
        return 0;
    }
}

void doMove(State *p, State *prev, int from, int to, bool perftQuestionMark) {
    U64 enPassantBitboard = generateEnPassant(*p, *prev);

    U64 fromBoard = (1ULL << from);
    U64 toBoard = (1ULL << to);

    bool wasThatWhitePawn = ((p->wp & fromBoard) > 0);
    bool wasThatBlackPawn = ((p->bp & fromBoard) > 0);

    bool wasThatWhiteKing = ((p->wk & fromBoard) > 0);
    bool wasThatBlackKing = ((p->bk & fromBoard) > 0);
    
    U64 wasThatCapture = 0;

    StateUnion su; 
    su.s = *p;

    int threeFold = p->threeMoveRepetition;
    int fiftyMove = p->fiftyMoveRule;

    // =====================Draw conditions =====================
    // FiftyMoveRule
    for (int i = 0; i < 12; i++) {
        wasThatCapture |= (su.pieces[i] & toBoard);
    }
    if (wasThatCapture || wasThatWhitePawn || wasThatBlackPawn) {
        fiftyMove = 0;
        // printf("that was not fifty\n");
    } else {
        fiftyMove++;
    }
    
    if (perftQuestionMark) {
        StateUnion suPrevPrev;
        suPrevPrev.s = prevprevState;

        bool same = true;

        // threeMoveRepetition;
        for (int i = 0; i < 12; i++) {
            if (su.pieces[i] != suPrevPrev.pieces[i]) {
                // it is different position
                same = false;
            }

            if (same) {
                threeFold++;
            } else {
                threeFold = 0;
            }
        }
    }

    // =====================moving pieces ======================
    // enPassant Caputre and Move and Castling
    if ( (toBoard & enPassantBitboard) && ( (wasThatWhitePawn) || (wasThatBlackPawn) )) {
        if(p->turn == WHITE) {
            moveBit(&p->wp, from, to);
            removeBit(&p->bp, (to - 8));
        } else {
            moveBit(&p->bp, from, to);
            removeBit(&p->wp, (to + 8));
        }
    } else if ( (wasThatWhiteKing) && (abs(to - from) == 2) ) { // white castled
        moveBit(&(p->wk), from, to);

        if (to == 6) { // king side
            moveBit(&(p->wr), 7, 5); // hardcoded square
        } else { // queen side
            moveBit(&(p->wr), 0, 3);
        }
    } else if ( (wasThatBlackKing) && (abs(to - from) == 2) ) { // black castled
        moveBit(&(p->bk), from, to);

        if(to == 62) { // king side
            moveBit(&(p->br), 63, 61);
        } else {
            moveBit(&(p->br), 56, 59);
        }
    } else { // regular moves
        // find what piece moved
        size_t pieceMoved = 0; 
        for (size_t i = 0; i < 12; i++) {
            pieceMoved += i * ((su.pieces[i] & fromBoard) > 0);
            removeBit(&(su.pieces[i]), to); //captures the thing if capture
        }

        // move it
        moveBit(&(su.pieces[pieceMoved]), from, to);
        *p = su.s;
    }

    // ============== remove castling right =============
    // White
    if (wasThatWhiteKing) {
        p->castleState &= 0b0011;
    }
    if (fromBoard & (1ULL << 7)) {
        p->castleState &= 0b0111;
    }
    if (fromBoard & (1ULL << 0)) {
        p->castleState &= 0b1011;
    }
    
    // Black
    if (wasThatBlackKing) {
        p->castleState &=0b1100;
    }
    if (fromBoard & (1ULL << 63)) {
        p->castleState &= 0b1101;
    }
    if (fromBoard & (1ULL << 56)) {
        p->castleState &= 0b1110;
    }

    p->turn = !p->turn;
    // because of the way we wrote this code, we kind of need to do this 
    // I didn't want to fix it lol, very stupid
    p->fiftyMoveRule = fiftyMove;
    p->threeMoveRepetition = threeFold;
}

int areYouMated(State *p, State *prev) { 
    // This can be done by asking, "do you have any move that can get you out from the check?"
    // first, check if you are in check, if you are not, you can not be mated.

    State state = *p;

    if (isInCheck(state) == 0) {
        return 0;
    }

    U64 occupied = allOccupied(state);
    U64 friendlyBoard = (state.turn == WHITE) ? whiteOccupied(state) : blackOccupied(state);

    // the, for every piece you have, let's find possible moves, and move them here and there
    // If you moved it, and stil in check for 
    for (int i = 0; i < 64; i++) {
        // skip the squares that are empty (where there no piece to move)
        if ( ((1ULL << i) & friendlyBoard) == 0 ) {
            continue;
        }

        U64 candidateMoves = generateMoveFromTargetSquare(&state, prev, i, occupied);
        U64 possibleMoves = candidateMoves & ~friendlyBoard;

        while (possibleMoves > 0) {
            int to = popLSB(&possibleMoves);

            State temp = state;
            doMove(&temp, p, i, to, true);
            temp.turn = !temp.turn;
            // if you found a move that makes you get out from the check,
            // you are not mated
            if (!isInCheck(temp)) {
                return 0;
            }
        }
    }
    // you only every come here if you found no move that makes you get out from the check
    return 1;
}


int generateLegalMove(State state, State prevState, Move *moves, int maxMoves) {
    int counter = 0;

    U64 occupied = allOccupied(state);
    U64 friendlyBoard = (state.turn == WHITE) ? whiteOccupied(state) : blackOccupied(state);

    for (int i = 0; i < 64; i++) {
        if ( ((1ULL << i) & friendlyBoard) == 0 ) {
            continue;
        }
       
        U64 possibleMoves = generateMoveFromTargetSquare(&state, &prevState, i, occupied) & ~friendlyBoard;

        while (possibleMoves > 0) {
            int to = popLSB(&possibleMoves);

            State temp = state;
            doMove(&temp, &prevState, i, to, false);

            State checkCheck = temp;
            checkCheck.turn = !checkCheck.turn;
            if (isInCheck(checkCheck)) {
                continue;
            }

            if (counter < maxMoves) {
                moves[counter].from = i;
                moves[counter].to = to;
                counter++;
            }
        }
    }
    return counter; 
}


PerftResult perft(State p, State prev, int depth) {
    PerftResult result = {0};

    // base case
    if (depth == 0) {
        result.nodes = 1;
        return result;
    }

    U64 occupied = allOccupied(p);
    U64 friendlyBoard = (p.turn == WHITE) ? whiteOccupied(p) : blackOccupied(p);
    U64 enemyBoard = (p.turn == WHITE) ? blackOccupied(p) : whiteOccupied(p);

    for (int i = 0; i < 64; i++) {
        if ( ((1ULL << i) & friendlyBoard) == 0 ) {
            continue;
        }

        // generate pseudo-legal moves
        U64 possibleMoves = generateMoveFromTargetSquare(&p, &prev, i, occupied) & ~friendlyBoard;

        while (possibleMoves) {
            // get lsb index
            int to = popLSB(&possibleMoves);
            U64 epSquare = generateEnPassant(p, prev);

            // check the type of move first
            bool wasThatWhitePawn = ((p.wp & (1ULL << i)) != 0);
            bool wasThatBlackPawn = ((p.bp & (1ULL << i)) != 0);
            bool wasThatPawn = wasThatWhitePawn || wasThatBlackPawn;

            // don't worry about it. This is basically how doMove handles but simpliefed (heaviliy, in one line)
            // I really hate this but it is just a debug function so I'll just leave it like this. Whatever.
            bool isCapture = (depth == 1) && ((1ULL << to) & enemyBoard);
            bool isCastle = (depth == 1) && (((p.wk | p.bk) & (1ULL << i)) && abs(to - i) == 2);
            bool isEnPassant = (depth == 1) && wasThatPawn && ((1ULL << to) & epSquare) && !(enemyBoard & (1ULL << to)) && (abs(to - i) == 7 || abs(to - i) == 9);

            // let's play a move
            State temp = p;
            doMove(&temp, &prev, i, to, false);

            // swap the turn
            // you want to check if your move leave your king in check
            State toCheck = temp;
            // side who made move (since doMove flips the turn)
            toCheck.turn = !toCheck.turn;
            if (isInCheck(toCheck)) {
                continue;
            }

            // if that wa a valid move, increment the following thing
            if (depth == 1) {
                if (isEnPassant) { 
                    result.enPassant++;
                    result.captures++;
                } else if (isCapture) {
                    result.captures++;
                }
                if (isCastle) {
                    result.castle++;
                }
            }
            
            // counting up hehe
            PerftResult r = perft(temp, p, depth - 1);
            result.nodes += r.nodes;
            result.captures += r.captures;
            result.enPassant+= r.enPassant;
            result.castle += r.castle;
            result.promotion += r.promotion;
        }
    }
    return result;
}

void runPerft() {
    State pos = initializeState();
    State prev = initializeState();

    int depth = 8;
    for (int i = 0; i < depth; i++) {
        // https://stackoverflow.com/questions/5248915/execution-time-of-c-program
        clock_t start = clock();
        PerftResult p = perft(pos, prev, i);
        clock_t end = clock();
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000;

        printf("depth %d overview (time: %f ms) | nodes: %llu \t captures %llu \tenPassant: %llu \tcastle: %llu \tpromotion: %llu\n", i, elapsed,p.nodes, p.captures, p.enPassant, p.castle, p.promotion);
    }
}

