#include <iostream>

#include "GameLoop.h"
#include "Vector2.h"
#include "Enemy.h"
#include "Renderer.h"

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

    entities.Push(new Enemy(Vector2(500, 300), "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(600, 110), "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(100, 600), "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(100, 150), "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(100, 50), "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(50, 100), "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(500, 100), "./assets/Enemy.png"));
    entities.Push(new Enemy(Vector2(800, 600), "./assets/Enemy.png"));

    entities.Push(new Enemy(Vector2(300, 850), "./assets/Enemy.png"));

    camera = new Camera(Vector2(canvas.getWidth(), canvas.getHeight()));
    camera->SetWorldBounds(Vector2(3200.0f, 2400.0f));

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
}

void GameLoop::Update()
{
	deltaTime = timer.dt();

    quadtree->Clear();
	for (int i = 0; i < entities.GetSize(); i++)
	{
		entities[i]->Update(deltaTime);

        CollisionObject object(entities[i], entities[i]->GetBoundingBox());
        quadtree->Insert(object);
	}

    collisionManager.Update(*quadtree, entities);

    camera->Follow(*player);
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