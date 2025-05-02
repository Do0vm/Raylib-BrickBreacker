#pragma once
#ifndef BRICK_H
#define BRICK_H

#include "raylib.h"

class Brick {
public:
    Vector2 position;
    Vector2 size;
    int row, col;
    int lives;
    Color color;

    Brick(); // Default constructor declaration

    void Init(Vector2 pos, Vector2 sz, int r, int c, int lvs);
    void Draw() const;
    Rectangle GetRect() const;
    bool IsDestroyed() const;
    void Hit();
};

#endif // BRICK_H