#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "bitboards.h"
#include "move_generation.h"
#include "fen.h"
#include "legal_move.h"

State currentState;
State prevState;
State prevprevState; 

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

int main() {
    runPerft();
    return 0;
}

/*
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
*/




/*
int main() {
    currentState = prevState = prevprevState = initializeState();

    // example position (there should be a better way than loading 3 position manually...)
    char startingPosition[] = "r1bqk2r/pppp1ppp/2n2n2/1B2p3/1b2P3/2N2N2/PPPP1PPP/R1BQK2R w KQkq - 6 5";
    currentState = fenToState(startingPosition);

    char prevPosition[] = "r1bqkb1r/pppp1ppp/2n2n2/1B2p3/4P3/2N2N2/PPPP1PPP/R1BQK2R b KQkq - 5 4";
    prevState = fenToState(prevPosition);

    char prevprevPosition[] = "r1bqkb1r/pppp1ppp/2n2n2/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 4 4";
    prevprevState = fenToState(prevprevPosition);
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

        // This gives you actual moves that the player can make
        // You AND with NOT of your pieces to discard friendly pieces
        U64 candidateMoves = generateMoveFromTargetSquare(&currentState, &prevState, from, occupied);

        // This is the pseudo-legal move (legal move without checking check)
        U64 possibleMoves = (currentState.turn == WHITE) ? (candidateMoves & ~whiteBoard) : (candidateMoves & ~blackBoard);
        
        // Here we check if the king is in check after the move (meaning illegal move)
        if( (1ULL << to) & possibleMoves) {
            State temp = currentState;       // Save the current State
            doMove(&currentState, &prevState, from, to, true); // Try the move
            
            // These are to check check
            State afterMove = currentState;
            afterMove.turn = !afterMove.turn;

            if (isInCheck(afterMove)) { // if in check, Undo the move 
                currentState = temp;
                printf("Illegal Move; King Check\n");
            } else { // else, Domove
                prevprevState = prevState;
                prevState = temp;

                printGameBoard(currentState);
                if (areYouMated(&currentState, &prevState)) {
                    printf("Checkmate! %s won!\n", (currentState.turn == WHITE) ? "Black" : "White");
                    break;
                }
            }
        } else {
            printf("invalid move\n");
        }
    }
    return 0;
}
*/
