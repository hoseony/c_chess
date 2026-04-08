#include "bitboards.h"
#include "types.h"
#include "move_generation.h"
#include "fen.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

bool parseLAN(char *input, int *fromSquare, int *toSquare) {
    int fromFile = input[0] - 'a';
    int fromRank = input[1] - '1';
    int toFile = input[2] - 'a';
    int toRank = input[3] - '1';

    printf("%d,%d -> %d,%d\n", fromFile, fromRank, toFile, toRank);

    *fromSquare = (fromRank * 8 + fromFile);
    *toSquare = (toRank * 8 + toFile);

    return 1;
}

// Note to Thomas: Make more efficient 
U64 generateMoveFromTargetSquare(State *p, int targetSquare, U64 occupied) {
    
    U64 square = (1ULL << targetSquare);
    bool turn = p->turn;

    if((square & p->wp) && (turn == WHITE)) {
        return generateWhitePawnMove(targetSquare, occupied);
    }
    else if ((square & p->bp) && (turn == BLACK)) {
        return generateBlackPawnMove(targetSquare, occupied);
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

void doMove(State *p, int from, int to) {
    U64 enPassantBitboard = generateEnPassant();

    U64 fromBoard = (1ULL << from);
    U64 toBoard = (1ULL << to);

    bool wasThatWhitePawn = ((p->wp & fromBoard) > 0);
    bool wasThatBlackPawn = ((p->bp & fromBoard) > 0);

    bool wasThatWhiteKing = ((p->wk & fromBoard) > 0);
    bool wasThatBlackKing = ((p->bk & fromBoard) > 0);
    
    U64 wasThatCapture = 0;

    StateUnion su; 
    StateUnion suPrevPrev;
    su.s = *p;
    suPrevPrev.s = prevprevState;

    int threeFold = p->threeMoveRepetition;
    int fiftyMove = p->fiftyMoveRule;

    // =====================Draw conditions =====================
    // FiftyMoveRule
    for (int i = 0; i < 12; i++) {
        wasThatCapture |= (su.pieces[i] & toBoard);
    }
    if (wasThatCapture || wasThatWhitePawn || wasThatBlackPawn) {
        fiftyMove = 0;
        printf("that was not fifty\n");
    } else {
        fiftyMove++;
    }

    // threeMoveRepetition;
    for (int i = 0; i < 12; i++) {
        if (su.pieces[i] != suPrevPrev.pieces[i]) {
            threeFold = 0;
        } else {
            threeFold++;
            break;
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
    // I didn't want to fix it lol
    p->fiftyMoveRule = fiftyMove;
    p->threeMoveRepetition = threeFold;
}



int main() {
    currentState = prevState = prevprevState = initializeState();

    // example position (there should be a better way than loading 3 position manually...)
/*
    char startingPosition[] = "r1bqk2r/pppp1ppp/2n2n2/1B2p3/1b2P3/2N2N2/PPPP1PPP/R1BQK2R w KQkq - 6 5";
    currentState = fenToState(startingPosition);

    char prevPosition[] = "r1bqkb1r/pppp1ppp/2n2n2/1B2p3/4P3/2N2N2/PPPP1PPP/R1BQK2R b KQkq - 5 4";
    prevState = fenToState(prevPosition);

    char prevprevPosition[] = "r1bqkb1r/pppp1ppp/2n2n2/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 4 4";
    prevprevState = fenToState(prevprevPosition);
*/
    char input[5];
    // Lan only need 4 + '\0'

    printGameBoard(currentState);
    int from, to;

    while(1) {
        // check the drawing condition
        if (currentState.fiftyMoveRule >= 100) {
            break;
        }
        if (currentState.threeMoveRepetition  >= 6) {
            break;
        }

        // get the user input
        scanf("%4s", input);

        // update occupied boards
        U64 occupied = allOccupied(currentState);
        U64 blackBoard = blackOccupied(currentState);
        U64 whiteBoard = whiteOccupied(currentState);

        // parse user input
        parseLAN(input, &from, &to);

        // This gives you actual moves that you can make (I hope)
        // You AND with NOT of your pieces to discard friendly pieces
        U64 candidateMoves = generateMoveFromTargetSquare(&currentState, from, occupied);

        // This is the pseudo-legal move (legal move without checking check)
        U64 possibleMoves = (currentState.turn == WHITE) ? (candidateMoves & ~whiteBoard) : (candidateMoves & ~blackBoard);
        
        // Here we check if the king is in check after the move (meaning illegal move)
        if( (1ULL << to) & possibleMoves) {
            State temp = currentState;       // Save the current State
            doMove(&currentState, from, to); // Try the move
            
            // These are to check check
            State afterMove = currentState;
            afterMove.turn = !afterMove.turn;

            if (isInCheck(afterMove)) { // if in check, Undo the move 
                currentState = temp;
                printf("Illegal Move; King Check\n");
            } else { // else, Domove
                prevprevState = prevState;
                prevState = temp;
            }
        } else {
            printf("invalid move\n");
        }
        printGameBoard(currentState);
    }
    return 0;
}
