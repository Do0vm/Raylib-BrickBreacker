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
// Defines and Constants
//------------------------------------------------------------------------------------
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 900;

// Paddle Constants
const float PADDLE_W = 150.0f;
const float PADDLE_H = 10.0f;
const float PADDLE_SPEED = 16.0f;
const float PADDLE_DECELERATION = 0.85f;
const float PADDLE_BOUNCE_MULTIPLIER = 0.8f; // Multiplier for paddle bounce angle

// Ball Constants
const float BALL_RADIUS = 10.0f;
const Vector2 INITIAL_BALL_SPEED = { 800.0f, -800.0f };
const float MAX_BALL_SPEED_X = 900.0f; 

// Brick Constants
const int BRICK_ROWS = 5;
const int BRICK_COLUMNS = 10;
const float BRICK_HEIGHT = 20.0f;
const float BRICK_GAP = 2.0f;
const float BRICK_TOP_OFFSET = 50.0f; 
const float BRICK_WIDTH = (WINDOW_WIDTH - (BRICK_COLUMNS + 1) * BRICK_GAP) / BRICK_COLUMNS;
const int SCORE_PER_BRICK = 10;

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

//------------------------------------------------------------------------------------
// Global Variables (Forward Declarations if necessary)
//------------------------------------------------------------------------------------
Font gameFont;

//------------------------------------------------------------------------------------
// Structs and Classes
//------------------------------------------------------------------------------------

// Brick Class
class Brick {
public:
    Vector2 position;       
    Vector2 size;           
    int row, col;          
    int lives;              
    Color color;           

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

    void Draw() const {
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

// Modifier Type Enum and Struct
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
        velocity = { 0.0f, MODIFIER_SPEED }; // Modifiers fall downwards
        type = t;
        active = true;
        size = MODIFIER_SIZE;
        // Set color based on type
        switch (type) {
        case MOD_MULTIBALL: color = BLUE; break;
        case MOD_SCORE_BONUS: color = GOLD; break;
        default: color = WHITE; break;
        }
    }

    void Update() {
        if (active) {
            position.y += velocity.y * GetFrameTime(); 
            // Deactivate if off-screen
            if (position.y > WINDOW_HEIGHT + size) {
                active = false;
            }
        }
    }

    void Draw() const {
        if (active) {
            // Draw based on type
            switch (type) {
            case MOD_MULTIBALL: DrawCircleV(position, size / 2.0f, color); break;
            case MOD_SCORE_BONUS: DrawRectangleV({ position.x - size / 2, position.y - size / 2 }, { size, size }, color); break;
            default: DrawCircleV(position, size / 2.0f, color); break; // Default draw as circle
            }
        }
    }

    Rectangle GetRect() const {
        // Return the bounding box for collision detection
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
            // Update position based on speed and frame time
            position.x += speed.x * GetFrameTime();
            position.y += speed.y * GetFrameTime();
        }
    }

    void Draw() const {
        if (active) {
            DrawCircleV(position, radius, color);
        }
    }
};

// Structure for temporary text effects
struct FloatingText {
    Vector2 position;
    Vector2 velocity;
    std::string text;
    Color color;
    int fontSize;
    float lifeTime; 
    float initialLifeTime; 
    bool active;

    FloatingText() : position{ 0,0 }, velocity{ 0,0 }, text(""), color(WHITE), fontSize(20), lifeTime(0), initialLifeTime(1.0f), active(false) {}

    void Init(Vector2 pos, Vector2 vel, std::string txt, Color col, int size, float life) {
        position = pos;
        velocity = vel;
        text = txt;
        color = col;
        fontSize = size;
        lifeTime = life;
        initialLifeTime = life; 
        active = true;
    }

    void Update() {
        if (active) {
            lifeTime -= GetFrameTime(); // Decrease lifetime based on frame time
            if (lifeTime <= 0) {
                active = false; // Deactivate when time runs out
            }
            else {
                position.x += velocity.x * GetFrameTime();
                position.y += velocity.y * GetFrameTime();
            }
        }
    }

