#include "GameManager.h"

// Initialise the game
int GameManager::Init()
{
	// Create window
    window.create(1024, 768, "Blast'ems++");

    isRunning = true;
	fontImage.load("./assets/text2.png");

	// Change the screen to the main menu
    ChangeScreen();

    return 1;
}

// Handle universal input across screens
// This allows for the player to forceload screens and quit out anytime
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

// Update deltatime and call current screen's update
void GameManager::Update()
{
	deltaTime = timer.dt();
	if (currentScreen != nullptr)
	{
		currentScreen->Update(deltaTime);
	}
}

// Call current screen's draw
void GameManager::Draw()
{
	if (currentScreen != nullptr)
	{
		currentScreen->Draw(window);
	}
}

// Destroy all pointers in the manager when game closes
void GameManager::Destroy()
{
	if (currentScreen)
	{
		currentScreen = nullptr;
	}
    isRunning = false;
}

// Change the screen state
void GameManager::ChangeScreen()
{
	// call destroy function for the previous screen being used
	if (currentScreen != nullptr)
	{
		currentScreen->Destroy();
	}

	// Switch to the new game state
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

	// Call the screen's init function to set up screen
    if (currentScreen != nullptr)
    {
        currentScreen->Init();
    }
}