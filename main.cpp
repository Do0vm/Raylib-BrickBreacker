#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 
#include <ctime> 
#include <cstdlib> 
#include <algorithm> 
#include "Paddle.h" 

//------------------------------------------------------------------------------------
// Defines and Global Variables
//------------------------------------------------------------------------------------
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 900;

// Paddle Constants

const float PADDLE_W = 10.0f;
const float PADDLE_H = 150.0f; 
const float PADDLE_SPEED = 8.0f; 
const float PADDLE_DECELERATION = 0.95f; 
const float PADDLE_BOUNCE_MULTIPLIER = 1.5f; // Multiplier for paddle bounce angle

// Ball Constants
const float BALL_RADIUS = 10.0f;
const Vector2 INITIAL_BALL_SPEED = { 10.0f, -10.0f };
const float MAX_BALL_SPEED_X = 8.0f; // Max horizontal speed after paddle hit

// Brick Constants
const int BRICK_ROWS = 5;
const int BRICK_COLUMNS = 10;
const float BRICK_HEIGHT = 20.0f;
const float BRICK_GAP = 2.0f;
const float BRICK_TOP_OFFSET = 50.0f; // Space above the bricks
const float BRICK_WIDTH = (WINDOW_WIDTH - (BRICK_COLUMNS + 1) * BRICK_GAP) / BRICK_COLUMNS;
const int SCORE_PER_BRICK = 10;

// Modifier Constants
const float MODIFIER_CHANCE = 15.0f; // Percentage chance (0-100) to drop modifier
const float MODIFIER_SPEED = 3.0f;
const float MODIFIER_SIZE = 15.0f;

// Game State
typedef enum {
    START_SCREEN,
    PLAYING,
    GAME_OVER
} GameState;

//------------------------------------------------------------------------------------
// Structs and Classes
//------------------------------------------------------------------------------------

// Brick Class (No changes needed here)
class Brick {
public:
    Vector2 position;       // Top-left corner
    Vector2 size;           // Width and height
    int row, col;           // Grid coordinates
    int lives;              // Use lives count (0 means destroyed)
    Color color;            // Brick color

    Brick() : position{ 0, 0 }, size{ 0, 0 }, row(0), col(0), lives(0), color(BLANK) {}

    void Init(Vector2 pos, Vector2 sz, int r, int c, int lvs) {
        position = pos;
        size = sz;
        row = r;
        col = c;
        lives = lvs;
        if (lives == 3) color = MAROON;
        else if (lives == 2) color = RED;
        else color = ORANGE;
    }

    void Draw() const { // Added const
        if (lives > 0) {
            DrawRectangleV(position, size, color);
        }
    }

    Rectangle GetRect() const {
        return { position.x, position.y, size.x, size.y };
    }

    bool IsDestroyed() const {
        return lives <= 0;
    }

    void Hit() {
        if (lives > 0) {
            lives--;
            if (lives == 2) color = RED;
            else if (lives == 1) color = ORANGE;
        }
    }
};

// Modifier Type Enum and Struct (No changes needed here)
typedef enum {
    MOD_NONE,
    MOD_MULTIBALL,
    MOD_SCORE_BONUS
} ModifierType;

class Modifier {
public:
    Vector2 position;
    Vector2 velocity;
    ModifierType type;
    bool active;
    Color color;
    float size;

    Modifier() : position{ 0,0 }, velocity{ 0,0 }, type(MOD_NONE), active(false), color(WHITE), size(MODIFIER_SIZE) {}

    void Init(Vector2 pos, ModifierType t) {
        position = pos;
        velocity = { 0.0f, MODIFIER_SPEED };
        type = t;
        active = true;
        size = MODIFIER_SIZE;
        switch (type) {
        case MOD_MULTIBALL: color = BLUE; break;
        case MOD_SCORE_BONUS: color = GOLD; break;
        default: color = WHITE; break;
        }
    }

