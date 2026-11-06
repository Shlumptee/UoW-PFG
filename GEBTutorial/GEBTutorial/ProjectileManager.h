#pragma once

#include "Projectile.h"
#include "Vector2.h"
#include "Quadtree.h"

// Class to manage projectiles, uses a stack to store unused projectiles (fast efficiency + gets latest projectile), and a dynamic array for current projectiles
// Some of this is taken from an older C++ project (mainly the concept + data types + enums), and rearranged to fit this project
// SHOULD PROBABLY LIMIT STACK TO AVOID TOO MUCH MEMORY BEING USED!!!!
// NEED TO PASS THROUGH DAMAGE FROM ENTITIES CALLING THE PROJECTILE
class ProjectileManager
{
public:
	ProjectileManager() = default;
	~ProjectileManager()
	{
		Clear();
	}

	// Shoot out the projectile with a starting position and a direction
	// Need the owner to make sure they don't get damaged
	void FireProjectile(Vector2 _position, Vector2 _direction, float _damage, ProjectileOwner _owner, const std::string& _imagePath)
	{
		Projectile* projectile = nullptr;

		// Check stack for available projectiles, if empty create a new projectile
		// SHOULD LIMIT STACK AND INIT WITH A SET NUMBER OF PROJECTILES!
		if (!inactiveProjectiles.IsEmpty())
		{
			projectile = inactiveProjectiles.Pop();
		}
		else
		{
			projectile = new Projectile(_position, _imagePath);
		}

		// Activate individual projectile
		projectile->Activate(_position, _direction, 256.0f, _damage, 2.5f, _owner);
		activeProjectiles.Push(projectile);
	}

	// Update all active projectiles
	void Update(float deltaTime, Tilemap& tilemap)
	{
		for (int i = 0; i < activeProjectiles.GetSize(); i++)
		{
			activeProjectiles[i]->Update(deltaTime, tilemap);
		}

		CheckProjectileRecycle();
	}
	// Draw all active projectiles to the screen
	void Draw(GamesEngineeringBase::Window& window, Camera& camera)
	{
		for (int i = 0; i < activeProjectiles.GetSize(); i++)
		{
			activeProjectiles[i]->Draw(window, camera);
		}
	}

	// SHOULD FIGURE OUT A WAY TO FIT INTO COLLISION MANAGER AND QUADTREE, BAD IMPLEMENTATION HERE!!!
	void CheckCollisions(DynamicArray<VisualEntity*>& entities, Quadtree& quadtree, ScoreManager& scoreManager)
	{
		// Get all the active projectiles (inactive not needed as they shouldn't show in game)
		for (int i = 0; i < activeProjectiles.GetSize(); i++)
		{
			Projectile* projectile = activeProjectiles[i];

			if (!projectile->IsActive())
			{
				continue;
			}

			// Loop through every entity stored in the dynamic array and check whether the projectile is not on the same team
			for (int j = 0; j < entities.GetSize(); j++)
			{
				VisualEntity* target = entities[j];

				if (target == nullptr)
				{
					continue;
				}

				if (target->GetTeam() == projectile->GetTeam())
				{
					continue;
				}

				if ((projectile->GetOwner() == ProjectileOwner::Enemy && target->GetTeam() == Team::Enemy) || (projectile->GetOwner() == ProjectileOwner::Player && target->GetTeam() == Team::Player))
				{
					continue;
				}

				// Check if projectile intersects with the target entity, if it does deal damage and deactivate projectile
				if (quadtree.Intersects(projectile->GetBoundingBox(), target->GetBoundingBox()))
				{
					Vector2 direction(target->GetPosition().x - projectile->GetPosition().x, target->GetPosition().y - projectile->GetPosition().y);

					target->ApplyKnockback(direction, 250.0f);

					bool wasAlive = !target->GetHealth().IsDead();
					int previousHealth = target->GetHealth().GetCurrentHealth();

					target->GetHealth().RemoveHealth(projectile->GetDamage());

					int damageDealt = previousHealth - target->GetHealth().GetCurrentHealth();

					if (damageDealt > 0)
					{
						scoreManager.AddScore(damageDealt * 0.75f);
					}
					if (wasAlive && target->GetHealth().IsDead())
					{
						scoreManager.AddKill(150);
						std::cout << "Enemy killed! Score: " << scoreManager.GetScore() << std::endl;
					}

					projectile->Deactivate();
					break;
				}
			}
		}

		CheckProjectileRecycle();
	}

	// Clear projectiles from stack and dynamicarray
	void Clear()
	{
		for (int i = 0; i < activeProjectiles.GetSize(); i++)
		{
			delete activeProjectiles[i];
		}

		activeProjectiles.Clear();

		while (!inactiveProjectiles.IsEmpty())
		{
			Projectile* projectile = inactiveProjectiles.Pop();
			delete projectile;
		}
	}

private:
	DynamicArray<Projectile*> activeProjectiles;
	Stack<Projectile*> inactiveProjectiles;

	// Check for if a projectile must be recycled back into the stack
	void CheckProjectileRecycle()
	{
		int i = 0;
		while (i < activeProjectiles.GetSize())
		{
			Projectile* projectile = activeProjectiles[i];

			if (!projectile->IsActive())
			{
				inactiveProjectiles.Push(projectile);
				activeProjectiles.Remove(i);
			}
			else
			{
				i++;
			}
		}
	}
};