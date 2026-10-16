#pragma once

#include "GamesEngineeringBase.h"

class GameLoop
{
public:
	int Init();
	void HandleInput();
	void Update();
	void Draw();

	bool isRunning;
	GamesEngineeringBase::Window canvas;
	GamesEngineeringBase::Image image;

private:
	unsigned int red = 0, green = 0, blue = 0;
};

