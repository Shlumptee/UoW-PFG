#include <iostream>

#include "GameLoop.h"
#include "Vector2.h"
#include "Enemy.h"
#include "Renderer.h"

// OLD - being replaced by GameManager.cpp
// I wanted to leave this as-is for now just in case anything goes horrifically wrong
// Should delete once GameManager.cpp is fully implemented and working

int GameLoop::Init()
{
    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    canvas.create(1024, 768, "Blast'ems++");
    isRunning = true; // Variable to control the main loop's running state.

    quadtree = new Quadtree(0, Rect(Vector2(0, 0), Vector2(1024*3, 768*3)));
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
    entities.Push(new Enemy(Vector2(100, 150), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(100, 50), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(50, 100), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(500, 100), player, "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(800, 600), player, "./assets/Enemy.png"));

    entities.Push(new Enemy(Vector2(300, 850), player, "./assets/Enemy.png"));

    camera = new Camera({ 0, 0 }, Vector2(canvas.getWidth(), canvas.getHeight()));
    camera->SetWorldBounds({ 0, 0 }, Vector2(3200.0f, 2400.0f));

    fontImage.load("./assets/text2.png");

    return 1;
}

void GameLoop::HandleInput()
{
    // Check for input (key presses or window events)
    if (canvas.keyPressed(VK_ESCAPE))
    {
        isRunning = false;
    }

	input.Set(0, 0);
    if (canvas.keyPressed('A'))
    {
        input.x -= 1.0f;
    }
    if (canvas.keyPressed('D'))
    {
        input.x += 1.0f;
    }
    if (canvas.keyPressed('S'))
    {
        input.y += 1.0f;
    }
    if (canvas.keyPressed('W'))
    {
        input.y -= 1.0f;
    }

    player->SetInput(input);
    player->SetMeleeInput(canvas.keyPressed('E'));
}

void GameLoop::Update()
{
	deltaTime = timer.dt();

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
		entities[i]->Update(deltaTime, *tilemap);

        CollisionObject object(entities[i], entities[i]->GetBoundingBox());
        quadtree->Insert(object);
	}

    /*for (int i = 0; i < entities.GetSize(); i++)
    {
        DynamicArray<CollisionObject> candidates;

        quadtree->Retrieve(candidates, entities[i]->GetBoundingBox());

        std::cout << "Entity " << i << " candidates: " << candidates.GetSize() << "\n";
    }*/

    collisionManager.Update(*quadtree, entities);

    camera->Follow(*player);
    quadtree->SetBounds(Rect(Vector2(camera->GetPosition().x - (canvas.getWidth() / 2), camera->GetPosition().y - (canvas.getHeight() / 2)), Vector2(camera->GetPosition().x + (canvas.getWidth() / 2), camera->GetPosition().y + (canvas.getHeight() / 2))));

    if (player->ConsumeMeleeRequest())
    {
        //player->MeleeAttack(*quadtree, entities);
    }
}

void GameLoop::Draw()
{
    for (int i = 0; i < canvas.getHeight(); i++)
    {
        for (int j = 0; j < canvas.getWidth(); j++)
        {
            canvas.draw(j, i, 0, 0, 128);
        }
    }

    tilemap->Draw(canvas, *camera);

	for (int i = 0; i < entities.GetSize(); i++)
	{
		entities[i]->Draw(canvas, *camera);
	}

    Render_Number(canvas, fontImage, fps, Vector2(1024 - 32*3.5f, 768 - 32), 32);
	Render_Number(canvas, fontImage, (int)player->GetHealth().GetCurrentHealth(), Vector2(32, 0), 32);
}

void GameLoop::Destroy()
{
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