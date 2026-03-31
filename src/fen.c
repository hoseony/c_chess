#include <stdio.h>
#include "types.h"

State fenToState(char *fen) {
    State state = {0};
    int squareIndex = 56;

    while(*fen != ' ') {
        char c = *fen++;

        if (c == '/') {
            squareIndex -= 16;
        } else if((c >= '1') && (c <= '8')) {
            squareIndex += (c - '0');
        } else {
            switch(c) {
                case 'p': state.bp |= (1ULL << squareIndex); break;
                case 'n': state.bn |= (1ULL << squareIndex); break;
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

    state.turn = (*fen = 'b') ? BLACK : WHITE;
    return state;
    
    // I still haven't added casteling part
    // It just parse until                     here->| 
    // rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
}