    void Update() {
        if (active) {
            position.y += velocity.y;
            if (position.y > WINDOW_HEIGHT + size) { // Check if fully off screen
                active = false;
            }
        }
    }

    void Draw() const { // Added const
        if (active) {
            switch (type) {
            case MOD_MULTIBALL: DrawCircleV(position, size / 2.0f, color); break;
            case MOD_SCORE_BONUS: DrawRectangleV({ position.x - size / 2, position.y - size / 2 }, { size, size }, color); break;
            default: DrawCircleV(position, size / 2.0f, color); break;
            }
        }
    }

    Rectangle GetRect() const {
        return { position.x - size / 2, position.y - size / 2, size, size };
    }
};


// Ball Struct
struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
    Color color;

    
    void Init(Vector2 pos, Vector2 spd, float rad, Color col = WHITE) {
        position = pos;
        speed = spd;
        radius = rad;
        active = true; // Ensure ball starts active
        color = col;
    }

    void Update() {
        if (active) {
            position.x += speed.x;
            position.y += speed.y;
        }
    }

    void Draw() const { 
        if (active) {
            DrawCircleV(position, radius, color);
        }
    }
};


//------------------------------------------------------------------------------------
// Global Variables (Game State)
//------------------------------------------------------------------------------------
GameState currentGameState = START_SCREEN;
Paddle playerPaddle;
std::vector<Ball> balls;
Brick bricks[BRICK_ROWS][BRICK_COLUMNS];
std::vector<Modifier> activeModifiers;

int score = 0;
int highScore = 0;
int activeBricksCount = 0;
float gameTimer = 0.0f;
Font gameFont;

// Sound
// Sound fxPaddleHit;
// Sound fxBrickHit;
// Sound fxPowerup;

//------------------------------------------------------------------------------------
// Function Declarations
//------------------------------------------------------------------------------------
void InitGame();
void UpdateGame();
void DrawGame();
void UpdateDrawFrame();
void SpawnModifier(Vector2 position);
void ActivateModifier(Modifier& mod);
void PlaySfx(Sound& sfx); 

//------------------------------------------------------------------------------------
// Main entry point
//------------------------------------------------------------------------------------
int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Advanced Brick Breaker - raylib");
    // InitAudioDevice(); // Uncomment if using sound
    SetTargetFPS(60);
    srand(time(NULL));

    gameFont = LoadFont("resources/fonts/alagard.png");
    if (gameFont.texture.id == 0) {
        std::cerr << "Warning: Failed to load font alagard.png. Using default font." << std::endl;
        gameFont = GetFontDefault();
    }

    // Load Sounds 
    // fxPaddleHit = LoadSound("resources/sounds/paddle_hit.wav");
    // fxBrickHit = LoadSound("resources/sounds/brick_hit.wav");
    // fxPowerup = LoadSound("resources/sounds/powerup.wav");


    InitGame(); // Initialize first game state

    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }

    // Unload Sounds 
    // UnloadSound(fxPaddleHit);
    // UnloadSound(fxBrickHit);
    // UnloadSound(fxPowerup);

    UnloadFont(gameFont);
    // CloseAudioDevice(); 
    CloseWindow();
    return 0;
}

//------------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------------

