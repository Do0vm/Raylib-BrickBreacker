#pragma once
#ifndef BALL_H
#define BALL_H

#include "raylib.h"

struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
    Color color;

    // Default constructor (optional but good practice)
    Ball();

    void Init(Vector2 pos, Vector2 spd, float rad, Color col = WHITE);
    void Update();
    void Draw() const;
};

#endif // BALL_H