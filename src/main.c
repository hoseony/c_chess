#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "bitboards.h"
#include "move_generation.h"
#include "parse.h"
#include "legal_move.h"
#include "finding_magic.h"
#include <raylib.h>

// ---------------------------------------------------------------
int pieceSquareTable(U64 board, int pieceSquareTable[64], int turn);
int positionEvaluation(State p);
int negamax(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic);
Move negmaxBestMove(State *p, State prev, int depth, RookMagic *rookMagic, BishopMagic *bishopMagic);
int qsearch(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic);
// ---------------------------------------------------------------

State currentState;
State prevState;
State prevprevState; 

clock_t searchStartTime;
int searchTimeLimit;

// ---------------------------------------------------------------
// Piece Square Table
static int king[64] = {
    -80, -80, -80, -80, -80, -80, -80, -80,
    -80, -80, -80, -80, -80, -80, -80, -80,
    -80, -80, -80, -80, -80, -80, -80, -80,
    -80, -80, -80, -80, -80, -80, -80, -80,
    -60, -60, -60, -60, -60, -60, -60, -60,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -20, -20, -20, -20, -20, -20, -20, -20,
     20,  30,  10, -30, -10, -20,  30,  20,
};

static int kingEnd[64] = {
    -10,   10,  -10, -10, -10,  -10, -10,  -10,
    -10,   00,   00,  00,  00,   00,  00,  -10,
    -10,   00,   10,  15,  15,   10,  00,  -10,
    -10,   00,   15,  20,  20,   15,  00,  -10,
    -10,   00,   15,  20,  20,   15,  00,  -10,
    -10,   00,   25,  15,  15,   25,  00,  -10,
    -10,   00,   00,  00,  00,   00,  00,  -10,
    -10,  -30,  -20,  00,  00,  -20, -30,  -10,
};

static int pawn[64] = {
    900, 900, 900,  900,  900, 900, 900, 900, // promotion goated
    00,  00,  00,   00,   00,  00,  00,  00,
    00,  00,  00,   10,   10,  00,  00,  00,
    00,  00,  10,   20,   20,  10,  00,  00,
    00,  00,  10,   20,   20,  10,  00,  00,
    05,  00,  00,   00,   00,  00,  00,  05,
    00,  00,  00,  -15,  -15,  00,  00,  00,
    00,  00,  00,   00,   00,  00,  00,  00,
};

static int knight[64] = {
    -10,   10,  -10, -10, -10,  -10, -10,  -10,
    -10,   00,   00,  00,  00,   00,  00,  -10,
    -10,   00,   10,  15,  15,   10,  00,  -10,
    -10,   00,   15,  20,  20,   15,  00,  -10,
    -10,   00,   15,  20,  20,   15,  00,  -10,
    -10,   00,   25,  15,  15,   25,  00,  -10,
    -10,   00,   00,  00,  00,   00,  00,  -10,
    -10,  -30,  -20,  00,  00,  -20, -30,  -10,
};

static int bishop[64] = {
    -10, -10, -10, -10, -10, -10, -10, -10,
    -10,  00,  10,  10,  10,  10,  00, -10,
    -10,  10,  15,  15,  15,  15,  10, -10,
    -10,  10,  15,  10,  10,  15,  10, -10,
    -10,  10,  15,  15,  15,  15,  10, -10,
    -10,  10,  15,  05,  05,  15,  10, -10,
    -10,  00,  10,  10,  10,  10,  00, -10,
    -10, -10, -30, -10, -10, -30, -10, -10,
};

static int rook[64] = {
     00, 00, 00, 00, 00, 00, 00,  00,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
     00, 00, 00, 20, 20, 00, 00,  00,
};

static int queen[64] = {
     00, 00, 00, 00, 00, 00, 00,  00,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
    -10, 00, 00, 00, 00, 00, 00, -10,
     00, 00, 00, 10, 10, 00, 00,  00,
};

// ---------------------------------------------------------------

