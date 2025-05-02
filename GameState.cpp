#include "GameState.h"
#include "Constants.h" // Include Constants header
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"
#include "Modifier.h"
#include "FloatingText.h"
#include <cmath>
#include <cstdlib> // For GetRandomValue, srand, time
#include <ctime>   // For time
#include <algorithm> // For std::remove_if
#include <iostream>  // For std::cerr (error reporting)

//------------------------------------------------------------------------------------
// Global Variables (Definitions) - Define without 'extern'
//------------------------------------------------------------------------------------
Font gameFont; // Actual definition
GameState currentGameState = START_SCREEN;
Paddle playerPaddle;
std::vector<Ball> balls;
Brick bricks[BRICK_ROWS][BRICK_COLUMNS]; // Definition for the array
std::vector<Modifier> activeModifiers;
std::vector<FloatingText> activeTextEffects;
Color currentBackgroundColor = NORMAL_BG_COLOR;
float backgroundFlashTimer = 0.0f;
int score = 0;
int highScore = 0; // Consider loading/saving this from a file later
int activeBricksCount = 0;
float gameTimer = 0.0f;
Sound fxPaddleHit;
Sound fxBrickHit;
Sound fxPowerup;

//------------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------------

void LoadGameResources() {
    gameFont = LoadFont("resources/fonts/alagard.png");
    if (gameFont.texture.id == 0) {
        std::cerr << "Warning: Failed to load font 'resources/fonts/alagard.png'. Using default font." << std::endl;
        gameFont = GetFontDefault(); // Use default font as fallback
    }

    fxPaddleHit = LoadSound("resources/sounds/paddle_hit.wav");
    fxBrickHit = LoadSound("resources/sounds/brick_hit.wav");
    fxPowerup = LoadSound("resources/sounds/powerup.wav");

    // Check if sounds loaded 
    if (fxPaddleHit.stream.buffer == nullptr) std::cerr << "Warning: Failed to load sound paddle_hit.wav" << std::endl;
    if (fxBrickHit.stream.buffer == nullptr) std::cerr << "Warning: Failed to load sound brick_hit.wav" << std::endl;
    if (fxPowerup.stream.buffer == nullptr) std::cerr << "Warning: Failed to load sound powerup.wav" << std::endl;
}

void UnloadGameResources() {
    UnloadSound(fxPaddleHit);
    UnloadSound(fxBrickHit);
    UnloadSound(fxPowerup);
    UnloadFont(gameFont);
}


// Initialize/Reset Game State
void InitGame() {
    // Reset game variables
    score = 0;
    gameTimer = 0.0f;
    balls.clear();
    activeModifiers.clear();
    activeTextEffects.clear();
    currentBackgroundColor = NORMAL_BG_COLOR;
    backgroundFlashTimer = 0.0f;

    // Initialize Paddle
    playerPaddle.Init(
        { (WINDOW_WIDTH / 2.0f) - (PADDLE_W / 2.0f), WINDOW_HEIGHT * 0.9f },
        { 0.0f, 0.0f }, // Initial speed (zero)
        PADDLE_W,
        PADDLE_HE,
        SKYBLUE);

    // Initialize Ball(s)
    Ball initialBall;
    initialBall.Init(
        { WINDOW_WIDTH / 2.0f, playerPaddle.GetPosition().y - PADDLE_H - BALL_RADIUS - 5 },
        INITIAL_BALL_SPEED,
        BALL_RADIUS,
        Color{ 2, 222, 233, 242 }
    );
    balls.push_back(initialBall);

    // Initialize Bricks
    ResetBricks(); // This also sets activeBricksCount
}

