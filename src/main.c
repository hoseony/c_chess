#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "engine.h"
#include "bitboards.h"
#include "move_generation.h"
#include "parse.h"
#include "legal_move.h"
#include "finding_magic.h"
#include <raylib.h>


// ---------------------------------------------------------------
State currentState;
State prevState;
State prevprevState; 

EngineThreadData engineData;
pthread_t engineThread;
// ---------------------------------------------------------------

void *makeEngineThread(void *arg) {
    EngineThreadData *data = (EngineThreadData *)arg;

    searchStartTime = clock();
    searchTimeLimit = 5000;

    data->bestMove = negmaxBestMove(&data->currentState, data->prevState, 5, data->rookMagic, data->bishopMagic);
    data->doneSearching = true;
    data->searching = false;

    return NULL;
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

inline static int retrievePieceForUnion(State p, U64 square) {
    StateUnion su; 
    int i; 

    su.s = p;
    
    int turnLoopValue = (p.turn == SIDE_WHITE) ? 0 : 5; 
    
    for (i = turnLoopValue; i < turnLoopValue + 6; i++) {
        if ((su.pieces[i] & square) > 0) {
            printf("%d\n", i);
            return i; 
        }
    }
    
    return -1; 
}

int main() {
    // game logic 
    RookMagic rookMagic;
    BishopMagic bishopMagic;
    prepareMagic(&rookMagic, &bishopMagic);

    // runPerft(&rookMagic, &bishopMagic);

    // list of all possible moves
    Move moves[218];

    char input[5];
    Move m;

    // printGameBoard(currentState);

    bool validSquareForDrawing = false; 
    int drawSquareClicked = -1; 
    int pieceSelected = -1;

    bool isValidPieceSelection = false; 
    U64 legalMoves, moveSelected;
    StateUnion state_union; 

    InitWindow(screenWidth, screenHeight, "Chess!");
    Vector2 mousePosition; 

    state_union.s = currentState = prevState = prevprevState = initializeState();  
    
    //-------------------------------- LOADING TEXTURE ------------------------------------
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

        /* 
                
        // User Move parse 
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
        // Draw
        //----------------------------------------------------------------------------------
        engineData.rookMagic = &rookMagic;
        engineData.bishopMagic = &bishopMagic;

        BeginDrawing();
            
            ClearBackground((Color){130, 130, 130, 255});
            drawSquareColors();
            drawPieces(currentState);

            if (currentState.turn == SIDE_BLACK) {
                // Kick off search if not already running
                if (!engineData.searching && !engineData.doneSearching) {
                    engineData.currentState = currentState;
                    engineData.prevState = prevState;
                    engineData.searching = true;
                    engineData.doneSearching = false;
                    pthread_create(&engineThread, NULL, makeEngineThread, &engineData);
                }

                if (engineData.doneSearching == true) {
                    pthread_join(engineThread, NULL);
                    State temp = currentState;
                    doMove(&currentState, &prevState, engineData.bestMove.from, engineData.bestMove.to, true);
                    prevState = temp;
                    engineData.doneSearching = false;
                    printGameBoard(currentState);
                }
            }

            if (currentState.turn == SIDE_WHITE) {
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
                    
                    moveSelected = legalMoves & squareClicked(mousePosition);
                        // & (~((currentState.turn != SIDE_WHITE) ? (whiteOccupied(currentState)) : (blackOccupied(currentState))));

                    printBitboard(legalMoves);
                    isValidPieceSelection = ((1ULL << pieceSelected) & 
                            ((currentState.turn == SIDE_WHITE) ? whiteOccupied(currentState) : blackOccupied(currentState))); 
                    if (moveSelected > 0 && pieceSelected > -1 && isValidPieceSelection) {
                        State temp = currentState;
                        doMove(&currentState, &prevState, pieceSelected, popLSB(&moveSelected), true);
                        // UPDATE prevState!!!
                        prevState = temp;
                        
                        printGameBoard(currentState);
                    }

                    drawSquareClicked = squareClickedi(mousePosition);  
                    pieceSelected = squareClickedi(mousePosition);
                }
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



