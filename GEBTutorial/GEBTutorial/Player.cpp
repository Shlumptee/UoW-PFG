#include <iostream>

#include "Player.h"

void Player::Init()
{
	image.load("./assets/Circle.png");
	abilityTimer.reset();
}

void Player::HandleInput()
{
	if (canvas.keyPressed('A'))
	{
		MoveLeft();
	}
	if (canvas.keyPressed('D'))
	{
		MoveRight();
	}
	if (canvas.keyPressed('S'))
	{
		MoveDown();
	}
	if (canvas.keyPressed('W'))
	{
		MoveUp();
	}
}

void Player::Update()
{
	//std::cout << "Player position: (" << position[0] << ", " << position[1] << ")" << std::endl;
}

void Player::Draw()
{
	for (int i = 0; i < image.width; i++)
	{
		for (int j = 0; j < image.height; j++)
		{
			if (!image.alphaAt(i, j))
			{
				continue;
			}

			unsigned char* pixel = image.at(i, j);
			canvas.draw(position[0] + i, position[1] + j, pixel[0], pixel[1], pixel[2]);
		}
	}
}

void Player::Destroy()
{
	canvas.~Window();
	image.~Image();
}