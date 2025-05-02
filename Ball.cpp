#include "Ball.h"
#include "raylib.h" // For GetFrameTime, DrawCircleV

// Default constructor implementation
Ball::Ball() : position{ 0,0 }, speed{ 0,0 }, radius(0.0f), active(false), color(WHITE) {}

void Ball::Init(Vector2 pos, Vector2 spd, float rad, Color col) {
    position = pos;
    speed = spd;
    radius = rad;
    active = true; // Ensure ball starts active
    color = col;
}

void Ball::Update() {
    if (active) {
        // Update position based on speed and frame time
        position.x += speed.x * GetFrameTime();
        position.y += speed.y * GetFrameTime();
    }
}

void Ball::Draw() const {
    if (active) {
        DrawCircleV(position, radius, color);
    }
}