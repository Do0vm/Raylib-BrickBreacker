#include "raylib.h"
#include <iostream>
#include <vector>
#include "Paddle.h"
#include <string>

using namespace std;
void ScoreGoal(bool ScoredLeft);


int lScore=0;
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 900;
const float PADDLE_WIDTH = 10.0f;
const float PADDLE_HEIGHT = 150.0f;
const float PADDLE_SPEED = 15.0f;
const float PADDLE_DECELERATION = 0.95f;
Vector2 lLeftPosition = { WINDOW_WIDTH / 4.0f, 0 };


int main() {



    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gregs Raylib");
    SetTargetFPS(60);

    Font ft = LoadFont("resources/fonts/alagard.png");

    int ballX = WINDOW_WIDTH / 2;
    int ballY = WINDOW_HEIGHT / 2;


    Paddle leftPaddle;

    int movementX = -10;
    int movementY = -10;



    leftPaddle.Init(
        Vector2{ 500.0f, (WINDOW_HEIGHT / 1.0f) - (PADDLE_HEIGHT / 2.0f) },
        Vector2{ 0.0f, 0.0f },
        PADDLE_HEIGHT,
        PADDLE_WIDTH,
        WHITE);



    

    while (!WindowShouldClose()) {

        ballX += movementX;
        ballY += movementY;




        Vector2 ballPos = { static_cast<float>(ballX), static_cast<float>(ballY) };
        float ballRadius = 10.0f;

        if (CheckCollisionCircleRec(ballPos, ballRadius, leftPaddle.GetPaddleRectangle()))
        {
            movementX = abs(movementX);
            ballX = leftPaddle.GetPosition().x + leftPaddle.GetWidth() + ballRadius;
        }
       

        if (ballY - ballRadius <= 0 || ballY + ballRadius >= WINDOW_HEIGHT)
        {
            movementY = -movementY;
        }
        
        //if (ballX - ballRadius <= 0 || ballX + ballRadius >= x)
        //{

        //    ballX = x / 12;
        //    ballY = y / 12;
        //    movementX = abs(movementX);
        //    movementY= abs(movementY);
        //}
        //Scoring logic
        if (ballX - ballRadius <= 0)
        {       
            ScoreGoal(false);
            ballX = WINDOW_WIDTH / 12;
            ballY = WINDOW_HEIGHT / 12;
            movementX = abs(movementX);
            movementY = abs(movementY);
        }
        else if (ballX + ballRadius >= WINDOW_WIDTH)
        {
            ScoreGoal(true);     
            ballX = WINDOW_WIDTH / 12;
            ballY = WINDOW_HEIGHT / 12;
            movementX = abs(movementX);
            movementY = abs(movementY);
        }

        
        //Left Paddle
        leftPaddle.Update();
        if (IsKeyDown('D'))
            leftPaddle.SetSpeed(Vector2{ PADDLE_SPEED, 0.0f });
        else if (IsKeyDown('A'))
            leftPaddle.SetSpeed(Vector2{ -PADDLE_SPEED, 0.0f });
        else
            leftPaddle.SetSpeed(Vector2{ 0.0f, leftPaddle.GetSpeed().y * PADDLE_DECELERATION });
       
        //collision left paddle
        if (leftPaddle.GetPosition().y <= 0)
            leftPaddle.SetPosition(Vector2{ leftPaddle.GetPosition().x, 0 });
        else if (leftPaddle.GetPosition().y + leftPaddle.GetHeight() >= WINDOW_HEIGHT)
            leftPaddle.SetPosition(Vector2{ leftPaddle.GetPosition().x, WINDOW_HEIGHT - leftPaddle.GetHeight() });
 



        BeginDrawing();



        ClearBackground(DARKGREEN);
        DrawText("HitMe", ballX - 10, ballY - 40, 15, RED);
        DrawCircle(ballX, ballY, ballRadius, Color{ 2,222,233,242 });
        leftPaddle.Draw();
        DrawTextEx(ft, TextFormat("%i",lScore), lLeftPosition, 65, 2, DARKPURPLE);



        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void ScoreGoal(bool ScoredLeft)
{

    if (ScoredLeft) { lScore++; }
    
        
}
