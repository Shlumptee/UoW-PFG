#pragma once

#include "Player.h"
#include "Quadtree.h"
#include "CollisionManager.h"
#include "Tilemap.h"
#include "Camera.h"
#include "GamesEngineeringBase.h"
#include "ProjectileManager.h"

class GameManager;

class GameScreen
{
public:
	GameScreen(GamesEngineeringBase::Window& _window, GameManager& _gameManager) : window(_window), gameManager(_gameManager)
	{
	}
	virtual ~GameScreen()
	{
	}

	virtual void Init()
	{
	}
	virtual void HandleInput() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(GamesEngineeringBase::Window& window) = 0;
	virtual void Destroy() = 0;

	// Needed across all game screens
	void UpdateFPSCounter(float deltaTime)
	{
		fpsCount++;
		fpsTimer += deltaTime;
		if (fpsTimer >= 1.0f)
		{
			fps = (int)(fpsCount / fpsTimer);
			fpsCount = 0;
			fpsTimer = 0.0f;
		}
	}

	void RenderFPSCounter(GamesEngineeringBase::Window& window);

protected:
	GamesEngineeringBase::Window& window;
	GameManager& gameManager;

private:
	float fpsTimer;
	int fpsCount;
	int fps;
};

class MainMenuScreen : public GameScreen
{
public:
	MainMenuScreen(GamesEngineeringBase::Window& _window, GameManager& _gameManager) : GameScreen(_window, _gameManager)
	{
	}
	void Init() override;
	void HandleInput() override;
	void Update(float deltaTime) override;
	void Draw(GamesEngineeringBase::Window& window) override;
	void Destroy() override;
};

class GameplayScreen : public GameScreen
{
public:
	GameplayScreen(GamesEngineeringBase::Window& _window, GameManager& _gameManager) : GameScreen(_window, _gameManager)
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
	ProjectileManager projectileManager;
	Tilemap* tilemap = nullptr;
	Camera* camera = nullptr;

	float levelTimer = 0.0f;
	float maxTime = 60.0f;

	GamesEngineeringBase::Timer timer;
	Vector2 input{ 0, 0 };
};

class GameplayScreenInfinite : public GameScreen
{
public:
	GameplayScreenInfinite(GamesEngineeringBase::Window& _window, GameManager& _gameManager) : GameScreen(_window, _gameManager)
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
	ProjectileManager projectileManager;
	Tilemap* tilemap = nullptr;
	Camera* camera = nullptr;

	float levelTimer = 0.0f;
	float maxTime = 60.0f;

	Vector2 input{ 0, 0 };
};

class TallyScreen : public GameScreen
{
public:
	TallyScreen(GamesEngineeringBase::Window& _window, GameManager& _gameManager) : GameScreen(_window, _gameManager)
	{

	}

	void Init() override;
	void HandleInput() override;
	void Update(float deltaTime) override;
	void Draw(GamesEngineeringBase::Window& window) override;
	void Destroy() override;

private:
	int score = 0;
	int kills = 0;
	float timeAlive = 0.0f;

	std::string scoreString;
	std::string killsString;
	std::string timeAliveString;
};

class GameOverScreen : public GameScreen
{
public:
	GameOverScreen(GamesEngineeringBase::Window& _window, GameManager& _gameManager) : GameScreen(_window, _gameManager)
	{
	}

	void Init() override;
	void HandleInput() override;
	void Update(float deltaTime) override;
	void Draw(GamesEngineeringBase::Window& window) override;
	void Destroy() override;

private:
	int score = 0;
	int kills = 0;
	float timeAlive = 0.0f;

	std::string scoreString;
	std::string killsString;
	std::string timeAliveString;
};