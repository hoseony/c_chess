#include "../include/raylib.h"
#include "../types.h" 

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

static void drawGameboard(Texture2D *pieces) {
    enum piecesTextureID id = WHITE_PAWN; 
    Rectangle sourceRec = {0.0f, 0.0f, (float)pieces[id].width, (float)pieces[id].height};
    int piecePosition; 
    // draw pawn
    StateUnion tempStateUnion; 
    tempStateUnion.s = currentState; 
    for (int i = 0; i < 12; i++) {
        while (tempStateUnion.pieces[i] > 0) {
            piecePosition = popLSB((tempStateUnion.pieces)+i);
            DrawTexturePro(pieces[id], sourceRec, (Rectangle){piecePosition / 8, piecePosition % 8, gridSquareLength, 
                    gridSquareLength}, (Vector2){(float)pieces[id].width, (float)pieces[id].height}, 0.0f, (Color){255, 255, 255, 0}); 
        }
    }
}


static Texture2D piecesTextures[12]; 

int main() {
    InitWindow(screenWidth, screenHeight, "Chess!"); 
    
    SetTargetFPS(60); 
        
    // White pieces
    piecesTextures[WHITE_PAWN]   = LoadTexture("../../assets/pieces/white-pawn.png");
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
    
    int frameWidth = piecesTextures[WHITE_PAWN].width;
    int frameHeight = piecesTextures[WHITE_PAWN].height;

    // Source rectangle (part of the texture to use for drawing)
    Rectangle sourceRec = { 0.0f, 0.0f, (float)frameWidth, (float)frameHeight };

    // Destination rectangle (screen rectangle where drawing part of texture)
    Rectangle destRec = { screenWidth/2.0f, screenHeight/2.0f, frameWidth*2.0f, frameHeight*2.0f };

    // Origin of the texture (rotation/scale point), it's relative to destination rectangle size
    Vector2 origin = { (float)frameWidth, (float)frameHeight };

    int rotation = 0;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground((Color){130, 130, 130, 255});

            // NOTE: Using DrawTexturePro() we can easily rotate and scale the part of the texture we draw
            // sourceRec defines the part of the texture we use for drawing
            // destRec defines the rectangle where our texture part will fit (scaling it to fit)
            // origin defines the point of the texture used as reference for rotation and scaling
            // rotation defines the texture rotation (using origin as rotation point)
            DrawTexturePro(piecesTextures[WHITE_PAWN], sourceRec, destRec, origin, (float)rotation, (Color){255, 255, 255, 255});
            drawSquareColors();

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



