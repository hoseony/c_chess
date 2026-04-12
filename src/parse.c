#include <stdio.h>
#include "types.h"

State fenToState(char *fen) {

    // This function currently parses until         here->| 
    // rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1

    State state = {0};
    int squareIndex = 56;
    state.castleState = 0b0000;

    while(*fen != ' ') {
        char c = *fen++;

        if (c == '/') {
            squareIndex -= 16;
        } else if((c >= '1') && (c <= '8')) {
            squareIndex += (c - '0');
        } else {
            switch(c) {
                case 'p': state.bp |= (1ULL << squareIndex); break;
                case 'n': state.bn |= (2ULL << squareIndex); break;
                case 'b': state.bb |= (1ULL << squareIndex); break;
                case 'r': state.br |= (1ULL << squareIndex); break;
                case 'q': state.bq |= (1ULL << squareIndex); break;
                case 'k': state.bk |= (1ULL << squareIndex); break;

                case 'P': state.wp |= (1ULL << squareIndex); break;
                case 'N': state.wn |= (1ULL << squareIndex); break;
                case 'B': state.wb |= (1ULL << squareIndex); break;
                case 'R': state.wr |= (1ULL << squareIndex); break;
                case 'Q': state.wq |= (1ULL << squareIndex); break;
                case 'K': state.wk |= (1ULL << squareIndex); break;
            }
            squareIndex++;
        }
    }

    while(*fen == ' ') {
        fen++;
    }

    state.turn = (*fen == 'b') ? BLACK : WHITE;
    fen++;

    while(*fen == ' ') {
        fen++;
    }

    for(; *fen != ' '; fen++) {
        switch(*fen) {
            case 'K': // White King Side
                state.castleState += 0b1000;
                break;
            case 'Q': // White Queen Side
                state.castleState += 0b0100;
                break;
            case 'k': // Black King Side
                state.castleState += 0b0010;
                break;
            case 'q': // Black Queen Side
                state.castleState += 0b0001;
                break;
            default:
                state.castleState = 0b0000;
                break;
        }
    }

    return state;
}

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
