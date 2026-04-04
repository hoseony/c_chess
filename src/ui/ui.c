#include "../include/raylib.h"

static const int screenWidth = 640; 
static const int screenHeight = 480; 

int main() {
    InitWindow(screenWidth, screenHeight, "Basic Window"); 
    
    SetTargetFPS(60); 
    
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE); 
        DrawText("This is a window!", 190, 200, 20, LIGHTGRAY); 

        EndDrawing(); 
    } 

    CloseWindow();

    return 0;

}

