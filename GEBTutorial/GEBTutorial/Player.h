#pragma once

#include "GamesEngineeringBase.h"


class Player
{
public:
	Player(const GamesEngineeringBase::Window& _canvas)
	{
		canvas = _canvas;
	}

	void Init();
	void HandleInput();
	void Update();
	void Draw();
	void Destroy();

	void MoveLeft()
	{
		position[0] -= moveSpeed;

		if (position[0] < 0)
		{
			position[0] = 0;
		}
	}

	void MoveRight()
	{
		position[0] += moveSpeed;

		if (position[0] > canvas.getWidth() - image.height)
		{
			position[0] -= moveSpeed;
		}
	}

	void MoveDown()
	{
		position[1] += moveSpeed;

		if (position[1] > (canvas.getHeight() - image.height))
		{
			position[1] -= moveSpeed;
		}
	}

	void MoveUp()
	{
		position[1] -= moveSpeed;
		if (position[1] < 0)
		{
			position[1] = 0;
		}
	}

private:
	int moveSpeed = 1;
	int position[2] = { 100, 100 };

	GamesEngineeringBase::Window canvas;
	GamesEngineeringBase::Image image;
};

