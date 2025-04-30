#ifndef PADDLE_H
//#define PADDLE_H

#include "raylib.h"  

class MovingObject {
public:
    MovingObject();
    MovingObject(Vector2 position, Vector2 speed);
    virtual ~MovingObject();

    Vector2 GetPosition() const;
    void SetPosition(Vector2 position);
    Vector2 GetSpeed() const;
    void SetSpeed(Vector2 speed);

   
    void Init(Vector2 position, Vector2 speed);
    virtual void Update();

protected:
    Vector2 recPosition;
    Vector2 recSpeed;
};

class Paddle : public MovingObject {
public:
   
    Paddle();
    Paddle(Vector2 position, Vector2 speed, float width, float height, Color color);

    void Init(Vector2 position, Vector2 speed, float width, float height, Color color);

    void Draw() const;

    Rectangle GetPaddleRectangle() const;
    float GetWidth() const;
    float GetHeight() const;

private:
    float recWidth;
    float recHeight;
    Color recColor;
};

#endif 
