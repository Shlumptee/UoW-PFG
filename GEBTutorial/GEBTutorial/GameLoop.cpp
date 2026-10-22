#include <iostream>

#include "GameLoop.h"

int GameLoop::Init()
{
    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    canvas.create(1024, 768, "Tiles");
    isRunning = true; // Variable to control the main loop's running state.
    //player->Init();

    return 1;
}

void GameLoop::HandleInput()
{
    // Check for input (key presses or window events)
    if (canvas.keyPressed(VK_ESCAPE))
    {
        isRunning = false;
    }

	Vector2 input{ 0, 0 };
    if (canvas.keyPressed('A'))
    {
        input.x = -1.0f;
    }
    if (canvas.keyPressed('D'))
    {
        input.x = 1.0f;
    }
    if (canvas.keyPressed('S'))
    {
        input.y = -1.0f;
    }
    if (canvas.keyPressed('W'))
    {
        input.y = 1.0f;
    }
}

void GameLoop::Update()
{
	player.Update(deltaTime);
}

void GameLoop::Draw()
{
    for (int i = 0; i < canvas.getHeight(); i++)
    {
        for (int j = 0; j < canvas.getWidth(); j++)
        {
            canvas.draw(j, i, 0, 0, 128);
        }
    }

    for (int i = 0; i < canvas.getHeight(); i++)
    {
        canvas.draw(70, i, 255, 255, 255);
    }

    /*image.load("./assets/Circle.png");
    for (int i = 0; i < image.width; i++)
    {
        for (int j = 0; j < image.height; j++)
        {
            if (!image.alphaAt(i, j))
            {
                continue;
            }

            unsigned char* pixel = image.at(i, j);
            canvas.draw(100 + i, 100 + j, red, green, blue);
        }
    }*/

	player->Draw();
}

void GameLoop::Destroy()
{
	canvas.~Window();
	player->Destroy();

    delete player;
}