    void Draw() const {
        if (active) {
            float alpha = (lifeTime / initialLifeTime); // 1.0 when full, 0.0 when expired
            alpha = fmaxf(0.0f, fminf(1.0f, alpha)); // Clamp alpha between 0 and 1

           
            DrawTextEx(gameFont, text.c_str(), position, (float)fontSize, 1, Fade(color, alpha));
        }
    }
};

//------------------------------------------------------------------------------------
// Global Variables (Game State)
//------------------------------------------------------------------------------------
// Font gameFont; 

GameState currentGameState = START_SCREEN;
Paddle playerPaddle;
std::vector<Ball> balls;
Brick bricks[BRICK_ROWS][BRICK_COLUMNS];
std::vector<Modifier> activeModifiers;

// ADDED: Variables for effects
std::vector<FloatingText> activeTextEffects; // List to hold active text popups
Color currentBackgroundColor = DARKGRAY;     
const Color NORMAL_BG_COLOR = DARKGRAY;      
const Color FLASH_BG_COLOR = RED;       
const float FLASH_DURATION = 0.1f;           // How long the flash lasts in seconds
float backgroundFlashTimer = 0.0f;           // Timer for the flash effect

int score = 0;
int highScore = 0;
int activeBricksCount = 0;
float gameTimer = 0.0f;

// Sound 
 Sound fxPaddleHit;
 Sound fxBrickHit;
 Sound fxPowerup;

//------------------------------------------------------------------------------------
// Function Declarations
//------------------------------------------------------------------------------------
void InitGame();
void ResetBricks();
void UpdateGame();
void DrawGame();
void UpdateDrawFrame();
void SpawnModifier(Vector2 position);
//spawning text effects
void SpawnTextEffect(Vector2 position, const std::string& text, Color color, int fontSize, Vector2 velocity, float lifeTime);
void ActivateModifier(Modifier& mod);
 void PlaySfx(Sound& sfx);

//------------------------------------------------------------------------------------
// Main entry point
//------------------------------------------------------------------------------------
int main() {
    // Initialization
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Advanced Brick Breaker - GREGlib");
     InitAudioDevice(); 
    SetTargetFPS(60);       
    srand(time(NULL));      // Seed the random number generator

    gameFont = LoadFont("resources/fonts/alagard.png"); 
    if (gameFont.texture.id == 0) { 
        std::cerr << "Warning: Failed to load font 'resources/fonts/alagard.png'. Using default font." << std::endl;
        gameFont = GetFontDefault(); // Use default font as fallback
    }

    
     fxPaddleHit = LoadSound("resources/sounds/paddle_hit.wav");
     fxBrickHit = LoadSound("resources/sounds/brick_hit.wav");
     fxPowerup = LoadSound("resources/sounds/powerup.wav");

    InitGame(); // Initialize the first game state

    // Main game loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key
        UpdateDrawFrame(); // Update and draw one frame
    }

    // De-Initialization
     UnloadSound(fxPaddleHit);
     UnloadSound(fxBrickHit);
     UnloadSound(fxPowerup);

    UnloadFont(gameFont); 
     CloseAudioDevice(); 
    CloseWindow();        

    return 0;
}

//------------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------------