int pieceSquareTable(U64 board, int pieceSquareTable[64], int turn) {
    int score = 0;
    while(board) {
        int square = popLSB(&board);
        int index = (turn == 1) ? (square) : (square ^ 56); 
        // 63 in binary is 111111
        // 63 should become 7 which is 000111
        // 53 in binary is 111000
        // and xor will do the job. 
        // This holds true for any pattern.
        // more can be found here: https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
        
        score += pieceSquareTable[index];
    }
    return score;
}


int positionEvaluation(State p) {
    int score = 0;

    // piece value
    // https://gcc.gnu.org/onlinedocs/gcc/Bit-Operation-Builtins.html
    // more funny things

    score += __builtin_popcountll(p.wp) * 100 + pieceSquareTable(p.wp, pawn, SIDE_WHITE);
    score += __builtin_popcountll(p.wn) * 300 + pieceSquareTable(p.wn, knight, SIDE_WHITE);
    score += __builtin_popcountll(p.wb) * 300 + pieceSquareTable(p.wb, bishop, SIDE_WHITE);
    score += __builtin_popcountll(p.wr) * 500 + pieceSquareTable(p.wr, rook, SIDE_WHITE);
    score += __builtin_popcountll(p.wq) * 900 + pieceSquareTable(p.wq, queen, SIDE_WHITE);
    score += pieceSquareTable(p.wk, king, SIDE_WHITE);

    score -= __builtin_popcountll(p.bp) * 100 + pieceSquareTable(p.bp, pawn, SIDE_BLACK);
    score -= __builtin_popcountll(p.bn) * 300 + pieceSquareTable(p.bn, knight, SIDE_BLACK);
    score -= __builtin_popcountll(p.bb) * 300 + pieceSquareTable(p.bb, bishop, SIDE_BLACK);
    score -= __builtin_popcountll(p.br) * 500 + pieceSquareTable(p.br, rook, SIDE_BLACK);
    score -= __builtin_popcountll(p.bq) * 900 + pieceSquareTable(p.bq, queen, SIDE_BLACK);
    score -= pieceSquareTable(p.bk, king, SIDE_BLACK);

    return (p.turn == SIDE_WHITE) ? (score) : (-score);
}


/* I did not came up with this algorithm..
 * 
 * Pseudo code:
 *
 * int negaMax( int depth ) {
 *     if ( depth == 0 ) return evaluate();
 *     int max = -oo;
 *     for ( all moves)  {
 *         score = -negaMax( depth - 1 );
 *         if( score > max )
 *             max = score;
 *     }
 *     return max;
 * }
 */

// https://en.wikipedia.org/wiki/Negamax
int negamax(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic) {
    // alpha and beta are lower and upper bound for a child node.
    // The idea is you start from alpha -infinity, beta +infinity, and 
    // if your score gets out of this bound, you cut off the search

    // This is for setting timeout for the search (since of course)
    // https://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
    static int nodeCount = 0; 
    // I do this to check the timeout condition less... 
    // Hope this helps a little bit in terms of performance
    if ((++nodeCount % 1024) == 0) {
        if (((clock() - searchStartTime) * 1000 / CLOCKS_PER_SEC) > searchTimeLimit) {
            // printf("search timeout\n");
            return 0;
        }
    }

    // base case
    if (depth == 0) {
        // return positionEvaluation(*p);
        return qsearch(p, prev, 6, alpha, beta, rookMagic, bishopMagic);
        // https://www.dogeystamp.com/chess5/
    }

    // get possible moves
    Move moves[218];
    int moveCount = generateLegalMove(*p, prev, moves, 218, rookMagic, bishopMagic);

    int value = -999999; // this will keep track of the best score

    // ideally, move ordering should go here.

    // checkmate or stalemate
    if (moveCount == 0) {
        if (isInCheck(*p, rookMagic, bishopMagic)) {
            // being mated is bad
            return -999999;
        } else {
            // draw is mid
            return -100;
            // I don't really want it to go for the draw, so I gave some -
        }
    }

    for(int i = 0; i < moveCount; i++) {
        State temp = *p;
        // you do the move

        doMove(&temp, &prev, moves[i].from, moves[i].to, false);

        int score = -negamax(&temp, *p, depth - 1, -beta, -alpha, rookMagic, bishopMagic);
       
        // This pruning way is called fail-soft variation of alpha-beta prunning
        
        // update the best score to value
        if (score > value) {
            value = score;
        }

        // when you found a move that's better, you don't need to search moves that
        // are worse than that
        if (value > alpha) {
            alpha = value;
        }

        // If your alpha is greater than beta, meaning your lower bound is higher than opponent's upperbound,
        // Your opponent will be smart enough to not let that happen.
        // prune the branch
        if (alpha >= beta) {
            break;
        }
    }
    return value;
}

