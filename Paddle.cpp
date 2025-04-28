#include "Paddle.h"

// ------------------------ MovingObject Implementation ------------------------

MovingObject::MovingObject() :
    recPosition{ Vector2{50.0f, 50.0f} },
    recSpeed{ Vector2{1.0f, 1.0f} }
{
}

MovingObject::MovingObject(Vector2 position, Vector2 speed) :
    recPosition{ position },
    recSpeed{ speed }
{
}

MovingObject::~MovingObject()
{
}

Vector2 MovingObject::GetPosition() const
{
    return recPosition;
}

void MovingObject::SetPosition(Vector2 position)
{
    recPosition = position;
}

Vector2 MovingObject::GetSpeed() const
{
    return recSpeed;
}

void MovingObject::SetSpeed(Vector2 speed)
{
    recSpeed = speed;
}

void MovingObject::Init(Vector2 position, Vector2 speed)
{
    recPosition = position;
    recSpeed = speed;
}

void MovingObject::Update()
{
    recPosition.x += recSpeed.x;
    recPosition.y += recSpeed.y;
}

// ------------------------ Paddle Implementation ------------------------

Paddle::Paddle() :
    MovingObject(),
    recWidth{ 0.0f },
    recHeight{ 0.0f },
    recColor{ WHITE }
{
}

Paddle::Paddle(Vector2 position, Vector2 speed, float width, float height, Color color) :
    MovingObject(position, speed),
    recWidth{ width },
    recHeight{ height },
    recColor{ color }
{
}



void Paddle::Init(Vector2 position, Vector2 speed, float width, float height, Color color)
{
    

    MovingObject::Init(position, speed);
    recWidth = width;
    recHeight = height;
    recColor = color;
}

void Paddle::Draw() const
{
    DrawRectangle(static_cast<int>(recPosition.x), static_cast<int>(recPosition.y),
        static_cast<int>(recWidth), static_cast<int>(recHeight), recColor);
}

Rectangle Paddle::GetPaddleRectangle() const
{
    return Rectangle{ recPosition.x, recPosition.y, recWidth, recHeight };
}

float Paddle::GetWidth() const
{
    return recWidth;
}

float Paddle::GetHeight() const
{
    return recHeight;
}
