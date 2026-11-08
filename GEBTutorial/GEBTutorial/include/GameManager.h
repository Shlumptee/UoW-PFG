#pragma once

#include "GamesEngineeringBase.h"
#include "GameScreen.h"
#include "ScoreManager.h"

// Enum for game screens
enum class GameState
{
	MainMenu,
	Game,
	GameInfinite,
	TallyScreen,
	GameOver
};

// Enum for what type of class the player has selected
enum class PlayerClass
{
	None,
	Warrior,
	Archer,
	Mage
};

class GameManager
{
public:
	GamesEngineeringBase::Image fontImage;

	GameManager()
	{
		currentState = GameState::MainMenu;
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
		ChangeScreen();
	}
	void SetPlayerClass(PlayerClass _playerClass)
	{
		playerClass = _playerClass;
	}
	void SetPlayerHealth(float _playerHealth)
	{
		playerHealth = _playerHealth;
	}
	GameState GetState() const
	{
		return currentState;
	}
	GamesEngineeringBase::Window& GetWindow()
	{
		return window;
	}
	ScoreManager& GetScoreManager()
	{
		return scoreManager;
	}
	PlayerClass GetPlayerClass()
	{
		return playerClass;
	}
	float GetPlayerHealth()
	{
		return playerHealth;
	}

private:
	GamesEngineeringBase::Window window;
	GamesEngineeringBase::Timer timer;

	GameState currentState;
	GameScreen* currentScreen = nullptr;

	GameplayScreen gameplayScreen{ window, *this };
	GameplayScreenInfinite gameplayScreenInfinite{ window, *this };
	MainMenuScreen mainMenuScreen{ window, *this };
	TallyScreen tallyScreen{ window, *this };
	GameOverScreen gameOverScreen{ window, *this };

	PlayerClass playerClass = PlayerClass::None;

	ScoreManager scoreManager;

	bool isRunning = false;
	float deltaTime = 0.0f;

	float playerHealth = -1;

	void ChangeScreen();
};

