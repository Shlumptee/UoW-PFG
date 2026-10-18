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
		orientation = 3;

		if (position[0] < 0)
		{
			position[0] = 0;
		}
	}

	void MoveRight()
	{
		position[0] += moveSpeed;
		orientation = 1;

		if (position[0] > canvas.getWidth() - image.width)
		{
			position[0] = canvas.getWidth() - image.width;
		}
	}

	void MoveDown()
	{
		position[1] += moveSpeed;
		orientation = 2;

		if (position[1] > (canvas.getHeight() - image.height))
		{
			position[1] = canvas.getHeight() - image.height;
		}
	}

	void MoveUp()
	{
		position[1] -= moveSpeed;
		orientation = 0;

		if (position[1] < 0)
		{
			position[1] = 0;
		}
	}

	void MoveAbility()
	{
		if (abilityTimer.dt() < abilityCooldown)
		{
			return;
		}

		switch (orientation)
		{
		case 0:
			position[1] -= moveSpeed * 50;
			if (position[1] < 0)
			{
				position[1] = 0;
			}
			break;
		case 1:
			position[0] += moveSpeed * 50;
			if (position[0] > canvas.getWidth() - image.width)
			{
				position[0] = canvas.getWidth() - image.width;
			}
			break;
		case 2:
			position[1] += moveSpeed * 50;
			if (position[1] > (canvas.getHeight() - image.height))
			{
				position[1] = canvas.getHeight() - image.height;
			}
			break;
		case 3:
			position[0] -= moveSpeed * 50;
			if (position[0] < 0)
			{
				position[0] = 0;
			}
			break;
		default:
			break;
		}

		abilityTimer.reset();
	}

private:
	int moveSpeed = 1;
	int position[2] = { 100, 100 };
	unsigned int orientation = 0;

	float abilityCooldown = 1.0f;

	GamesEngineeringBase::Window canvas;
	GamesEngineeringBase::Image image;
	GamesEngineeringBase::Timer abilityTimer;
};

