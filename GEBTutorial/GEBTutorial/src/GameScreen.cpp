#include <iostream>
#include <random>

#include "GameScreen.h"
#include "Enemy.h"
#include "GameManager.h"

// Render the fps counter across all screen states
void GameScreen::RenderFPSCounter(GamesEngineeringBase::Window& window)
{
    std::string fpsString = "FPS:" + std::to_string(fps);
    Render_String(window, gameManager.fontImage, fpsString.c_str(), Vector2(1024 - 32 * 8, 768 - 32), 32);
}

// Pick a random map for the game loop
std::string ChooseRandomMap()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int level = (gen() % 3) + 1;
    return "./assets/levels/" + std::to_string(level) + ".txt";
}

#pragma region MainMenuScreen

void MainMenuScreen::Init()
{
	// Initialize main menu screen
}

// Handle main menu input
void MainMenuScreen::HandleInput()
{
    // Warrior
    if (window.keyPressed('I'))
    {
        gameManager.SetPlayerClass(PlayerClass::Warrior);
        gameManager.SetState(GameState::Game);
    }
    //Archer
    else if (window.keyPressed('O'))
    {
        gameManager.SetPlayerClass(PlayerClass::Archer);
        gameManager.SetState(GameState::Game);
    }
    // Mage
    else if (window.keyPressed('P'))
    {
        gameManager.SetPlayerClass(PlayerClass::Mage);
        gameManager.SetState(GameState::Game);
    }
}

void MainMenuScreen::Update(float deltaTime)
{
	// Update main menu screen
    UpdateFPSCounter(deltaTime);
}

// Draw all text
void MainMenuScreen::Draw(GamesEngineeringBase::Window& window)
{
	// Draw main menu screen
    Render_Background(window, 184, 143, 67);

    Render_String(window, gameManager.fontImage, "Main Menu", {350, 25});

    Render_String(window, gameManager.fontImage, "Select Class", { 50, 250 });
    Render_String(window, gameManager.fontImage, "I = Warrior", { 50, 300 });
    Render_String(window, gameManager.fontImage, "O = Archer", { 50, 350 });
    Render_String(window, gameManager.fontImage, "P = Mage", { 50, 400 });

    Render_String(window, gameManager.fontImage, "Controls", { 600, 250 });
    Render_String(window, gameManager.fontImage, "Move WASD", { 600, 300 });
    Render_String(window, gameManager.fontImage, "Attack E", { 600, 350 });
    Render_String(window, gameManager.fontImage, "Ability Q", { 600, 400 });
    Render_String(window, gameManager.fontImage, "Utility SHFT", { 600, 450 });
    Render_String(window, gameManager.fontImage, "End Game TAB", { 600, 500 });

    Render_String(window, gameManager.fontImage, "Select 1 - 5 to load screens", { 50, 600 });
    Render_String(window, gameManager.fontImage, "Esc to leave game", { 250, 700 });

    RenderFPSCounter(window);
}

void MainMenuScreen::Destroy()
{
	// Clean up resources for main menu screen
}
#pragma endregion

#pragma region GameplayScreen

void GameplayScreen::Init()
{
	// Initialize gameplay screen
	GameScreen::Init();

    quadtree = new Quadtree(0, Rect(Vector2(0, 0), Vector2(1024 * 3, 768 * 3)));
    tilemap = new Tilemap(Vector2(64, 64), 64, Vector2(0, 0), ChooseRandomMap(), "./assets/TileMapAtlas.png");

    switch (gameManager.GetPlayerClass())
    {
    case PlayerClass::Warrior:
        player = new PlayerWarrior(Vector2(100, 100), "./assets/Circle.png");
        break;
    case PlayerClass::Archer:
        player = new PlayerArcher(Vector2(100, 100), "./assets/Circle.png");
        break;
    case PlayerClass::Mage:
        player = new PlayerMage(Vector2(100, 100), "./assets/Circle.png");
        break;
    default:
        player = new PlayerWarrior(Vector2(100, 100), "./assets/Circle.png");
        break;
    }

    // If gamemanager's health storage has changed, set the player health to whatever this is
    // This is a very cheesy way of retaining health across levels due to the game architecture
    if (gameManager.GetPlayerHealth() != -1)
    {
        player->GetHealth().SetCurrentHealth(gameManager.GetPlayerHealth());
    }

    // Push all entities into the DynamicArray
    entities.Push(player);
    entities.Push(new SkeletonEnemy(Vector2(400, 400), player, "./assets/SkeletonEnemy.png", &projectileManager));
    entities.Push(new SlimeEnemy(Vector2(600, 110), player, "./assets/SlimeEnemy.png"));
    entities.Push(new SlimeEnemy(Vector2(100, 600), player, "./assets/SlimeEnemy.png"));
    entities.Push(new ZombieEnemy(Vector2(400, 150), player, "./assets/Enemy.png"));
    entities.Push(new ZombieEnemy(Vector2(100, 500), player, "./assets/Enemy.png"));
    entities.Push(new ZombieEnemy(Vector2(500, 300), player, "./assets/Enemy.png"));
    entities.Push(new ZombieEnemy(Vector2(500, 100), player, "./assets/Enemy.png"));
    entities.Push(new SkeletonEnemy(Vector2(900, 600), player, "./assets/SkeletonEnemy.png", &projectileManager));

    entities.Push(new SkeletonEnemy(Vector2(300, 850), player, "./assets/SkeletonEnemy.png", &projectileManager));

    camera = new Camera({ 0, 0 }, Vector2(window.getWidth(), window.getHeight()));
    camera->SetWorldBounds({ 0, 0 }, Vector2(1024.0f * 2, 768.0f * 2));

    levelTimer = maxTime;
}