Move negmaxBestMove(State *p, State prev, int depth, RookMagic *rookMagic, BishopMagic *bishopMagic) {
    Move moves[218]; // 218 is the max possible moves,  
                     // this stores every legal move
                     
    int moveCount = generateLegalMove(*p, prev, moves, 218, rookMagic, bishopMagic);
    int numPieces = __builtin_popcountll(allOccupied(*p));

    // hardcoded search depth
    if (numPieces <= 10) {
        depth = 8;
    } else if (numPieces < 15){
        depth = 7;
    }

    Move best = moves[0];
    int alpha = -999999;
    int beta = 999999;

    for(int i = 0; i < moveCount; i++) {
        State temp = *p;
        doMove(&temp, &prev, moves[i].from, moves[i].to, false);

        int score = -negamax(&temp, *p, depth - 1, -beta, -alpha, rookMagic, bishopMagic);
        //printf("negamax: root move %d -> %d score %d\n", moves[i].from, moves[i].to, score);

        // you found a better move. Nice!
        // (basically, it is trying every square (and its branch), to see if its a good move)
        if (score > alpha) {
            alpha = score;
            best = moves[i];
        }
    }

    printf("searched depth %d, timeout: %d\n", depth, ((clock() - searchStartTime) * 1000 / CLOCKS_PER_SEC) > searchTimeLimit);
    return best;
}

