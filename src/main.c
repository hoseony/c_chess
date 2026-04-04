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

    // enPassant Caputre and Move
    if ( (toBoard & enPassantBitboard) && ( (wasThatWhitePawn) || (wasThatBlackPawn) )) {
        if(p->turn == WHITE) {
            moveBit(&p->wp, from, to);
            removeBit(&p->bp, (to - 8));
        } else {
            moveBit(&p->bp, from, to);
            removeBit(&p->wp, (to + 8));
        }
    } else if ( (wasThatWhiteKing) && (abs(to - from) == 2) ) { // wow, white castled
        moveBit(&(p->wk), from, to);

        if (to == 6) { // king side
            moveBit(&(p->wr), 7, 5); // hardcoded square
        } else { // queen side
            moveBit(&(p->wr), 0, 3);
        }
    } else if ( (wasThatBlackKing) && (abs(to - from) == 2) ) { // wow, black castled
        moveBit(&(p->bk), from, to);

        if(to == 62) { // king side
            moveBit(&(p->br), 63, 61);
        } else {
            moveBit(&(p->br), 56, 59);
        }
    } else { // regular moves
        // placing the move to the board
        stateUnion su; 
        su.s = *p;

        // find what piece moved
        size_t pieceMoved = 0; 
        for (size_t i = 0; i < 12; i++) {
            pieceMoved += i * ((su.pieces[i] & fromBoard) > 0);
            removeBit(&(su.pieces[i]), to);
        }

        // move it
        moveBit(&(su.pieces[pieceMoved]), from, to);
        *p = su.s;
    }

    // remove castling right
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
}



int main() {
    currentState = prevState = prevprevState = initializeState();


    char startingPosition[] = "r1bqk2r/pppp1ppp/2n2n2/1B2p3/1b2P3/2N2N2/PPPP1PPP/R1BQK2R w KQkq - 6 5";
    currentState = fenToState(startingPosition);

    char prevPosition[] = "r1bqkb1r/pppp1ppp/2n2n2/1B2p3/4P3/2N2N2/PPPP1PPP/R1BQK2R b KQkq - 5 4";
    prevState = fenToState(prevPosition);

    char prevprevPosition[] = "r1bqkb1r/pppp1ppp/2n2n2/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 4 4";
    prevprevState = fenToState(prevprevPosition);

    // char input[4] = {'e', '2', 'e', '3'};
    char input[5];

    printGameBoard(currentState);
    int from, to;

    while(1) {
        scanf("%4s", input);
        U64 occupied = allOccupied(currentState);
        U64 blackBoard = blackOccupied(currentState);
        U64 whiteBoard = whiteOccupied(currentState);

        parseLAN(input, &from, &to);

        U64 candidateMoves = generateMoveFromTargetSquare(&currentState, from, occupied);
        // This gives you actual moves that you can make (I hope)
        // You AND with NOT of your pieces to discard friendly pieces
        
        U64 possibleMoves = (currentState.turn == WHITE)? (candidateMoves & ~whiteBoard) : (candidateMoves & ~blackBoard);
        //printBitboard(possibleMoves);
        
        if( (1ULL << to) & possibleMoves) {
            State temp = currentState;
            doMove(&currentState, from, to);
            prevprevState = prevState;
            prevState = temp;

        } else {
            printf("invalid move\n");
        }

        printGameBoard(currentState);
        // printBitboard(currentState.wp);
    }
    // if your targetSquare is included in the possibleMoves (if the move is somewhat valid)
    // You should now check if that move result in check
    // This will be done with do, undo move
    // Then see if king is in the attackBoard after the move
    // I already made whiteAttackBoard and blackAttackBoard
    // If so, discard that move (set that bit to 0)
    // and you have final legalMoves
    // Check check before and after move :)
    return 0;
}