// Initialize/Reset Game State
void InitGame() {
    score = 0;
    gameTimer = 0.0f;
    activeBricksCount = 0;
    balls.clear();
    activeModifiers.clear();

    // Initialize Paddle
    playerPaddle.Init(
        { (WINDOW_WIDTH / 2.0f) - (PADDLE_W / 2.0f), WINDOW_HEIGHT * 0.9f }, // Position (Centered horizontally)
        { 0.0f, 0.0f },   // Initial speed
        PADDLE_H,         
        PADDLE_W,         
        WHITE);

    // Initialize Ball(s)
    Ball initialBall;
    initialBall.Init(
        { WINDOW_WIDTH / 2.0f, playerPaddle.GetPosition().y - PADDLE_H / 2.0f - BALL_RADIUS - 5 }, // Start slightly above paddle center
        INITIAL_BALL_SPEED,
        BALL_RADIUS,
        Color{ 2, 222, 233, 242 }
    );
    balls.push_back(initialBall);


    // Initialize Bricks
    for (int r = 0; r < BRICK_ROWS; ++r) {
        for (int c = 0; c < BRICK_COLUMNS; ++c) {
            Vector2 brickPos = {
                c * (BRICK_WIDTH + BRICK_GAP) + BRICK_GAP,
                r * (BRICK_HEIGHT + BRICK_GAP) + BRICK_GAP + BRICK_TOP_OFFSET
            };
            Vector2 brickSize = { BRICK_WIDTH, BRICK_HEIGHT };
            int lives = 1;
            if (r < 4) lives = 3;
            else if (r < 8) lives = 2;

            bricks[r][c].Init(brickPos, brickSize, r, c, lives);
            if (bricks[r][c].lives > 0) {
                activeBricksCount++;
            }
        }
    }
}