// Function to Reset/Initialize Bricks
void ResetBricks() {
    activeBricksCount = 0; // Reset the counter before initializing

    for (int r = 0; r < BRICK_ROWS; ++r) {
        for (int c = 0; c < BRICK_COLUMNS; ++c) {
            Vector2 brickPos = {
                c * (BRICK_WIDTH + BRICK_GAP) + BRICK_GAP,
                r * (BRICK_HEIGHT + BRICK_GAP) + BRICK_GAP + BRICK_TOP_OFFSET
            };
            Vector2 brickSize = { BRICK_WIDTH, BRICK_HEIGHT };

            // Determine lives based on row
            int lives = 1;
            if (r < 1) lives = 3;       // Top row gets 3 lives
            else if (r < 3) lives = 2; // Next two rows get 2 lives

            bricks[r][c].Init(brickPos, brickSize, r, c, lives);

            if (bricks[r][c].lives > 0) {
                activeBricksCount++;
            }
        }
    }
    // Only give bonus and text effect if it's NOT the very first level init
    if (gameTimer > 0.1f) { // Check if game has actually started
        score += 999999; // Example bonus
        SpawnTextEffect({ WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 3.0f }, "CLEARED! +999999", GOLD, 35, { 0, -40 }, 1.5f);
    }
}

// Update and Draw Frame
void UpdateDrawFrame() {
    switch (currentGameState) {
    case START_SCREEN:
        if (IsKeyPressed(KEY_ENTER)) {
            InitGame();
            currentGameState = PLAYING;
        }
        // Draw Start Screen elements
        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawTextEx(gameFont, "BRICK BREAKER", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "BRICK BREAKER", 60, 2).x / 2, WINDOW_HEIGHT / 4.0f }, 60, 2, YELLOW);
        DrawTextEx(gameFont, "EPILEPSY WARNING", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "EPILEPSY WARNING", 30, 2).x / 2, WINDOW_HEIGHT * 0.9f }, 30, 2, RED);
        DrawTextEx(gameFont, "Press [ENTER] to Start", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "Press [ENTER] to Start", 30, 2).x / 2, WINDOW_HEIGHT / 2.0f }, 30, 2, WHITE);
        DrawTextEx(gameFont, TextFormat("High Score: %i", highScore), { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, TextFormat("High Score: %i", highScore), 25, 2).x / 2, WINDOW_HEIGHT * 0.6f }, 25, 2, GOLD);
        DrawTextEx(gameFont, "Controls: A/D or Left/Right Arrows to Move", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "Controls: A/D or Left/Right Arrows to Move", 20, 1).x / 2, WINDOW_HEIGHT * 0.8f }, 20, 1, LIGHTGRAY);
        EndDrawing();
        break;

    case PLAYING:
        UpdateGame();
        DrawGame();
        break;

    case GAME_OVER:
        if (score > highScore) {
            highScore = score; // Consider saving high score here
        }
        if (IsKeyPressed(KEY_R)) {
            currentGameState = START_SCREEN; // Go back to start screen
        }
        // Draw Game Over Screen elements
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(gameFont, "GAME OVER", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "GAME OVER", 70, 2).x / 2, WINDOW_HEIGHT / 4.0f }, 70, 2, RED);
        DrawTextEx(gameFont, TextFormat("Final Score: %i", score), { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, TextFormat("Final Score: %i", score), 40, 2).x / 2, WINDOW_HEIGHT / 2.0f }, 40, 2, WHITE);
        DrawTextEx(gameFont, TextFormat("Time: %.2f s", gameTimer), { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, TextFormat("Time: %.2f s", gameTimer), 30, 2).x / 2, WINDOW_HEIGHT * 0.6f }, 30, 2, LIGHTGRAY);
        DrawTextEx(gameFont, TextFormat("High Score: %i", highScore), { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, TextFormat("High Score: %i", highScore), 30, 2).x / 2, WINDOW_HEIGHT * 0.68f }, 30, 2, GOLD);
        DrawTextEx(gameFont, "Press [R] to Restart", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "Press [R] to Restart", 30, 2).x / 2, WINDOW_HEIGHT * 0.8f }, 30, 2, YELLOW);
        EndDrawing();
        break;
    }
}

