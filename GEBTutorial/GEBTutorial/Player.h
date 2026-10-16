#pragma once

#include "GamesEngineeringBase.h"


class Player
{
public:
	void Init();
	void Update();
	void Draw();

	void MoveLeft()
	{
		position[1] -= moveSpeed;

		if (position[1] < 0)
		{
			position[1] = 0;
		}
	}

	/*void MoveRight()
	{
		position[1] += moveSpeed;

		if (position[1] > canvas.getWidth())
		{
			position[1] -= moveSpeed;
		}
	}*/

private:
	int moveSpeed = 1;
	int position[2] = { 100, 100 };
};