// https://www.chessprogramming.org/Quiescence_Search
int qsearch(State *p, State prev, int depth, int alpha, int beta, RookMagic *rookMagic, BishopMagic *bishopMagic) {
    int staticEval = positionEvaluation(*p);
    
    if (depth == 0) {
        return positionEvaluation(*p);
    }

    int best_value = staticEval;
    
    if (best_value >= beta) {
        return best_value;
    }

    if (best_value > alpha) {
        alpha = best_value;
    }

    Move moves[218];
    int moveCount = generateLegalMove(*p, prev, moves, 218, rookMagic, bishopMagic);
    U64 enemyBoard = (p->turn == SIDE_WHITE) ? (blackOccupied(*p)) : (whiteOccupied(*p));

    for (int i = 0; i < moveCount; i++) {
        U64 toBoard = (1ULL << moves[i].to);

        // you want to keep searching the captures (moves that are not quite)
        if ( (toBoard & enemyBoard) == 0 ) {
            continue;
        }

        State temp = *p;
        doMove(&temp, &prev, moves[i].from, moves[i].to, false);

        int score = -qsearch(&temp, *p, depth - 1, -beta, -alpha, rookMagic, bishopMagic);
        //printf("qsearch: root move %d -> %d score %d\n", moves[i].from, moves[i].to, score);

        if (score >= beta) {
            return score;
        } 

        if (score > best_value) {
            best_value = score;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

extern int popLSB(U64 *board); 


static const unsigned int screenWidth = 1280; 
static const unsigned int screenHeight = 960;

static const unsigned int gridSquareLength = 80;

static const unsigned int gridStartingPointW = (screenWidth - 8 * gridSquareLength) / 2; 
static const unsigned int gridStartingPointH = (screenHeight - 8 * gridSquareLength) / 2;

enum piecesTextureID {
    WHITE_PAWN = 0, 
    WHITE_KNIGHT = 1,
    WHITE_BISHOP = 2,
    WHITE_ROOK = 3,
    WHITE_QUEEN = 4,
    WHITE_KING = 5,

    BLACK_PAWN = 6,
    BLACK_KNIGHT = 7,
    BLACK_BISHOP = 8,
    BLACK_ROOK = 9,
    BLACK_QUEEN = 10,
    BLACK_KING = 11
};


static Texture2D piecesTextures[12]; 

static void drawSquareColors() {
    Color lightSquare = (Color){238, 238, 210, 255};
    Color darkSquare = (Color){100, 120, 100, 255};
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            DrawRectangle(gridStartingPointW + j * gridSquareLength, gridStartingPointH + i * gridSquareLength, 
                    gridSquareLength, gridSquareLength, ((j + i) % 2 == 0) ? lightSquare : darkSquare); 
        }
    }
    
}

inline static int indexGrid(int rank, int file) {
    return rank * 8 + file; 
}


static void drawPieces(State p) {
    

    StateUnion su;

    Rectangle sourceRec; 
    Rectangle destRec;


    su.s = p; 


    for (int i = 0; i < 12; i++) {
        for (;su.pieces[i] > 0; ) {
            int square = popLSB(&su.pieces[i]);
            // row = square / 8 
            // col = square % 8 
            
            sourceRec = (Rectangle){0.0f, 0.0f, piecesTextures[i].width, piecesTextures[i].height}; 
            destRec = (Rectangle){gridStartingPointW + (1.6 + (square % 8)) * gridSquareLength, 
                gridStartingPointH + (1.6 + (7 - (square / 8))) * gridSquareLength,
            gridSquareLength, gridSquareLength}; 
            DrawTexturePro(piecesTextures[i], sourceRec, destRec, 
                    (Vector2){(float)piecesTextures[i].width, (float)piecesTextures[i].height}, 0.0f, (Color){255, 255, 255, 255}); 
        }
    }
}

inline static U64 squareClicked(Vector2 mousePosition) {
    U64 boardPosClicked = 1ULL << (((int)mousePosition.x - gridStartingPointW) / gridSquareLength);
    boardPosClicked <<= 8 * (7 - ((int)mousePosition.y - gridStartingPointH) / gridSquareLength); 
    return boardPosClicked; 
}

inline static int squareClickedi(Vector2 mousePosition) {
    return (((int)mousePosition.x - gridStartingPointW) / gridSquareLength) 
        + 8 * (7 - (((int)mousePosition.y - gridStartingPointH) / gridSquareLength));
} 

inline static void drawPossibleMoves(U64 moves) {
    int square; 
    Color highlightSquare = (Color){230, 155, 99, 255};
    while (moves > 0) { 
        square = popLSB(&moves); 
        DrawRectangle(gridStartingPointW + (square % 8) * gridSquareLength + 3 * gridSquareLength / 8, 
                gridStartingPointH + (7 - (square / 8)) * gridSquareLength + 3 * gridSquareLength / 8, 
                    gridSquareLength / 4, gridSquareLength / 4, highlightSquare); 
    }
        
}


int main() {
    
    // game logic 

    RookMagic rookMagic;
    BishopMagic bishopMagic;
    prepareMagic(&rookMagic, &bishopMagic);

    // runPerft(&rookMagic, &bishopMagic);

    // list of possible moves
    Move moves[218];

    char input[5];
    Move m;

    printGameBoard(currentState);

    bool validSquareForDrawing = false; 
    int drawSquareClicked = -1; 
    U64 legalMoves; 

    InitWindow(screenWidth, screenHeight, "Chess!");
    Vector2 mousePosition; 
    
    SetTargetFPS(60); 

    currentState = prevState = prevprevState = initializeState();  
        
    // White pieces
    piecesTextures[WHITE_PAWN]   = LoadTexture("../assets/pieces/white-pawn.png");
    piecesTextures[WHITE_KNIGHT] = LoadTexture("../assets/pieces/white-knight.png");
    piecesTextures[WHITE_BISHOP] = LoadTexture("../assets/pieces/white-bishop.png");
    piecesTextures[WHITE_ROOK]   = LoadTexture("../assets/pieces/white-rook.png");
    piecesTextures[WHITE_QUEEN]  = LoadTexture("../assets/pieces/white-queen.png");
    piecesTextures[WHITE_KING]   = LoadTexture("../assets/pieces/white-king.png");

    // Black pieces
    piecesTextures[BLACK_PAWN]   = LoadTexture("../assets/pieces/black-pawn.png");
    piecesTextures[BLACK_KNIGHT] = LoadTexture("../assets/pieces/black-knight.png");
    piecesTextures[BLACK_BISHOP] = LoadTexture("../assets/pieces/black-bishop.png");
    piecesTextures[BLACK_ROOK]   = LoadTexture("../assets/pieces/black-rook.png");
    piecesTextures[BLACK_QUEEN]  = LoadTexture("../assets/pieces/black-queen.png");
    piecesTextures[BLACK_KING]   = LoadTexture("../assets/pieces/black-king.png");
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        /*
        int moveCount = generateLegalMove(currentState, prevState, moves, 218, &rookMagic, &bishopMagic);
        
        if (moveCount == 0) { //if there's no move, it's either mate or stalemate, for now, I do not care
            if (isInCheck(currentState, &rookMagic, &bishopMagic)) {
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

        if (currentState.turn == SIDE_WHITE) {
            printf("Your move?: ");
            scanf("%s", input);
            int from, to;

            parseLAN(input, &from, &to);

            bool wasThatValidMove = false;
            for (int i = 0; i < moveCount; i++) {
                if(moves[i].from == from && moves[i].to == to) {
                    wasThatValidMove = true;
                    m = moves[i];
                    break;
                }
            }

            if (wasThatValidMove == false) {
                printf("Illegal Move! try again :(\n");
                continue;
            }
        } else {
            searchStartTime = clock();
            searchTimeLimit = 5000; // ms
            m = negmaxBestMove(&currentState, prevState, 5, &rookMagic, &bishopMagic);
        }

        State oldState = currentState;
        // printf("playing: %d -> %d\n", m.from, m.to);
        doMove(&currentState, &prevState, m.from, m.to, true);

        prevprevState = prevState;
        prevState = oldState;

        printGameBoard(currentState);
        */


        // Update
        //----------------------------------------------------------------------------------
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground((Color){130, 130, 130, 255});
            drawSquareColors();
            drawPieces(currentState);
            
            validSquareForDrawing = ((drawSquareClicked < 64 && drawSquareClicked > 0) 
                    && (1ULL << drawSquareClicked) &
                    ((currentState.turn == SIDE_WHITE) ? whiteOccupied(currentState) 
                    : blackOccupied(currentState)) 
                    ) ? true : false; 
            
            legalMoves = legalBitboard(
                            &currentState,         
                            &prevState, 
                            drawSquareClicked,
                            allOccupied(currentState), 
                            (currentState.turn == SIDE_WHITE) ? blackAttackBoard(currentState, &rookMagic, &bishopMagic) 
                            : whiteAttackBoard(currentState, &rookMagic, &bishopMagic),
                            &rookMagic,
                            &bishopMagic
                        );

            if (validSquareForDrawing) {
                drawPossibleMoves(legalMoves);
            }



            mousePosition = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (legalMoves & squareClickedi(mousePosition) > 0) {
                    
                }

                drawSquareClicked = squareClickedi(mousePosition);  
            }
            // NOTE: Using DrawTexturePro() we can easily rotate and scale the part of the texture we draw
            // sourceRec defines the part of the texture we use for drawing
            // destRec defines the rectangle where our texture part will fit (scaling it to fit)
            // origin defines the point of the texture used as reference for rotation and scaling
            // rotation defines the texture rotation (using origin as rotation point)
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    for (int i = 0; i < 12; i++) 
        UnloadTexture(piecesTextures[i]);        // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
     
    return 0;
}



