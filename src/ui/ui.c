#include "../include/raylib.h"
#include "../bitboards.c" 
#include "../legal_move.c"

extern int popLSB(U64 *board); 


static const unsigned int screenWidth = 1280; 
static const unsigned int screenHeight = 960;

static const unsigned int gridSquareLength = 80;

static const unsigned int gridStartingPointW = (screenWidth - 8 * gridSquareLength) / 2; 
static const unsigned int gridStartingPointH = (screenHeight - 8 * gridSquareLength) / 2;

State currentState;
State prevState; 
State prevprevState; 

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

inline static void drawBitboard(U64 )


int main() {
    
    InitWindow(screenWidth, screenHeight, "Chess!");
    Vector2 mousePosition; 
    
    SetTargetFPS(60); 

    currentState = prevState = prevprevState = initializeState();  
        
    // White pieces
    piecesTextures[WHITE_PAWN]   = LoadTexture("../../assets/pieces/white-pawn.png");
    piecesTextures[WHITE_KNIGHT] = LoadTexture("../../assets/pieces/white-knight.png");
    piecesTextures[WHITE_BISHOP] = LoadTexture("../../assets/pieces/white-bishop.png");
    piecesTextures[WHITE_ROOK]   = LoadTexture("../../assets/pieces/white-rook.png");
    piecesTextures[WHITE_QUEEN]  = LoadTexture("../../assets/pieces/white-queen.png");
    piecesTextures[WHITE_KING]   = LoadTexture("../../assets/pieces/white-king.png");

    // Black pieces
    piecesTextures[BLACK_PAWN]   = LoadTexture("../../assets/pieces/black-pawn.png");
    piecesTextures[BLACK_KNIGHT] = LoadTexture("../../assets/pieces/black-knight.png");
    piecesTextures[BLACK_BISHOP] = LoadTexture("../../assets/pieces/black-bishop.png");
    piecesTextures[BLACK_ROOK]   = LoadTexture("../../assets/pieces/black-rook.png");
    piecesTextures[BLACK_QUEEN]  = LoadTexture("../../assets/pieces/black-queen.png");
    piecesTextures[BLACK_KING]   = LoadTexture("../../assets/pieces/black-king.png");
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        //----------------------------------------------------------------------------------
        mousePosition = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) printBitboard(squareClicked(mousePosition)); 

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground((Color){130, 130, 130, 255});

            // NOTE: Using DrawTexturePro() we can easily rotate and scale the part of the texture we draw
            // sourceRec defines the part of the texture we use for drawing
            // destRec defines the rectangle where our texture part will fit (scaling it to fit)
            // origin defines the point of the texture used as reference for rotation and scaling
            // rotation defines the texture rotation (using origin as rotation point)
            drawSquareColors();
            drawPieces(currentState);
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