void GameplayScreen::HandleInput()
{
	// Handle input for gameplay screen
    // Check for input (key presses or window events)

    // Player movement
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

    // Set input functions based off of if the player presses specific keys
    player->SetattackInput(window.keyPressed('E'));
    player->SetAbilityInput(window.keyPressed('Q'));
    player->SetUtilityInput(window.keyPressed(VK_SHIFT));

    // Force a game over if tab is pressed
    if (window.keyPressed(VK_TAB))
    {
        gameManager.SetState(GameState::GameOver);
    }
}

void GameplayScreen::Update(float deltaTime)
{
	// Update gameplay screen

    // If the player has died, change to game over and set gamemanager's health storage to default
    if (player->GetHealth().IsDead())
    {
        gameManager.SetPlayerHealth(-1.0f);
        gameManager.SetState(GameState::GameOver);
        return;
    }

    // Reduce remaining time on level, and move to tally screen if the player survived
    if (levelTimer > 0)
    {
        levelTimer -= deltaTime;
        if (levelTimer <= 0)
        {
            gameManager.SetPlayerHealth(player->GetHealth().GetCurrentHealth());
            gameManager.SetState(GameState::TallyScreen);
            return;
        }
    }

    // Update fps and score
	UpdateFPSCounter(deltaTime);
	gameManager.GetScoreManager().Update(deltaTime);

    // Update entities and store them in the quadtree
    quadtree->Clear();
    for (int i = 0; i < entities.GetSize(); i++)
    {
        entities[i]->Update(deltaTime, *tilemap);

        CollisionObject object(entities[i], entities[i]->GetBoundingBox());
        quadtree->Insert(object);
    }

    // run collisionmanager once all entities are in the quadtree
    collisionManager.Update(*quadtree, entities);

    // Update projectiles
    projectileManager.Update(deltaTime, *tilemap);
    projectileManager.CheckCollisions(entities, *quadtree, gameManager.GetScoreManager());

    // Move camera and set the bounds of the quadtree to the cam pos for next frame
    camera->Follow(*player);
    quadtree->SetBounds(Rect(camera->GetPosition(), Vector2(camera->GetPosition().x + window.getWidth(), camera->GetPosition().y + window.getHeight())));

    // Check whether a player input function must be made
    if (player->ConsumeMeleeRequest())
    {
        player->MeleeAttack(*quadtree, entities, gameManager.GetScoreManager(), projectileManager);
    }
    if (player->ConsumeAbilityRequest())
    {
        player->Ability(entities, gameManager.GetScoreManager());
    }
    if (player->ConsumeUtilityRequest())
    {
        player->Utility();
    }
}

// Draw stuff!
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
    //Render_Background(window);

    tilemap->Draw(window, *camera);

    projectileManager.Draw(window, *camera);

    for (int i = 0; i < entities.GetSize(); i++)
    {
        entities[i]->Draw(window, *camera);
    }

    //Render_String(window, fontImage, "sjdnsjdjsdhjsdhsjdhjsdhjsdhjshdjhdsjhshdjsd", Vector2(0, 0));
	RenderFPSCounter(window);
    Render_Number(window, gameManager.fontImage, (int)player->GetHealth().GetCurrentHealth(), Vector2(32, 0), 32);
    Render_Number(window, gameManager.fontImage, (int)player->GetMana(), Vector2(32, 32), 32);
    std::string timeRemaining = "Time: " + std::to_string((int)levelTimer);
    Render_String(window, gameManager.fontImage, timeRemaining.c_str(), Vector2(32, 768 - 32), 32);
}