// Initialize/Reset Game State
void InitGame() {
    // Reset game variables
    score = 0;
    gameTimer = 0.0f;
    // activeBricksCount = 0; // This will be set by ResetBricks now
    balls.clear();           // Remove all existing balls
    activeModifiers.clear(); // Remove all existing modifiers
    activeTextEffects.clear(); // Remove all existing text effects
    currentBackgroundColor = NORMAL_BG_COLOR; // Reset background color
    backgroundFlashTimer = 0.0f;

    // Initialize Paddle
    playerPaddle.Init(
        { (WINDOW_WIDTH / 2.0f) - (PADDLE_W / 2.0f), WINDOW_HEIGHT * 0.9f },
        { 0.0f, 0.0f },   // Initial speed (zero)
        PADDLE_W,
        PADDLE_H,
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
    ResetBricks();

    
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
        // Draw centered text using MeasureTextEx for centering
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
            currentGameState = START_SCREEN; // Go back to start screen
        }
        // Draw Game Over Screen elements
        BeginDrawing();
        ClearBackground(BLACK);
        // Draw centered text
        DrawTextEx(gameFont, "GAME OVER", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "GAME OVER", 70, 2).x / 2, WINDOW_HEIGHT / 4.0f }, 70, 2, RED);
        DrawTextEx(gameFont, TextFormat("Final Score: %i", score), { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, TextFormat("Final Score: %i", score), 40, 2).x / 2, WINDOW_HEIGHT / 2.0f }, 40, 2, WHITE);
        DrawTextEx(gameFont, TextFormat("Time: %.2f s", gameTimer), { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, TextFormat("Time: %.2f s", gameTimer), 30, 2).x / 2, WINDOW_HEIGHT * 0.6f }, 30, 2, LIGHTGRAY);
        DrawTextEx(gameFont, TextFormat("High Score: %i", highScore), { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, TextFormat("High Score: %i", highScore), 30, 2).x / 2, WINDOW_HEIGHT * 0.68f }, 30, 2, GOLD);
        DrawTextEx(gameFont, "Press [R] to Restart", { WINDOW_WIDTH / 2.0f - MeasureTextEx(gameFont, "Press [R] to Restart", 30, 2).x / 2, WINDOW_HEIGHT * 0.8f }, 30, 2, YELLOW);
        EndDrawing();
        break;
    }
}
//------------------------------------------------------------------------------------
// Function to Reset/Initialize Bricks
//------------------------------------------------------------------------------------
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
            // Else lives remains 1

            bricks[r][c].Init(brickPos, brickSize, r, c, lives);

            // Count active bricks again
            if (bricks[r][c].lives > 0) {
                activeBricksCount++;
            }
        }
    }

    // Give the player a bonus for clearing the screen
    score += 250; // Example bonus
    SpawnTextEffect({ WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 3.0f }, "CLEARED! +250", GOLD, 35, { 0, -40 }, 1.5f);

    // Maybe reset ball speed slightly or add a small delay?
    // For now, we just reset the bricks.
}