// Update Game Logic for PLAYING state
void UpdateGame() {
    float dt = GetFrameTime();
    gameTimer += dt;

    // Update Background Flash
    if (backgroundFlashTimer > 0.0f) {
        backgroundFlashTimer -= dt;
        if (backgroundFlashTimer <= 0.0f) {
            currentBackgroundColor = NORMAL_BG_COLOR;
        }
    }

    // Paddle Input and Movement
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        playerPaddle.SetSpeed(Vector2{ PADDLE_SPEED, 0.0f });
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        playerPaddle.SetSpeed(Vector2{ -PADDLE_SPEED, 0.0f });
    else
        playerPaddle.SetSpeed(Vector2{ playerPaddle.GetSpeed().x * PADDLE_DECELERATION, 0.0f });

    playerPaddle.Update(); // Update paddle position

    // Paddle Screen Bounds
    if (playerPaddle.GetPosition().x <= 0)
        playerPaddle.SetPosition(Vector2{ 0.0f, playerPaddle.GetPosition().y });
    else if (playerPaddle.GetPosition().x + playerPaddle.GetWidth() >= WINDOW_WIDTH)
        playerPaddle.SetPosition(Vector2{ WINDOW_WIDTH - playerPaddle.GetWidth(), playerPaddle.GetPosition().y });


    // Ball Update and Collisions
    for (int i = balls.size() - 1; i >= 0; --i)
    {
        Ball& ball = balls[i];
        if (!ball.active) continue;

        ball.Update(); // Update ball position

        // Ball vs Walls Collision
        if (ball.position.x - ball.radius <= 0 || ball.position.x + ball.radius >= WINDOW_WIDTH) {
            ball.speed.x *= -1.0f;
            if (ball.position.x - ball.radius <= 0) ball.position.x = ball.radius + 0.1f;
            if (ball.position.x + ball.radius >= WINDOW_WIDTH) ball.position.x = WINDOW_WIDTH - ball.radius - 0.1f;
        }
        if (ball.position.y - ball.radius <= 0) {
            ball.speed.y *= -1.0f;
            ball.position.y = ball.radius + 0.1f;
        }

        // Ball vs Bottom Edge
        if (ball.position.y + ball.radius >= WINDOW_HEIGHT) {
            ball.active = false;
            // Don't set game over yet, wait until all balls are checked
        }

        // Ball vs Paddle Collision
        if (CheckCollisionCircleRec(ball.position, ball.radius, playerPaddle.GetPaddleRectangle()))
        {
            if (ball.speed.y > 0) { // Only bounce if moving downwards
                // Adjust Y position to prevent sinking
                ball.position.y = playerPaddle.GetPosition().y - ball.radius - 0.1f;

                float hitPos = ball.position.x - (playerPaddle.GetPosition().x + playerPaddle.GetWidth() / 2.0f);
                float normalizedHitPos = hitPos / (playerPaddle.GetWidth() / 2.0f);
                normalizedHitPos = fmaxf(-0.95f, fminf(0.95f, normalizedHitPos)); // Clamp influence

                ball.speed.x = MAX_BALL_SPEED_X * normalizedHitPos * PADDLE_BOUNCE_MULTIPLIER;

                // Maintain overall speed (approximately)
                float speedMagnitude = sqrtf(INITIAL_BALL_SPEED.x * INITIAL_BALL_SPEED.x + INITIAL_BALL_SPEED.y * INITIAL_BALL_SPEED.y);
                ball.speed.y = -sqrtf(fmaxf(1.0f, speedMagnitude * speedMagnitude - ball.speed.x * ball.speed.x)); // Ensure Y speed is reasonable

                PlaySfx(fxPaddleHit);
            }
        }

        // Ball vs Bricks Collision
        bool brickHit = false;
        for (int r = 0; r < BRICK_ROWS && !brickHit; ++r) {
            for (int c = 0; c < BRICK_COLUMNS && !brickHit; ++c) {
                if (!bricks[r][c].IsDestroyed()) {
                    if (CheckCollisionCircleRec(ball.position, ball.radius, bricks[r][c].GetRect())) {

                        Vector2 brickCenter = { bricks[r][c].position.x + bricks[r][c].size.x / 2, bricks[r][c].position.y + bricks[r][c].size.y / 2 };
                        Color brickColor = bricks[r][c].color;

                        // Determine text color randomly
                        Color textColor;
                        int randColor = GetRandomValue(0, 4);
                        switch (randColor) {
                        case 0: textColor = GOLD; break; case 1: textColor = PURPLE; break;
                        case 2: textColor = GREEN; break; case 3: textColor = BLUE; break;
                        default: textColor = DARKBROWN; break;
                        }

                        // Trigger Background Flash
                        currentBackgroundColor = ColorBrightness(textColor, -0.5f); // Flash slightly darker than text
                        backgroundFlashTimer = FLASH_DURATION;

                        // Determine hit text randomly
                        std::string hitText;
                        int randText = GetRandomValue(0, 5);
                        switch (randText) {
                        case 0: hitText = "+" + std::to_string(SCORE_PER_BRICK); break;
                        case 1: hitText = "POP!"; break; case 2: hitText = "BAM!!!!!"; break;
                        case 3: hitText = "CRACK!!!!!"; break; case 4: hitText = "SMASH!"; break; // Shortened examples
                        case 5: hitText = "GREAT!"; break;
                        default: hitText = "+" + std::to_string(SCORE_PER_BRICK); break;
                        }

                        SpawnTextEffect(brickCenter, hitText, textColor, 40, { (float)GetRandomValue(-20, 20), -50.0f }, 0.85f);

                        PlaySfx(fxBrickHit);
                        bricks[r][c].Hit(); // Damage the brick

                        if (bricks[r][c].IsDestroyed()) {
                            score += SCORE_PER_BRICK;
                            activeBricksCount--;
                            if (GetRandomValue(1, 100) <= MODIFIER_CHANCE) {
                                SpawnModifier(brickCenter);
                            }
                        }

                        // Accurate Bounce Logic
                        Rectangle brickRect = bricks[r][c].GetRect();
                        float overlapX = (ball.radius + brickRect.width / 2) - fabsf(ball.position.x - (brickRect.x + brickRect.width / 2));
                        float overlapY = (ball.radius + brickRect.height / 2) - fabsf(ball.position.y - (brickRect.y + brickRect.height / 2));

                        bool verticalCollision = overlapY < overlapX;
                        // Tie-breaking for corner hits (optional refinement)
                        if (fabsf(overlapX - overlapY) < 1.0f) { // If overlaps are very close, consider velocity direction
                            verticalCollision = fabsf(ball.speed.y) > fabsf(ball.speed.x);
                        }


                        if (verticalCollision) {
                            ball.speed.y *= -1;
                            // Nudge ball out vertically
                            ball.position.y += (ball.speed.y > 0 ? overlapY : -overlapY) * 0.51f;
                        }
                        else {
                            ball.speed.x *= -1;
                            // Nudge ball out horizontally
                            ball.position.x += (ball.speed.x > 0 ? overlapX : -overlapX) * 0.51f;
                        }


                        brickHit = true; // Prevent multiple brick hits per ball per frame
                    }
                }
            }
        }
    } // End ball loop

    // Modifier Update and Collisions
    for (int i = activeModifiers.size() - 1; i >= 0; --i) {
        Modifier& mod = activeModifiers[i];
        if (!mod.active) continue;

        mod.Update();

        if (mod.active && CheckCollisionRecs(mod.GetRect(), playerPaddle.GetPaddleRectangle())) {
            ActivateModifier(mod);
            mod.active = false;
            PlaySfx(fxPowerup);
        }
    }

    // Update Text Effects
    for (int i = activeTextEffects.size() - 1; i >= 0; --i) {
        activeTextEffects[i].Update();
    }

    // Cleanup Inactive Objects
    balls.erase(std::remove_if(balls.begin(), balls.end(), [](const Ball& b) { return !b.active; }), balls.end());
    activeModifiers.erase(std::remove_if(activeModifiers.begin(), activeModifiers.end(), [](const Modifier& m) { return !m.active; }), activeModifiers.end());
    activeTextEffects.erase(std::remove_if(activeTextEffects.begin(), activeTextEffects.end(), [](const FloatingText& t) { return !t.active; }), activeTextEffects.end());

    // Check Game Over Condition (No active balls left)
    if (balls.empty() && currentGameState == PLAYING) {
        currentGameState = GAME_OVER;
        return; // Exit UpdateGame early if game is over
    }

    // Check Win Condition (No active bricks left) -> Reset Level
    if (activeBricksCount <= 0 && currentGameState == PLAYING) {
        ResetBricks(); // Reset bricks for a new level

        // Reset ball position and speed (using the first ball if multiple exist)
        if (!balls.empty()) {
            balls[0].position = { WINDOW_WIDTH / 2.0f, playerPaddle.GetPosition().y - PADDLE_H - BALL_RADIUS - 5 };
            balls[0].speed = INITIAL_BALL_SPEED; // Reset speed
            // Make sure the first ball is active if somehow it wasn't
            balls[0].active = true;
            // Remove any other extra balls from multiball etc.
            balls.resize(1);
        }
        else {
            // This case shouldn't happen if gameover check is correct, but just in case:
            InitGame(); // Full reset if no balls left somehow when bricks are cleared
        }
    }
}

