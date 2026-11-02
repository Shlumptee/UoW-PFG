#include <iostream>

#include "GameScreen.h"
#include "Enemy.h"
#include "GameManager.h"

#pragma region MainMenuScreen

void MainMenuScreen::Init()
{
	// Initialize main menu screen
    GameScreen::Init();
}

void MainMenuScreen::HandleInput()
{
    if (window.keyPressed('P'))
    {
        gameManager.SetState(GameState::Game);
    }
}

void MainMenuScreen::Update(float deltaTime)
{
	// Update main menu screen
}

void MainMenuScreen::Draw(GamesEngineeringBase::Window& window)
{
	// Draw main menu screen
    Render_Background(window, 184, 143, 67);

    Render_String(window, GetFontImage(), "Main Menu", {350, 25});
}

void MainMenuScreen::Destroy()
{
	// Clean up resources for main menu screen
	std::cout << "Main Menu Screen Destroyed" << std::endl;
}
#pragma endregion

#pragma region GameplayScreen

void GameplayScreen::Init()
{
	// Initialize gameplay screen
	GameScreen::Init();

    quadtree = new Quadtree(0, Rect(Vector2(0, 0), Vector2(1024 * 3, 768 * 3)));
    tilemap = new Tilemap(
        Vector2(32, 32),
        32,
        Vector2(200, 200),
        "./assets/Levels/3 RLE.txt", "./assets/TileMapAtlas.png");

    player = new Player(Vector2(100, 100), "./assets/Circle.png");
    entities.Push(player);
    entities.Push(new Enemy(Vector2(500, 300), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(600, 110), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(100, 600), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(400, 150), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(100, 500), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(500, 200), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(500, 100), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(800, 600), player, "./assets/Enemy.png"));

    entities.Push(new Enemy(Vector2(300, 850), player, "./assets/Enemy.png"));

    camera = new Camera(Vector2(window.getWidth(), window.getHeight()));
    camera->SetWorldBounds(Vector2(3200.0f, 2400.0f));
}

void GameplayScreen::HandleInput()
{
	// Handle input for gameplay screen
    // Check for input (key presses or window events)
    input.Set(0, 0);
    if (window.keyPressed('A'))
    {
        input.x -= 1.0f;
    }
    if (window.keyPressed('D'))
    {
        input.x += 1.0f;
    }
    if (window.keyPressed('S'))
    {
        input.y += 1.0f;
    }
    if (window.keyPressed('W'))
    {
        input.y -= 1.0f;
    }

    player->SetInput(input);
    player->SetMeleeInput(window.keyPressed('E'));
}

void GameplayScreen::Update(float deltaTime)
{
	// Update gameplay screen
    fpsCount++;
    fpsTimer += deltaTime;

    if (fpsTimer >= 1.0f)
    {
        fps = (int)(fpsCount / fpsTimer);

        fpsCount = 0;
        fpsTimer = 0.0f;
    }

    quadtree->Clear();
    for (int i = 0; i < entities.GetSize(); i++)
    {
        entities[i]->Update(deltaTime);

        CollisionObject object(entities[i], entities[i]->GetBoundingBox());
        quadtree->Insert(object);
    }

    collisionManager.Update(*quadtree, entities);

    camera->Follow(*player);
    quadtree->SetBounds(Rect(Vector2(camera->GetPosition().x - (window.getWidth() / 2), camera->GetPosition().y - (window.getHeight() / 2)), Vector2(camera->GetPosition().x + (window.getWidth() / 2), camera->GetPosition().y + (window.getHeight() / 2))));

    if (player->ConsumeMeleeRequest())
    {
        player->MeleeAttack(*quadtree, entities);
    }
}

void GameplayScreen::Draw(GamesEngineeringBase::Window& window)
{
	// Draw gameplay screen
    /*for (int i = 0; i < window.getHeight(); i++)
    {
        for (int j = 0; j < window.getWidth(); j++)
        {
            window.draw(j, i, 0, 0, 128);
        }
    }*/
    Render_Background(window);

    tilemap->Draw(window, *camera);

    for (int i = 0; i < entities.GetSize(); i++)
    {
        entities[i]->Draw(window, *camera);
    }

    //Render_String(window, fontImage, "sjdnsjdjsdhjsdhsjdhjsdhjsdhjshdjhdsjhshdjsd", Vector2(0, 0));
	std::string fpsString = "FPS:" + std::to_string(fps);
    Render_String(window, GetFontImage(), fpsString.c_str(), Vector2(1024 - 32 * 8, 768 - 32), 32);
    Render_Number(window, GetFontImage(), (int)player->GetHealth().GetCurrentHealth(), Vector2(32, 0), 32);
}

void GameplayScreen::Destroy()
{
	// Clean up resources for gameplay screen
    for (int i = 0; i < entities.GetSize(); i++)
    {
        delete entities[i];
    }
    entities.Clear();

    if (quadtree != nullptr)
    {
        delete quadtree;
        quadtree = nullptr;
    }

    player = nullptr;
}

#pragma endregion

#pragma region GameplayScreenInfinite

void GameplayScreenInfinite::Init()
{
	// Initialize gameplay screen infinite
}

void GameplayScreenInfinite::HandleInput()
{
	// Handle input for gameplay screen infinite
}

void GameplayScreenInfinite::Update(float deltaTime)
{
	// Update gameplay screen infinite
}

void GameplayScreenInfinite::Draw(GamesEngineeringBase::Window& window)
{
	// Draw gameplay screen infinite
}

void GameplayScreenInfinite::Destroy()
{
	// Clean up resources for gameplay screen infinite
}

#pragma endregion

#pragma region TallyScreen

void TallyScreen::Init()
{
	// Initialize tally screen
	GameScreen::Init();
}

void TallyScreen::HandleInput()
{
	// Handle input for tally screen
	if (window.keyPressed('M'))
	{
		gameManager.SetState(GameState::MainMenu);
	}
	if (window.keyPressed(VK_SPACE))
	{
		gameManager.SetState(GameState::Game);
	}
}

void TallyScreen::Update(float deltaTime)
{
	// Update tally screen
}

void TallyScreen::Draw(GamesEngineeringBase::Window& window)
{
	// Draw tally screen
	Render_String(window, GetFontImage(), "Tally Screen", {350, 25});
}

void TallyScreen::Destroy()
{
	// Clean up resources for tally screen
}

#pragma endregion

#pragma region GameOverScreen

void GameOverScreen::Init()
{
	// Initialize game over screen
	GameScreen::Init();
}

void GameOverScreen::HandleInput()
{
	// Handle input for game over screen
	if (window.keyPressed('M'))
	{
		gameManager.SetState(GameState::MainMenu);
	}
	if (window.keyPressed('R'))
	{
		gameManager.SetState(GameState::Game);
	}
}

void GameOverScreen::Update(float deltaTime)
{
	// Update game over screen
}

void GameOverScreen::Draw(GamesEngineeringBase::Window& window)
{
	// Draw game over screen
	Render_Background(window, 128, 0, 0);

	Render_String(window, GetFontImage(), "Game Over!", { 350, 25 });
}

void GameOverScreen::Destroy()
{
	// Clean up resources for game over screen
}

#pragma endregion