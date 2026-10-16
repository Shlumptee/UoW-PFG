#include <iostream>

#include "GameLoop.h"

int GameLoop::Init()
{
    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    canvas.create(1024, 768, "Tiles");
    isRunning = true; // Variable to control the main loop's running state.

    return 1;
}

void GameLoop::HandleInput()
{
    // Check for input (key presses or window events)
    if (canvas.keyPressed(VK_ESCAPE))
    {
        isRunning = false;
    }
    if (canvas.keyPressed(VK_SHIFT))
    {
        if (canvas.keyPressed('R'))
        {
            red++;
            if (red > 255)
            {
                red = 0;
            }
        }
        if (canvas.keyPressed('G'))
        {
            green++;
            if (green > 255)
            {
                green = 0;
            }
        }
        if (canvas.keyPressed('B'))
        {
            blue++;
            if (blue > 255)
            {
                blue = 0;
            }
        }
    }
    else if (canvas.keyPressed(VK_CONTROL))
    {
        if (canvas.keyPressed('R'))
        {
            red--;
            if (red < 0)
            {
                red = 255;
            }
        }
        if (canvas.keyPressed('G'))
        {
            green--;
            if (green < 0)
            {
                green = 255;
            }
        }
        if (canvas.keyPressed('B'))
        {
            blue--;
            if (blue < 0)
            {
                blue = 255;
            }
        }
    }
}

void GameLoop::Update()
{
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

    image.load("./assets/Circle.png");
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
    }
}
