#pragma once

#include "GameLoop.h"
#include "GamesEngineeringBase.h"

class GameManager;

class GameScreen
{
public:
	GameScreen(GamesEngineeringBase::Window& _window) : window(_window)
	{

	}
	virtual ~GameScreen() = default;

	virtual void Init() = 0;
	virtual void HandleInput() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(GamesEngineeringBase::Window& window) = 0;
	virtual void Destroy() = 0;
protected:
	GamesEngineeringBase::Window& window;
};

class MainMenuScreen : public GameScreen
{
public:
	MainMenuScreen(GamesEngineeringBase::Window& _window) : GameScreen(_window)
	{
	}
	void Init() override;
	void HandleInput() override;
	void Update(float deltaTime) override;
	void Draw(GamesEngineeringBase::Window& window) override;
	void Destroy() override;
private:
	GamesEngineeringBase::Image fontImage;
};

class GameplayScreen : public GameScreen
{
public:
	GameplayScreen(GamesEngineeringBase::Window& _window) : GameScreen(_window)
	{
	}
	void Init() override;
	void HandleInput() override;
	void Update(float deltaTime) override;
	void Draw(GamesEngineeringBase::Window& window) override;
	void Destroy() override;
private:
	DynamicArray<VisualEntity*> entities;
	Player* player = nullptr;
	Quadtree* quadtree = nullptr;
	CollisionManager collisionManager;
	Tilemap* tilemap = nullptr;
	Camera* camera = nullptr;

	float deltaTime = 0.0f;

	GamesEngineeringBase::Timer timer;
	Vector2 input{ 0, 0 };

	GamesEngineeringBase::Image fontImage;

	float fpsTimer = 0.0f;
	int fpsCount = 0;
	int fps = 0;
};

class GameplayInfScreen : public GameScreen
{
public:
	GameplayInfScreen(GamesEngineeringBase::Window& _window) : GameScreen(_window)
	{

	}

	void Init() override;
	void HandleInput() override;
	void Update(float deltaTime) override;
	void Draw(GamesEngineeringBase::Window& window) override;
	void Destroy() override;
};

class TallyScreen : public GameScreen
{
public:
	TallyScreen(GamesEngineeringBase::Window& _window) : GameScreen(_window)
	{

	}

	void Init() override;
	void HandleInput() override;
	void Update(float deltaTime) override;
	void Draw(GamesEngineeringBase::Window& window) override;
	void Destroy() override;
};