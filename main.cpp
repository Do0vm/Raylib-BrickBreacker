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



    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BRICKBREAKER RAYLIB by GREG");
    SetTargetFPS(60);

    Font ft = LoadFont("resources/fonts/alagard.png");

    int ballX = WINDOW_WIDTH / 2;
    int ballY = WINDOW_HEIGHT / 2;


    Paddle leftPaddle;

    int movementX = 5;
    int movementY = -5;



    leftPaddle.Init(
        Vector2{ 500.0f, (WINDOW_HEIGHT *(0.9f)) - (PADDLE_HEIGHT / 2.0f) },
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
            movementY = abs(movementY);
            ballY = leftPaddle.GetPosition().y + leftPaddle.GetHeight() + ballRadius;
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
        if (ballY - ballRadius <= 0)
        {       
            ScoreGoal(false);
            ballX = WINDOW_WIDTH / 6;
            ballY = WINDOW_HEIGHT / 6;
            movementX = abs(movementX);
            movementY = abs(movementY);
        }
        else if (ballY + ballRadius >= WINDOW_HEIGHT)
        {
            ScoreGoal(true);     
            ballX = WINDOW_WIDTH / 6;
            ballY = WINDOW_HEIGHT / 6;
            movementX = abs(movementX);
            movementY = abs(movementY);
        }

        
        //Paddle
        leftPaddle.Update();
        if (IsKeyDown('D'))
            leftPaddle.SetSpeed(Vector2{ PADDLE_SPEED, 0.0f });
        else if (IsKeyDown('A'))
            leftPaddle.SetSpeed(Vector2{ -PADDLE_SPEED, 0.0f });
        else
            leftPaddle.SetSpeed(Vector2{ leftPaddle.GetSpeed().x * PADDLE_DECELERATION ,0.0f});
       
        //collision paddle
        if (leftPaddle.GetPosition().x <= 0)
            leftPaddle.SetPosition(Vector2{ 0.0f , leftPaddle.GetPosition().y });

        else if (leftPaddle.GetPosition().x + leftPaddle.GetWidth() >= WINDOW_WIDTH)
            leftPaddle.SetPosition(Vector2{ WINDOW_WIDTH - leftPaddle.GetWidth() ,leftPaddle.GetPosition().y });



 



        BeginDrawing();



        ClearBackground(RED);
        DrawText("++", ballX - 15, ballY + 1, 30, DARKBLUE);
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