// Clear all resources used in this screen
void GameplayScreen::Destroy()
{
	// Clean up resources for gameplay screen
    projectileManager.Clear();

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

    delete camera;
    delete tilemap;

    camera = nullptr;
    tilemap = nullptr;
    player = nullptr;
}

#pragma endregion

#pragma region GameplayScreenInfinite

// !! GAMEPLAYSCREENINFINITE IS THE SAME AS GAMEPLAYSCREEN, I'M TOO LAZY TO CODE COMMENT BUT ITS ALL THE SAME !!
void GameplayScreenInfinite::Init()
{
    GameScreen::Init();
    // Initialize gameplay screen infinite
    quadtree = new Quadtree(0, Rect(Vector2(0, 0), Vector2(1024 * 3, 768 * 3)));
    tilemap = new Tilemap(
        Vector2(32, 32),
        32,
        Vector2(200, 200),
        ChooseRandomMap(), "./assets/TileMapAtlasOLD.png");

    switch (gameManager.GetPlayerClass())
    {
    case PlayerClass::Warrior:
        player = new PlayerWarrior(Vector2(100, 100), "./assets/Circle.png");
        break;
    case PlayerClass::Archer:
        player = new PlayerArcher(Vector2(100, 100), "./assets/Circle.png");
        break;
    case PlayerClass::Mage:
        player = new PlayerMage(Vector2(100, 100), "./assets/Circle.png");
        break;
    default:
        player = new PlayerWarrior(Vector2(100, 100), "./assets/Circle.png");
        break;
    }
    entities.Push(player);
    entities.Push(new ZombieEnemy(Vector2(500, 300), player, "./assets/Enemy.png"));
    entities.Push(new ZombieEnemy(Vector2(600, 110), player, "./assets/Enemy.png"));
    entities.Push(new SlimeEnemy(Vector2(100, 600), player, "./assets/SlimeEnemy.png"));
    entities.Push(new SlimeEnemy(Vector2(400, 150), player, "./assets/SlimeEnemy.png"));
    entities.Push(new ZombieEnemy(Vector2(100, 500), player, "./assets/Enemy.png"));
    entities.Push(new SkeletonEnemy(Vector2(500, 200), player, "./assets/SkeletonEnemy.png", &projectileManager));
    entities.Push(new SkeletonEnemy(Vector2(500, 100), player, "./assets/SkeletonEnemy.png", &projectileManager));
    entities.Push(new ZombieEnemy(Vector2(800, 600), player, "./assets/Enemy.png"));
    entities.Push(new SkeletonEnemy(Vector2(300, 850), player, "./assets/SkeletonEnemy.png", &projectileManager));

    camera = new Camera({ 0, 0 }, Vector2(window.getWidth(), window.getHeight()));
    camera->SetWorldBounds({ -10000, -10000 }, { 10000, 10000 });

    levelTimer = maxTime;
}

void GameplayScreenInfinite::HandleInput()
{
	// Handle input for gameplay screen infinite
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

    if (window.keyPressed(VK_TAB))
    {
        gameManager.SetState(GameState::GameOver);
    }

    player->SetInput(input);
    player->SetattackInput(window.keyPressed('E'));
    player->SetAbilityInput(window.keyPressed('Q'));
    player->SetUtilityInput(window.keyPressed(VK_SHIFT));
}

void GameplayScreenInfinite::Update(float deltaTime)
{
	// Update gameplay screen infinite

    if (player->GetHealth().IsDead())
    {
        gameManager.SetPlayerHealth(-1.0f);
        gameManager.SetState(GameState::GameOver);
        return;
    }

    if (levelTimer > 0)
    {
        levelTimer -= deltaTime;
        if (levelTimer <= 0)
        {
            gameManager.SetPlayerHealth(player->GetHealth().GetCurrentHealth());
            gameManager.SetState(GameState::TallyScreen);
            return;
        }
    }

    UpdateFPSCounter(deltaTime);
    gameManager.GetScoreManager().Update(deltaTime);

    quadtree->Clear();
    for (int i = 0; i < entities.GetSize(); i++)
    {
        entities[i]->Update(deltaTime, *tilemap);

        CollisionObject object(entities[i], entities[i]->GetBoundingBox());
        quadtree->Insert(object);
    }

    collisionManager.Update(*quadtree, entities);

    projectileManager.Update(deltaTime, *tilemap);
    projectileManager.CheckCollisions(entities, *quadtree, gameManager.GetScoreManager());

    camera->Follow(*player);
    quadtree->SetBounds(Rect(camera->GetPosition(), Vector2(camera->GetPosition().x + window.getWidth(), camera->GetPosition().y + window.getHeight())));

    if (player->ConsumeMeleeRequest())
    {
        player->MeleeAttack(*quadtree, entities, gameManager.GetScoreManager(), projectileManager);
    }
    if (player->ConsumeAbilityRequest())
    {
        player->Ability(entities, gameManager.GetScoreManager());
    }
    if (player->ConsumeUtilityRequest())
    {
        player->Utility();
    }
}

