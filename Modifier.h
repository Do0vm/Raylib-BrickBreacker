#ifndef MODIFIER_H
#define MODIFIER_H

#include "raylib.h"
#include "Constants.h" // For ModifierType, MODIFIER_SIZE, WINDOW_HEIGHT

class Modifier {
public:
    Vector2 position;
    Vector2 velocity;
    ModifierType type;
    bool active;
    Color color;
    float size;

    Modifier(); 

    void Init(Vector2 pos, ModifierType t);
    void Update();
    void Draw() const;
    Rectangle GetRect() const;
};

#endif 