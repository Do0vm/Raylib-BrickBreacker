#pragma once
#ifndef FLOATING_TEXT_H
#define FLOATING_TEXT_H

#include "raylib.h"
#include <string>

struct FloatingText {
    Vector2 position;
    Vector2 velocity;
    std::string text;
    Color color;
    int fontSize;
    float lifeTime;
    float initialLifeTime;
    bool active;
    Font* pGameFont; // Pointer to the game font

    FloatingText(); 

    // Pass font by pointer during Init
    void Init(Font* font, Vector2 pos, Vector2 vel, std::string txt, Color col, int size, float life);
    void Update();
    void Draw() const;
};

#endif