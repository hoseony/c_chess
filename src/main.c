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


int main() {
    currentState = prevState = prevprevState = initializeState();
    char input[4] = {'f', '1', 'e', '4'};
    int from, to;

    U64 occupied = allOccupied(currentState);

    printGameBoard(currentState);
    parseLAN(input, &from, &to);

    printBitboard(generateMoveFromTargetSquare(&currentState, from, occupied));

    return 0;
}
