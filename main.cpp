#include "raylib.h"
#include "Constants.h" 
#include "GameState.h" 
#include <cstdlib>    // For srand
#include <ctime>      

int main() {
    // Initialization
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Advanced Brick Breaker - Gregory.Dearham@LinkedIN ");
    InitAudioDevice();
    SetTargetFPS(144);
    srand(time(NULL)); // Seed random number generator once

    // Load global resources (font, sounds) using the function from GameState.cpp
    LoadGameResources();

    // Set initial game state 
    currentGameState = START_SCREEN;
    // InitGame(); // InitGame is now called when transitioning from START_SCREEN

    // Main game loop
    while (!WindowShouldClose()) { 
        UpdateDrawFrame(); // This function now handles state switching, updates, and drawing
    }

    UnloadGameResources();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}