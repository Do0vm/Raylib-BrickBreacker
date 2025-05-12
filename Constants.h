#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h" // For Vector2

//------------------------------------------------------------------------------------
// Defines and Constants
//------------------------------------------------------------------------------------
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 900;

// Paddle Constants
const float PADDLE_W = 150.0f;
const float PADDLE_HE = 10.0f;
const float PADDLE_SPEED = 7.0f;
const float PADDLE_DECELERATION = 0.9f;
const float PADDLE_BOUNCE_MULTIPLIER = 0.8f; // Multiplier for paddle bounce angle

// Ball Constants
const float BALL_RADIUS = 10.0f;
const Vector2 INITIAL_BALL_SPEED = { 500.0f, -500.0f };
const float MAX_BALL_SPEED_X = 600.0f;

// Brick Constants
const int BRICK_ROWS = 5;
const int BRICK_COLUMNS = 10;
const float BRICK_HEIGHT = 20.0f;
const float BRICK_GAP = 2.0f;
const float BRICK_TOP_OFFSET = 50.0f;
const float BRICK_WIDTH = (WINDOW_WIDTH - (BRICK_COLUMNS + 1) * BRICK_GAP) / BRICK_COLUMNS;
const int SCORE_PER_BRICK = 99999;

// Modifier Constants
const float MODIFIER_CHANCE = 65.0f;
const float MODIFIER_SPEED = 400.0f;
const float MODIFIER_SIZE = 15.0f;

// Game State Enum
typedef enum {
    START_SCREEN,
    PLAYING,
    GAME_OVER
} GameState;

// Modifier Type Enum
typedef enum {
    MOD_NONE,
    MOD_MULTIBALL,
    MOD_SCORE_BONUS
} ModifierType;

// Background Flash Constants
const Color NORMAL_BG_COLOR = DARKGRAY;
const Color FLASH_BG_COLOR = RED; //  maybe change based on hit
const float FLASH_DURATION = 0.1f;

#endif // CONSTANTS_H