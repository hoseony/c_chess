#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "bitboards.h"
#include "move_generation.h"
#include "parse.h"
#include "legal_move.h"

State currentState;
State prevState;
State prevprevState; 


int main() {
    currentState = prevState = prevprevState = initializeState();
    srand(time(NULL));
   
    // list of possible moves
    Move moves[218];

    while(1) {
        int r = rand(); 

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

        Move m = moves[r % moveCount];

        State oldState = currentState;
        printf("playing: %d -> %d\n", m.from, m.to);
        doMove(&currentState, &prevState, m.from, m.to, true);

        prevprevState = prevState;
        prevState = oldState;

        printGameBoard(currentState);

    }

    return 0;
}