// Draw the Game Screen (PLAYING state)
void DrawGame() {
    BeginDrawing();
    ClearBackground(currentBackgroundColor); // Use dynamic background color

    // Draw Bricks
    for (int r = 0; r < BRICK_ROWS; ++r) {
        for (int c = 0; c < BRICK_COLUMNS; ++c) {
            bricks[r][c].Draw();
        }
    }

    // Draw Paddle
    playerPaddle.Draw();

    // Draw Ball(s)
    for (const auto& ball : balls) {
        ball.Draw();
    }

    // Draw Modifiers
    for (const auto& mod : activeModifiers) {
        mod.Draw();
    }

    // Draw Text Effects
    for (const auto& textEffect : activeTextEffects) {
        textEffect.Draw();
    }

    // Draw UI
    DrawTextEx(gameFont, TextFormat("Score: %i", score), { 10, 10 }, 30, 2, GOLD);
    DrawTextEx(gameFont, TextFormat("Time: %.1f", gameTimer), { WINDOW_WIDTH - 150.0f, 10 }, 30, 2, WHITE);

    EndDrawing();
}

// Spawn a floating text effect
void SpawnTextEffect(Vector2 position, const std::string& text, Color color, int fontSize, Vector2 velocity, float lifeTime) {
    FloatingText newTextEffect;
    // Pass the address of the global gameFont
    newTextEffect.Init(&gameFont, position, velocity, text, color, fontSize, lifeTime);
    activeTextEffects.push_back(newTextEffect);
}

