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

void doMove() {

}



int main() {
    currentState = prevState = prevprevState = initializeState();
    char input[4] = {'e', '1', 'e', '4'};
    int from, to;

    U64 occupied = allOccupied(currentState);
    U64 blackBoard = blackOccupied(currentState);
    U64 whiteBoard = whiteOccupied(currentState);

    printGameBoard(currentState);
    parseLAN(input, &from, &to);

    U64 candidateMoves = generateMoveFromTargetSquare(&currentState, from, occupied);
    printBitboard(candidateMoves);

    // This gives you actual moves that you can make (I hope)
    // You AND with NOT of your pieces to discard friendly pieces
    U64 possibleMoves = candidateMoves & ~whiteBoard;
    printBitboard(possibleMoves); 
    
    // if your targetSquare is included in the possibleMoves (if the move is somewhat valid)
    // You should now check if that move result in check
    // This will be done with do, undo move
    // Then see if king is in the attackBoard after the move
    // I already made whiteAttackBoard and blackAttackBoard
    // If so, discard that move (set that bit to 0)
    // and you have final legalMoves

    return 0;
}
