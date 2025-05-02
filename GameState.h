#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include <vector>
#include <string>
#include "Constants.h"
#include "Paddle.h" 
#include "Ball.h"
#include "Brick.h"
#include "Modifier.h"
#include "FloatingText.h"

//------------------------------------------------------------------------------------
// Global Variables (Declarations) - use 'extern'
//------------------------------------------------------------------------------------
extern Font gameFont; // Make font globally accessible if needed by multiple files (like FloatingText)
extern GameState currentGameState;
extern Paddle playerPaddle;
extern std::vector<Ball> balls;
extern Brick bricks[BRICK_ROWS][BRICK_COLUMNS];
extern std::vector<Modifier> activeModifiers;
extern std::vector<FloatingText> activeTextEffects;
extern Color currentBackgroundColor;
extern float backgroundFlashTimer;
extern int score;
extern int highScore;
extern int activeBricksCount;
extern float gameTimer;
extern Sound fxPaddleHit;
extern Sound fxBrickHit;
extern Sound fxPowerup;

//------------------------------------------------------------------------------------
// Function Declarations
//------------------------------------------------------------------------------------
void InitGame();
void ResetBricks();
void UpdateGame();
void DrawGame();
void UpdateDrawFrame();
void SpawnModifier(Vector2 position);
void SpawnTextEffect(Vector2 position, const std::string& text, Color color, int fontSize, Vector2 velocity, float lifeTime);
void ActivateModifier(Modifier& mod);
void PlaySfx(Sound& sfx);
void LoadGameResources();   
void UnloadGameResources();


#endif // GAME_STATE_H