#include "FloatingText.h"
#include "raylib.h" // For GetFrameTime, DrawTextEx, Fade, fmaxf, fminf
#include <cmath>    // For fmaxf, fminf

// Default constructor implementation
FloatingText::FloatingText() : position{ 0,0 }, velocity{ 0,0 }, text(""), color(WHITE), fontSize(20), lifeTime(0), initialLifeTime(1.0f), active(false), pGameFont(nullptr) {}

void FloatingText::Init(Font* font, Vector2 pos, Vector2 vel, std::string txt, Color col, int size, float life) {
    pGameFont = font; // Store the pointer to the font
    position = pos;
    velocity = vel;
    text = txt;
    color = col;
    fontSize = size;
    lifeTime = life;
    initialLifeTime = life;
    active = true;
}

void FloatingText::Update() {
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

void FloatingText::Draw() const {
    if (active && pGameFont && pGameFont->texture.id != 0) { // Check if font is valid
        float alpha = (lifeTime / initialLifeTime); // 1.0 when full, 0.0 when expired
        alpha = fmaxf(0.0f, fminf(1.0f, alpha)); // Clamp alpha between 0 and 1

        DrawTextEx(*pGameFont, text.c_str(), position, (float)fontSize, 1, Fade(color, alpha));
    }
    // else if (active) { DrawText(...) }
}