// Spawn a Modifier
void SpawnModifier(Vector2 position) {
    Modifier newMod;
    int randType = GetRandomValue(0, 1); // Only two types currently
    ModifierType type = MOD_NONE;
    switch (randType) {
    case 0: type = MOD_MULTIBALL; break;
    case 1: type = MOD_SCORE_BONUS; break;
    }

    if (type != MOD_NONE) {
        newMod.Init(position, type);
        activeModifiers.push_back(newMod);
    }
}

// Activate Modifier Effect
void ActivateModifier(Modifier& mod) {
    switch (mod.type) {
    case MOD_MULTIBALL:
    {
        int ballsToSpawn = 4; // Spawn two extra balls
        Vector2 spawnPos = mod.position; // Spawn near where modifier was collected

        for (int i = 0; i < ballsToSpawn && balls.size() < 10; ++i) { // Limit max balls
            Ball extraBall;
            // Give new ball slightly random upward velocity from paddle
            Vector2 newSpeed = {
                 INITIAL_BALL_SPEED.x * ((float)GetRandomValue(5, 15) / 10.0f) * (GetRandomValue(0,1) == 0 ? 1.0f : -1.0f) , // Random horizontal component
                -fabs(INITIAL_BALL_SPEED.y) * ((float)GetRandomValue(8, 12) / 10.0f) // Random upward vertical
            };

            extraBall.Init(spawnPos, newSpeed, BALL_RADIUS, SKYBLUE);
            balls.push_back(extraBall);
        }
        SpawnTextEffect(mod.position, "MULTI!", BLUE, 42, { 0.0f, -60.0f }, 1.0f);
    }
    break;
    case MOD_SCORE_BONUS:
        score += 99999;
        SpawnTextEffect(mod.position, "+99999!", GOLD, 42, { 0.0f, -60.0f }, 1.0f);
        break;
    case MOD_NONE:
        break; // Should not happen if SpawnModifier works correctly
    }
}

// Play Sound Effect Safely
void PlaySfx(Sound& sfx) {
    if (sfx.stream.buffer != nullptr) { // Check if sound is loaded
        PlaySound(sfx);
    }
}