// Update Game Logic for PLAYING state
void UpdateGame() {
    float dt = GetFrameTime(); 
    gameTimer += dt;

    // --- Update Background Flash --- ADDED ---
    if (backgroundFlashTimer > 0.0f) {
        backgroundFlashTimer -= dt; // Decrease timer
        if (backgroundFlashTimer <= 0.0f) {
            currentBackgroundColor = NORMAL_BG_COLOR; // Reset background color when timer expires
        }
    }
    // --- End Background Flash Update ---

    // --- Paddle Input and Movement ---
    // Set speed based on key presses
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        playerPaddle.SetSpeed(Vector2{ PADDLE_SPEED, 0.0f });
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        playerPaddle.SetSpeed(Vector2{ -PADDLE_SPEED, 0.0f });
    else 
        playerPaddle.SetSpeed(Vector2{ playerPaddle.GetSpeed().x * PADDLE_DECELERATION, 0.0f });

    playerPaddle.Update(); // Update paddle position based on its speed (uses its own internal logic)

    // --- Paddle Screen Bounds ---
    if (playerPaddle.GetPosition().x <= 0)
        playerPaddle.SetPosition(Vector2{ 0.0f, playerPaddle.GetPosition().y });
    else if (playerPaddle.GetPosition().x + playerPaddle.GetWidth() >= WINDOW_WIDTH)
        playerPaddle.SetPosition(Vector2{ WINDOW_WIDTH - playerPaddle.GetWidth(), playerPaddle.GetPosition().y });


    // --- Ball Update and Collisions ---
    // Iterate backwards for safe removal during iteration
    for (int i = balls.size() - 1; i >= 0; --i)
    {
        Ball& ball = balls[i]; 
        if (!ball.active) continue; // Skip inactive balls

        ball.Update(); // Update ball position

        // Ball vs Walls Collision
        // Left/Right walls
        if (ball.position.x - ball.radius <= 0 || ball.position.x + ball.radius >= WINDOW_WIDTH) {
            ball.speed.x *= -1.0f; // Reverse horizontal speed
            // Nudge ball slightly away from wall to prevent sticking
            if (ball.position.x - ball.radius <= 0) ball.position.x = ball.radius + 0.1f;
            if (ball.position.x + ball.radius >= WINDOW_WIDTH) ball.position.x = WINDOW_WIDTH - ball.radius - 0.1f;
        }
        // Top wall
        if (ball.position.y - ball.radius <= 0) {
            ball.speed.y *= -1.0f; // Reverse vertical speed
            ball.position.y = ball.radius + 0.1f; // Nudge away from top
        }

        // Ball vs Bottom Edge (Game Over Condition for this ball)
        if (ball.position.y + ball.radius >= WINDOW_HEIGHT) {
            ball.active = false; // Deactivate the ball
            // No immediate game over here; check if *all* balls are inactive later
        }

        // Ball vs Paddle Collision
        if (CheckCollisionCircleRec(ball.position, ball.radius, playerPaddle.GetPaddleRectangle()))
        {
            // Only bounce if ball is moving downwards
            if (ball.speed.y > 0) {
                ball.position.y = playerPaddle.GetPosition().y - ball.radius;

                float hitPos = ball.position.x - (playerPaddle.GetPosition().x + playerPaddle.GetWidth() / 2.0f);
                float normalizedHitPos = hitPos / (playerPaddle.GetWidth() / 2.0f);
                normalizedHitPos = fmaxf(-0.95f, fminf(0.95f, normalizedHitPos));

                ball.speed.x = MAX_BALL_SPEED_X * normalizedHitPos * PADDLE_BOUNCE_MULTIPLIER;

                float speedMagnitude = sqrtf(INITIAL_BALL_SPEED.x * INITIAL_BALL_SPEED.x + INITIAL_BALL_SPEED.y * INITIAL_BALL_SPEED.y);
                ball.speed.y = -sqrtf(fmaxf(1.0f, speedMagnitude * speedMagnitude - ball.speed.x * ball.speed.x));

                 PlaySfx(fxPaddleHit); 
            }
        }

        // Ball vs Bricks Collision
        bool brickHit = false; 
        for (int r = 0; r < BRICK_ROWS && !brickHit; ++r) {
            for (int c = 0; c < BRICK_COLUMNS && !brickHit; ++c) {
                if (!bricks[r][c].IsDestroyed()) {
                    if (CheckCollisionCircleRec(ball.position, ball.radius, bricks[r][c].GetRect())) {

                        // --- Trigger Effects on Hit --- ADDED ---
                        Vector2 brickCenter = { bricks[r][c].position.x + bricks[r][c].size.x / 2, bricks[r][c].position.y + bricks[r][c].size.y / 2 };
                        Color brickColor = bricks[r][c].color; // Get brick color

                        Color textColor;
                        int randColor = GetRandomValue(0, 4);
                        switch (randColor) {
                        case 0: textColor = GOLD; break;
                        case 1: textColor = PURPLE; break;
                        case 2: textColor = GREEN; break;
                        case 3: textColor = BLUE; break;
                        default: textColor = DARKBROWN; break;
                        }


                        // Trigger Background Flash
                        currentBackgroundColor = ColorBrightness(textColor,-0.5f);
                        backgroundFlashTimer = FLASH_DURATION; // Reset flash timer

                        std::string hitText;
                        int randText = GetRandomValue(0, 5);
                        switch (randText) {
                        case 0: hitText = "+" + std::to_string(SCORE_PER_BRICK); break;
                        case 1: hitText = "POP!"; break;
                        case 2: hitText = "BAM!!!!!"; break;
                        case 3: hitText = "CRACK!!!!!"; break;
                        case 4: hitText = "UBISOFT HE IS AVAILABLE!!!"; break;
                        case 5: hitText = "GREGORY.DEARHAM@LINKEDIN"; break;
                        default: hitText = "+" + std::to_string(SCORE_PER_BRICK); break;


                        }

                        SpawnTextEffect(brickCenter,       // Position (brick center)
                            hitText,          
                            textColor,            
                            40,               
                            { (float)GetRandomValue(-20, 20), -50.0f }, 
                            0.85f);            
                        // --- End Trigger Effects ---

                         PlaySfx(fxBrickHit); 
                        bricks[r][c].Hit(); // Damage the brick

                        // Check if brick was destroyed by the hit
                        if (bricks[r][c].IsDestroyed()) {
                            score += SCORE_PER_BRICK; // Increase score
                            activeBricksCount--;      // Decrease active brick count
                            // Chance to spawn a modifier
                            if (GetRandomValue(1, 100) <= MODIFIER_CHANCE) {
                                SpawnModifier(brickCenter); // Spawn modifier at brick center
                            }
                        }

                        // Accurate Bounce Logic
                        Rectangle brickRect = bricks[r][c].GetRect();
                        float pointX = ball.position.x;
                        float pointY = ball.position.y;
                        if (pointX < brickRect.x) pointX = brickRect.x;
                        else if (pointX > brickRect.x + brickRect.width) pointX = brickRect.x + brickRect.width;
                        if (pointY < brickRect.y) pointY = brickRect.y;
                        else if (pointY > brickRect.y + brickRect.height) pointY = brickRect.y + brickRect.height;

                        // Vector from closest point to ball center
                        Vector2 delta = { ball.position.x - pointX, ball.position.y - pointY };
                      
                        float deltaLenSq = delta.x * delta.x + delta.y * delta.y;
       

                        // Calculate overlap on each axis (relative to centers)
                        float overlapX = (ball.radius + bricks[r][c].size.x / 2) - fabsf(ball.position.x - (bricks[r][c].position.x + bricks[r][c].size.x / 2));
                        float overlapY = (ball.radius + bricks[r][c].size.y / 2) - fabsf(ball.position.y - (bricks[r][c].position.y + bricks[r][c].size.y / 2));

                        // Determine primary collision axis based on smaller overlap
                        bool verticalCollision = overlapY < overlapX;
                        if (fabsf(overlapX - overlapY) < 0.1f) { 
                            verticalCollision = fabsf(ball.speed.y) > fabsf(ball.speed.x);
                        }

                        // Reverse speed component based on collision axis
                        if (verticalCollision) { // Hit top or bottom
                            ball.speed.y *= -1;
                            ball.position.y += (ball.speed.y > 0 ? overlapY : -overlapY) * 0.51f; // Adjusted nudge slightly
                        }
                        else { // Hit sides
                            ball.speed.x *= -1;
                            // Nudge ball out horizontally
                            ball.position.x += (ball.speed.x > 0 ? overlapX : -overlapX) * 0.51f; // Adjusted nudge slightly
                        }

                        brickHit = true; // Mark that a brick was hit this frame
                    } // End CheckCollisionCircleRec
                } // End !IsDestroyed
            } // End column loop
        } // End row loop
    } // End ball loop

    // --- Modifier Update and Collisions ---
    for (int i = activeModifiers.size() - 1; i >= 0; --i) {
        Modifier& mod = activeModifiers[i];
        if (!mod.active) continue; 

        mod.Update();

        // Check collision between modifier and paddle
        if (mod.active && CheckCollisionRecs(mod.GetRect(), playerPaddle.GetPaddleRectangle())) {
            ActivateModifier(mod);
            mod.active = false;   
             PlaySfx(fxPowerup);
        }
    }

    // --- Update Text Effects --- ADDED ---
    // Iterate backwards for safe removal
    for (int i = activeTextEffects.size() - 1; i >= 0; --i) {
        activeTextEffects[i].Update(); // Update position and lifetime
    }
    // --- End Text Effects Update ---

    // --- Cleanup Inactive Objects --- (Consolidated cleanup after all updates)
    // Remove inactive balls
    balls.erase(std::remove_if(balls.begin(), balls.end(), [](const Ball& b) { return !b.active; }), balls.end());
    activeModifiers.erase(std::remove_if(activeModifiers.begin(), activeModifiers.end(), [](const Modifier& m) { return !m.active; }), activeModifiers.end());
    activeTextEffects.erase(std::remove_if(activeTextEffects.begin(), activeTextEffects.end(), [](const FloatingText& t) { return !t.active; }), activeTextEffects.end());

    // --- Check Game Over Condition (No active balls left) ---
    if (balls.empty() && currentGameState == PLAYING) {
        currentGameState = GAME_OVER;
        return; 
    }

    // --- Check Win Condition (No active bricks left) ---
    if (activeBricksCount <= 0 && currentGameState == PLAYING) {
        // currentGameState = GAME_OVER;
        ResetBricks();               

        
        if (!balls.empty()) {
             balls[0].position = { WINDOW_WIDTH / 2.0f, playerPaddle.GetPosition().y - PADDLE_H - BALL_RADIUS - 5 };
             balls[0].speed = INITIAL_BALL_SPEED; // Or maybe slightly faster?
        }
        
    }
}