// Update and Draw Frame (Manages Game States)
void UpdateDrawFrame() {
    switch (currentGameState) {
    case START_SCREEN:
        if (IsKeyPressed(KEY_ENTER)) {
            InitGame();
            currentGameState = PLAYING;
        }
        // Draw Start Screen
        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawTextEx(gameFont, "BRICK BREAKER", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "BRICK BREAKER", 60, 2).x / 2, WINDOW_HEIGHT / 4.0f }, 60, 2, YELLOW);
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
            highScore = score;
        }
        if (IsKeyPressed(KEY_R)) {
            currentGameState = START_SCREEN;
        }
        // Draw Game Over Screen
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
    gameTimer += GetFrameTime();

    // --- Paddle Input and Movement ---
    
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        playerPaddle.SetSpeed(Vector2{ PADDLE_SPEED, 0.0f });
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        playerPaddle.SetSpeed(Vector2{ -PADDLE_SPEED, 0.0f });
    else 
        playerPaddle.SetSpeed(Vector2{ playerPaddle.GetSpeed().x * PADDLE_DECELERATION, 0.0f });

    playerPaddle.Update();

    // --- Paddle Screen Bounds ---
    if (playerPaddle.GetPosition().x <= 0)
        playerPaddle.SetPosition(Vector2{ 0.0f, playerPaddle.GetPosition().y });
    else if (playerPaddle.GetPosition().x + playerPaddle.GetWidth() >= WINDOW_WIDTH)
        playerPaddle.SetPosition(Vector2{ WINDOW_WIDTH - playerPaddle.GetWidth(), playerPaddle.GetPosition().y });


    // --- Ball Update and Collisions ---
   
    for (int i = balls.size() - 1; i >= 0; --i)
    {
        Ball& ball = balls[i]; // Use non-const reference to modify ball
        if (!ball.active) continue;

        ball.Update();

        // Ball vs Walls
        if (ball.position.x - ball.radius <= 0 || ball.position.x + ball.radius >= WINDOW_WIDTH) {
            ball.speed.x *= -1.0f;
            if (ball.position.x - ball.radius <= 0) ball.position.x = ball.radius + 0.1f; // Nudge out
            if (ball.position.x + ball.radius >= WINDOW_WIDTH) ball.position.x = WINDOW_WIDTH - ball.radius - 0.1f; // Nudge out
        }
        if (ball.position.y - ball.radius <= 0) {
            ball.speed.y *= -1.0f;
            ball.position.y = ball.radius + 0.1f; // Nudge out
        }

        // Ball vs Bottom Edge
        if (ball.position.y + ball.radius >= WINDOW_HEIGHT) {
            ball.active = false;
        }

        // Ball vs Paddle
        if (CheckCollisionCircleRec(ball.position, ball.radius, playerPaddle.GetPaddleRectangle()))
        {
            if (ball.speed.y > 0) {
                ball.position.y = playerPaddle.GetPosition().y - ball.radius;

                float hitPos = ball.position.x - (playerPaddle.GetPosition().x + playerPaddle.GetWidth() / 2.0f);
                float normalizedHitPos = hitPos / (playerPaddle.GetWidth() / 2.0f);
                normalizedHitPos = fmaxf(-0.95f, fminf(0.95f, normalizedHitPos));

                ball.speed.x = MAX_BALL_SPEED_X * normalizedHitPos * PADDLE_BOUNCE_MULTIPLIER;

                float speedMagnitude = sqrtf(INITIAL_BALL_SPEED.x * INITIAL_BALL_SPEED.x + INITIAL_BALL_SPEED.y * INITIAL_BALL_SPEED.y);
                ball.speed.y = -sqrtf(fmaxf(1.0f, speedMagnitude * speedMagnitude - ball.speed.x * ball.speed.x));

                // PlaySfx(fxPaddleHit); // Optional sound
            }
        }

        // Ball vs Bricks
        bool brickHit = false;
        for (int r = 0; r < BRICK_ROWS && !brickHit; ++r) {
            for (int c = 0; c < BRICK_COLUMNS; ++c) {
                if (!bricks[r][c].IsDestroyed()) {
                    if (CheckCollisionCircleRec(ball.position, ball.radius, bricks[r][c].GetRect())) {
                        // PlaySfx(fxBrickHit); // Optional sound
                        bricks[r][c].Hit();

                        if (bricks[r][c].IsDestroyed()) {
                            score += SCORE_PER_BRICK;
                            activeBricksCount--;
                            if (GetRandomValue(1, 100) <= MODIFIER_CHANCE) {
                                SpawnModifier({ bricks[r][c].position.x + bricks[r][c].size.x / 2, bricks[r][c].position.y + bricks[r][c].size.y / 2 });
                            }
                        }

                        // Bounce Logic
                        Rectangle brickRect = bricks[r][c].GetRect();
                        float pointX = ball.position.x;
                        float pointY = ball.position.y;

                        // Find closest point on brick rectangle to ball center
                        if (pointX < brickRect.x) pointX = brickRect.x;
                        else if (pointX > brickRect.x + brickRect.width) pointX = brickRect.x + brickRect.width;
                        if (pointY < brickRect.y) pointY = brickRect.y;
                        else if (pointY > brickRect.y + brickRect.height) pointY = brickRect.y + brickRect.height;

                        // Vector from closest point to ball center
                        Vector2 delta = { ball.position.x - pointX, ball.position.y - pointY };
                        float deltaLenSq = delta.x * delta.x + delta.y * delta.y;

                        // Check if really colliding (distance < radius)
                        if (deltaLenSq < ball.radius * ball.radius) {
                            float overlapX = (ball.radius + bricks[r][c].size.x / 2) - fabsf(ball.position.x - (bricks[r][c].position.x + bricks[r][c].size.x / 2));
                            float overlapY = (ball.radius + bricks[r][c].size.y / 2) - fabsf(ball.position.y - (bricks[r][c].position.y + bricks[r][c].size.y / 2));

                            // Determine primary collision axis based on entry direction and overlap
                            bool verticalCollision = overlapY < overlapX;
                            if (overlapY == overlapX) { // If overlaps are equal, use velocity direction
                                verticalCollision = fabsf(ball.speed.y) > fabsf(ball.speed.x);
                            }


                            if (verticalCollision) { // Hit top or bottom
                                ball.speed.y *= -1;
                                // Nudge ball out
                                ball.position.y += (ball.speed.y > 0 ? overlapY : -overlapY) * 1.01f; // Nudge based on overlap
                            }
                            else { // Hit sides
                                ball.speed.x *= -1;
                                // Nudge ball out
                                ball.position.x += (ball.speed.x > 0 ? overlapX : -overlapX) * 1.01f; // Nudge based on overlap
                            }
                            brickHit = true;
                        }
                        if (brickHit) break; 
                    }
                }
            } 
        } 
    } 

    // Remove inactive balls & Check Game Over Condition
    balls.erase(std::remove_if(balls.begin(), balls.end(), [](const Ball& b) { return !b.active; }), balls.end());
    if (balls.empty()) { // Check if the vector is empty after removal
        currentGameState = GAME_OVER;
        return; 
    }


    // --- Modifier Update and Collisions ---
    for (int i = activeModifiers.size() - 1; i >= 0; --i) {
        Modifier& mod = activeModifiers[i]; // Use non-const reference
        mod.Update();

        if (mod.active) {
            if (CheckCollisionRecs(mod.GetRect(), playerPaddle.GetPaddleRectangle())) {
                ActivateModifier(mod);
                mod.active = false;
                // PlaySfx(fxPowerup); // Optional sound
            }
        }
        // Remove inactive modifiers more robustly
        // if (!mod.active) {
        //    activeModifiers.erase(activeModifiers.begin() + i);
        // }
    }
    // Separate removal loop for inactive modifiers
    activeModifiers.erase(std::remove_if(activeModifiers.begin(), activeModifiers.end(),
        [](const Modifier& m) { return !m.active; }),
        activeModifiers.end());


    // --- Check Win Condition ---
    if (activeBricksCount <= 0 && currentGameState == PLAYING) { // Ensure not already Game Over
        currentGameState = GAME_OVER; // Or a WIN state
    }
}