void GameplayScreenInfinite::Draw(GamesEngineeringBase::Window& window)
{
	// Draw gameplay screen infinite
    Render_Background(window);

    tilemap->Draw(window, *camera);

    projectileManager.Draw(window, *camera);

    for (int i = 0; i < entities.GetSize(); i++)
    {
        entities[i]->Draw(window, *camera);
    }

    //Render_String(window, fontImage, "sjdnsjdjsdhjsdhsjdhjsdhjsdhjshdjhdsjhshdjsd", Vector2(0, 0));
    RenderFPSCounter(window);
    Render_Number(window, gameManager.fontImage, (int)player->GetHealth().GetCurrentHealth(), Vector2(32, 0), 32);
    Render_Number(window, gameManager.fontImage, (int)player->GetMana(), Vector2(32, 32), 32);
    std::string timeRemaining = "Time: " + std::to_string((int)levelTimer);
    Render_String(window, gameManager.fontImage, timeRemaining.c_str(), Vector2(32, 768 - 32), 32);
}

void GameplayScreenInfinite::Destroy()
{
	// Clean up resources for gameplay screen infinite
    projectileManager.Clear();

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

    delete camera;
    delete tilemap;

    camera = nullptr;
    tilemap = nullptr;
    player = nullptr;
}

#pragma endregion

#pragma region TallyScreen

void TallyScreen::Init()
{
	// Initialize tally screen
	GameScreen::Init();

    // Get the scoremanager from gamemanager
    ScoreManager& scoreManager = gameManager.GetScoreManager();

    // Store all tracked stuff
	score = scoreManager.GetScore();
	kills = scoreManager.GetKills();
	timeAlive = scoreManager.GetTimeAlive();

    // Divide time alive into minutes and seconds
	int minutes = (int)(timeAlive / 60);
	int seconds = (int)(timeAlive) % 60;

    // Set textstrings
	scoreString = "Score: " + std::to_string(score);
	killsString = "Kills: " + std::to_string(kills);
	timeAliveString = "Time Alive: " + std::to_string(minutes) + "m:" + std::to_string(seconds) + "s";
}

void TallyScreen::HandleInput()
{
	// Handle input for tally screen
	if (window.keyPressed('M'))
	{
        gameManager.GetScoreManager().Reset();
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
    UpdateFPSCounter(deltaTime);
}

void TallyScreen::Draw(GamesEngineeringBase::Window& window)
{
	// Draw tally screen
    Render_String(window, gameManager.fontImage, scoreString.c_str(), {50, 100});
    Render_String(window, gameManager.fontImage, killsString.c_str(), {50, 200});
    Render_String(window, gameManager.fontImage, timeAliveString.c_str(), {50, 300});
	Render_String(window, gameManager.fontImage, "Tally Screen", {350, 25});

    Render_String(window, gameManager.fontImage, "SPACE for next level", { 50, 500 });
    Render_String(window, gameManager.fontImage, "M to quit to menu", { 50, 600 });

    RenderFPSCounter(window);
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

    ScoreManager& scoreManager = gameManager.GetScoreManager();

    score = scoreManager.GetScore();
    kills = scoreManager.GetKills();
    timeAlive = scoreManager.GetTimeAlive();

    int minutes = (int)(timeAlive / 60);
    int seconds = (int)(timeAlive) % 60;

    scoreString = "Score: " + std::to_string(score);
    killsString = "Kills: " + std::to_string(kills);
    timeAliveString = "Time Alive: " + std::to_string(minutes) + "m:" + std::to_string(seconds) + "s";

    scoreManager.Reset();
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
    UpdateFPSCounter(deltaTime);
}

void GameOverScreen::Draw(GamesEngineeringBase::Window& window)
{
	// Draw game over screen
	Render_Background(window, 128, 0, 0);

	Render_String(window, gameManager.fontImage, "Game Over!", { 350, 25 });

    Render_String(window, gameManager.fontImage, scoreString.c_str(), { 50, 100 });
    Render_String(window, gameManager.fontImage, killsString.c_str(), { 50, 200 });
    Render_String(window, gameManager.fontImage, timeAliveString.c_str(), { 50, 300 });

    Render_String(window, gameManager.fontImage, "R to restart game", { 50, 500 });
    Render_String(window, gameManager.fontImage, "M to quit to menu", {50, 600});

    RenderFPSCounter(window);
}

void GameOverScreen::Destroy()
{
	// Clean up resources for game over screen
}

#pragma endregion