void DrawGame() {
    BeginDrawing();
    // MODIFIED: Use current background color for flash effect
    ClearBackground(currentBackgroundColor);

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

    // --- Draw Text Effects --- ADDED ---
    for (const auto& textEffect : activeTextEffects) {
        textEffect.Draw();
    }
    // --- End Draw Text Effects ---

    // Draw UI (Score and Time)
    DrawTextEx(gameFont, TextFormat("Score: %i", score), { 10, 10 }, 30, 2, GOLD);
    DrawTextEx(gameFont, TextFormat("Time: %.1f", gameTimer), { WINDOW_WIDTH - 150.0f, 10 }, 30, 2, WHITE);

    EndDrawing();
}

// ADDED: Spawn a floating text effect
void SpawnTextEffect(Vector2 position, const std::string& text, Color color, int fontSize, Vector2 velocity, float lifeTime) {
    FloatingText newTextEffect;
    newTextEffect.Init(position, velocity, text, color, fontSize, lifeTime);
    activeTextEffects.push_back(newTextEffect); // Add to the list of active effects
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
        int ballsToSpawn = 3; // Spawn one extra ball

        for (int i = 0; i < ballsToSpawn && balls.size() < 10; ++i) { // Limit max balls
            Ball extraBall;
            Vector2 spawnPos = mod.position; // Spawn where modifier was collected
            // Give new ball slightly random upward velocity
            Vector2 newSpeed = { (float)GetRandomValue(-5, 5), -abs(INITIAL_BALL_SPEED.y) * ((float)GetRandomValue(8, 12) / 10.0f) };
            // Ensure it has some horizontal speed
            if (fabsf(newSpeed.x) < 1.5f) newSpeed.x = (newSpeed.x >= 0 ? 1.5f : -1.5f);
            // Ensure it has decent vertical speed upwards
            if (newSpeed.y > -3.0f) newSpeed.y = -3.0f;


            extraBall.Init(spawnPos, newSpeed, BALL_RADIUS, SKYBLUE);
            balls.push_back(extraBall);
        }
    }
    break;
    case MOD_SCORE_BONUS:
        score += 100;
        // Spawn a text effect for the score bonus
        SpawnTextEffect(mod.position, "+100!", GOLD, 24, { 0.0f, -60.0f }, 1.0f);
        break;
    case MOD_NONE:
        break;
    }
}


 void PlaySfx(Sound& sfx) {
     if (sfx.stream.buffer != nullptr) { // Check if sound is loaded
         PlaySound(sfx);
     }
 }