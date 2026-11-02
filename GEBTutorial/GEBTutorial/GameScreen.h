#pragma once

#include "GameLoop.h"
#include "GamesEngineeringBase.h"

class GameManager;

class GameScreen
{
public:
	GameScreen(GamesEngineeringBase::Window& _window, GameManager& _gameManager) : window(_window), gameManager(_gameManager)
	{

	}
	virtual ~GameScreen() = default;

	virtual void Init()
	{
		fontImage.load("./assets/text2.png");
	}
	virtual void HandleInput() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(GamesEngineeringBase::Window& window) = 0;
	virtual void Destroy() = 0;

	GamesEngineeringBase::Image& GetFontImage()
	{
		return fontImage;
	}

protected:
	GamesEngineeringBase::Window& window;
	GameManager& gameManager;

private:
	GamesEngineeringBase::Image fontImage;
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
	Tilemap* tilemap = nullptr;
	Camera* camera = nullptr;

	float deltaTime = 0.0f;

	GamesEngineeringBase::Timer timer;
	Vector2 input{ 0, 0 };

	float fpsTimer = 0.0f;
	int fpsCount = 0;
	int fps = 0;
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
};