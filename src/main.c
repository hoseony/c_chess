#include "bitboards.h"
#include "types.h"
#include "move_generation.h"
#include "fen.h"

#include <stdio.h>
#include <string.h>

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
        return generateKingMove(targetSquare);
    } else {
        return 0;
    }
}

void doMove(State *p, int from, int to) {
    U64 enPassantBitboard = generateEnPassant();

    U64 fromBoard = (1ULL << from);
    U64 toBoard = (1ULL << to);

    // enPassant Caputre
    if(toBoard & enPassantBitboard) {
        if(p->turn == WHITE) {
            moveBit(&p->wp, from, to);
            removeBit(&p->bp, (to - 8));
        } else {
            moveBit(&p->bp, from, to);
            removeBit(&p->bp, (to + 8));
        }
        printf("hello");
    } else {
        // regular stuff
        // placing the move to the board
        stateUnion su; 
        su.s = *p;

        // find what piece moved
        size_t pieceMoved = 0; 
        for (size_t i = 0; i < 12; i++) 
            pieceMoved += i * ((su.pieces[i] & fromBoard) > 0); 

        // move it
        moveBit(&(su.pieces[pieceMoved]), from, to);
        *p = su.s;
        p->turn = !p->turn;
    }
}



int main() {
    currentState = prevState = prevprevState = initializeState();
    // char input[4] = {'e', '2', 'e', '3'};
    char input[4];

    printGameBoard(currentState);
    int from, to;

while(1) {
    scanf("%s", input);
    U64 occupied = allOccupied(currentState);
    U64 blackBoard = blackOccupied(currentState);
    U64 whiteBoard = whiteOccupied(currentState);

    parseLAN(input, &from, &to);

    U64 candidateMoves = generateMoveFromTargetSquare(&currentState, from, occupied);
    // This gives you actual moves that you can make (I hope)
    // You AND with NOT of your pieces to discard friendly pieces
    U64 possibleMoves = candidateMoves & ~whiteBoard;
    // printBitboard(possibleMoves);
    
    if( (1ULL << to) & possibleMoves) {
        doMove(&currentState, from, to);
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
