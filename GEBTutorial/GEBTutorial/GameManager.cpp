#include "GameManager.h"

int GameManager::Init()
{
    window.create(1024, 768, "Blast'ems++");

    isRunning = true;

    ChangeScreen();

    return 1;
}

void GameManager::HandleInput()
{
	if (window.keyPressed(VK_ESCAPE))
	{
		isRunning = false;
	}
	if (currentScreen != nullptr)
	{
		currentScreen->HandleInput();
	}
}

void GameManager::Update()
{
	deltaTime = timer.dt();
	if (currentScreen != nullptr)
	{
		currentScreen->Update(deltaTime);
	}
}

void GameManager::Draw()
{
	if (currentScreen != nullptr)
	{
		currentScreen->Draw(window);
	}
}

void GameManager::Destroy()
{
	if (currentScreen)
	{
		currentScreen = nullptr;
	}
    isRunning = false;
}

void GameManager::ChangeScreen()
{
    switch (currentState)
    {
    case GameState::MainMenu:
        // activeScreen = &mainMenuScreen;
        break;

    case GameState::Game:
        currentScreen = &gameplayScreen;
        break;

    case GameState::GameInfinite:
        // activeScreen = &infiniteGameplayScreen;
        break;

    case GameState::TallyScreen:
        // activeScreen = &tallyScreen;
        break;

    case GameState::GameOver:
        // activeScreen = &gameOverScreen;
        break;
    }

    if (currentScreen != nullptr)
    {
        currentScreen->Init();
    }
}