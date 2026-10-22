#pragma once

#include "GamesEngineeringBase.h"
#include "Player.h"

class GameLoop
{
public:
	GameLoop(/*const GamesEngineeringBase::Window& _canvas*/)
	{
		//canvas = _canvas;
	}

	int Init();
	void HandleInput();
	void Update();
	void Draw();
	void Destroy();

	bool isRunning;
	GamesEngineeringBase::Window canvas;

private:
	unsigned int red = 0, green = 0, blue = 0;
	Player player;

	float deltaTime = 0.0f;

	GamesEngineeringBase::Timer timer;
};

