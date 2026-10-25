#pragma once

#include "GamesEngineeringBase.h"
#include "Player.h"
#include "Quadtree.h"
#include "CollisionManager.h"

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
	DynamicArray<VisualEntity*> entities;
	Player* player = nullptr;
	Quadtree* quadtree = nullptr;
	CollisionManager collisionManager;

	float deltaTime = 0.0f;

	GamesEngineeringBase::Timer timer;
	Vector2 input{ 0, 0 };
};

