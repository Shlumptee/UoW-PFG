#include "GameManager.h"

int GameManager::Init()
{
    window.create(1024, 768, "Blast'ems++");

    isRunning = true;
	fontImage.load("./assets/text2.png");

    ChangeScreen();

    return 1;
}

void GameManager::HandleInput()
{
	if (window.keyPressed(VK_ESCAPE))
	{
		isRunning = false;
	}
    if (window.keyPressed('1'))
    {
		currentState = GameState::MainMenu;
		ChangeScreen();
	}
	else if (window.keyPressed('2'))
	{
		currentState = GameState::Game;
		ChangeScreen();
	}
	else if (window.keyPressed('3'))
	{
		currentState = GameState::GameInfinite;
		ChangeScreen();
	}
	else if (window.keyPressed('4'))
	{
		currentState = GameState::TallyScreen;
		ChangeScreen();
	}
	else if (window.keyPressed('5'))
	{
		currentState = GameState::GameOver;
		ChangeScreen();
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
	if (currentScreen != nullptr)
	{
		currentScreen->Destroy();
	}
    switch (currentState)
    {
    case GameState::MainMenu:
        currentScreen = &mainMenuScreen;
        break;

    case GameState::Game:
        currentScreen = &gameplayScreen;
        break;

    case GameState::GameInfinite:
        currentScreen = &gameplayScreenInfinite;
        break;

    case GameState::TallyScreen:
        currentScreen = &tallyScreen;
        break;

    case GameState::GameOver:
        currentScreen = &gameOverScreen;
        break;
    }

    if (currentScreen != nullptr)
    {
        currentScreen->Init();
    }
}