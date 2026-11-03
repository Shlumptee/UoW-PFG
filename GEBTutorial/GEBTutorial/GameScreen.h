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

	void RenderFPSCounter(GamesEngineeringBase::Window& window)
	{
		std::string fpsString = "FPS:" + std::to_string(fps);
		Render_String(window, fontImage, fpsString.c_str(), Vector2(1024 - 32 * 8, 768 - 32), 32);
	}

	GamesEngineeringBase::Image& GetFontImage()
	{
		return fontImage;
	}

protected:
	GamesEngineeringBase::Window& window;
	GameManager& gameManager;

private:
	GamesEngineeringBase::Image fontImage;

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
	Tilemap* tilemap = nullptr;
	Camera* camera = nullptr;

	float deltaTime = 0.0f;

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
};