// Draw Game Elements for PLAYING state
void DrawGame() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // Draw Bricks
    for (int r = 0; r < BRICK_ROWS; ++r) {
        for (int c = 0; c < BRICK_COLUMNS; ++c) {
            bricks[r][c].Draw(); // Draw is const
        }
    }

    // Draw Paddle
    playerPaddle.Draw(); // Assumes Paddle::Draw exists

    // Draw Ball(s)
    for (const auto& ball : balls) { // Use const ref here is fine
        ball.Draw(); // Ball::Draw is const
    }

    // Draw Modifiers
    for (const auto& mod : activeModifiers) { // Use const ref here is fine
        mod.Draw(); // Modifier::Draw should be const
    }

    // Draw UI
    DrawTextEx(gameFont, TextFormat("Score: %i", score), { 10, 10 }, 30, 2, GOLD);
    DrawTextEx(gameFont, TextFormat("Time: %.1f", gameTimer), { WINDOW_WIDTH - 150.0f, 10 }, 30, 2, WHITE);

    EndDrawing();
}

// Spawn a Modifier
void SpawnModifier(Vector2 position) {
    Modifier newMod;
    int randType = GetRandomValue(0, 1);
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
        int ballsToSpawn = 1;
        for (int i = 0; i < ballsToSpawn && balls.size() < 10; ++i) {
            Ball extraBall;
            Vector2 spawnPos = mod.position;
            Vector2 newSpeed = { (float)GetRandomValue(-4, 4), -abs(INITIAL_BALL_SPEED.y) * (float)GetRandomValue(8, 12) / 10.0f };
            if (abs(newSpeed.x) < 1.0f) newSpeed.x = (newSpeed.x >= 0 ? 1.0f : -1.0f); // Ensure minimum horizontal speed

            extraBall.Init(spawnPos, newSpeed, BALL_RADIUS, SKYBLUE);
            balls.push_back(extraBall);
        }
    }
    break;
    case MOD_SCORE_BONUS:
        score += 100;
        break;
    case MOD_NONE:
        break;
    }
}

// Helper to load/play sound once (Optional)
// void PlaySfx(Sound& sfx) {
//     if (sfx.stream.buffer != nullptr) { // Check if sound is loaded
//         PlaySound(sfx);
//     }
// }