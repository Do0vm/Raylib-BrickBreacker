#include "Brick.h"
#include "raylib.h"

// Default constructor implementation (can be empty if members are initialized in declaration or Init)
Brick::Brick() : position{ 0, 0 }, size{ 0, 0 }, row(0), col(0), lives(0), color(BLANK) {}

void Brick::Init(Vector2 pos, Vector2 sz, int r, int c, int lvs) {
    position = pos;
    size = sz;
    row = r;
    col = c;
    lives = lvs;
    if (lives == 3) color = MAROON;
    else if (lives == 2) color = RED;
    else color = ORANGE;
}

void Brick::Draw() const {
    if (lives > 0) {
        DrawRectangleV(position, size, color);
    }
}

Rectangle Brick::GetRect() const {
    return { position.x, position.y, size.x, size.y };
}

bool Brick::IsDestroyed() const {
    return lives <= 0;
}

void Brick::Hit() {
    if (lives > 0) {
        lives--;
        if (lives == 2) color = RED;
        else if (lives == 1) color = ORANGE;
        // Color becomes BLANK or similar implicitly when Draw isn't called for lives <= 0
    }
}