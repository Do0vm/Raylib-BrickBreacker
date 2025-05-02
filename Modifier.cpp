#include "Modifier.h"
#include "raylib.h" // For GetFrameTime, DrawCircleV, DrawRectangleV
#include "Constants.h" // Include again for constants if needed inside methods

// Default constructor implementation
Modifier::Modifier() : position{ 0,0 }, velocity{ 0,0 }, type(MOD_NONE), active(false), color(WHITE), size(MODIFIER_SIZE) {}

void Modifier::Init(Vector2 pos, ModifierType t) {
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

void Modifier::Update() {
    if (active) {
        position.y += velocity.y * GetFrameTime();
        // Deactivate if off-screen
        if (position.y > WINDOW_HEIGHT + size) {
            active = false;
        }
    }
}

void Modifier::Draw() const {
    if (active) {
        // Draw based on type
        switch (type) {
        case MOD_MULTIBALL: DrawCircleV(position, size / 2.0f, color); break;
        case MOD_SCORE_BONUS: DrawRectangleV({ position.x - size / 2, position.y - size / 2 }, { size, size }, color); break;
        default: DrawCircleV(position, size / 2.0f, color); break; // Default draw as circle
        }
    }
}

Rectangle Modifier::GetRect() const {
    // Return the bounding box for collision detection
    return { position.x - size / 2, position.y - size / 2, size, size };
}