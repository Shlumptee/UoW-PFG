#pragma once

#include "GamesEngineeringBase.h"
#include "GameScreen.h"


enum class GameState
{
	MainMenu,
	Game,
	GameInfinite,
	TallyScreen,
	GameOver
};

class GameManager
{
public:
	GameManager()
	{
		currentState = GameState::Game;
	}

	int Init();
	void HandleInput();
	void Update();
	void Draw();
	void Destroy();

	bool IsRunning() const
	{
		return isRunning;
	}
	void SetState(GameState state)
	{
		currentState = state;
	}
	GameState GetState() const
	{
		return currentState;
	}
	GamesEngineeringBase::Window& GetWindow()
	{
		return window;
	}

private:
	GamesEngineeringBase::Window window;
	GamesEngineeringBase::Timer timer;
	GameState currentState;
	GameScreen* currentScreen = nullptr;

	GameplayScreen gameplayScreen{ window };

	bool isRunning = false;
	float deltaTime = 0.0f;

	void ChangeScreen();
};

