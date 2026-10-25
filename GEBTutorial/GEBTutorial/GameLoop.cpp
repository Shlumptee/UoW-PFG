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

    quadtree = new Quadtree(0, Rect(Vector2(0, 0), Vector2(canvas.getWidth(), canvas.getHeight())));

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

    for (int i = 0; i < entities.GetSize(); i++)
    {
        VisualEntity* entity = entities[i];

        DynamicArray<CollisionObject> candidates;

        quadtree->Retrieve(candidates, entity->GetBoundingBox());

        //std::cout << "Entity " << i << " has " << candidates.GetSize() << " potential collision candidates.\n";

        for (int c = 0; c < candidates.GetSize(); c++)
        {
            CollisionObject& candidate = candidates[c];

            if (candidate.entity == nullptr || candidate.entity == entity)
            {
                continue;
            }

            int otherIndex = -1;

            for (int j = 0; j < entities.GetSize(); j++)
            {
                if (entities[j] == candidate.entity)
                {
                    otherIndex = j;
                    break;
                }
            }

            if (otherIndex <= i)
            {
                continue;
            }

            if (quadtree->Intersects(entity->GetBoundingBox(), candidate.boundingBox))
            {
                std::cout << "Collision detected between entity " << i << " and entity " << otherIndex << std::endl;
            }
        }
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

    for (int i = 0; i < canvas.getHeight(); i++)
    {
        canvas.draw(70, i, 255, 255, 255);
    }

    /*image.load("./assets/Circle.png");
    for (int i = 0; i < image.width; i++)
    {
        for (int j = 0; j < image.height; j++)
        {
            if (!image.alphaAt(i, j))
            {
                continue;
            }

            unsigned char* pixel = image.at(i, j);
            canvas.draw(100 + i, 100 + j, red, green, blue);
        }
    }*/

	for (int i = 0; i < entities.GetSize(); i++)
	{
		entities[i]->Draw(canvas);
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