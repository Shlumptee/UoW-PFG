#pragma once

#include "GamesEngineeringBase.h"
#include "GEBase.cpp"

class GameLoop
{
public:
	int Init();
	void HandleInput();
	void Update();
	void Render();
};

