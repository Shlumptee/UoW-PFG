#pragma once
#include <iostream>

#include "GamesEngineeringBase.h"
#include "Entity.h"

#include "Shape.h"
#include "Vector2.h"
#include "Quadtree.h"
#include "ScoreManager.h"

class Player : public VisualEntity
{
public:
	Player(const Vector2 position, std::string filePath) : VisualEntity(position, filePath)
	{
		std::cout << "Init Player" << std::endl;
	}


	virtual void Update(float deltaTime, Tilemap& tilemap) override
	{
		VisualEntity::Update(deltaTime, tilemap);

		if (inputAxis.x != 0 || inputAxis.y != 0)
		{
			orientation = inputAxis;
		}


		previousPosition = GetPosition();
		Vector2 movement = (inputAxis * moveSpeed) + GetKnockbackVelocity();
		TileMove(movement, deltaTime, tilemap);

		UpdateKnockback(deltaTime);

		boundingBox.Set(GetBoundingBox());

		if (meleeInput && !previousMeleeInput)
		{
			requestingMelee = true;
		}
		if (abilityInput && !previousAbilityInput)
		{
			requestingAbility = true;
		}

		previousMeleeInput = meleeInput;
		previousAbilityInput = abilityInput;
	}
	virtual void OnCollision(VisualEntity* other) override
	{
		VisualEntity::OnCollision(other);
		//SetPosition(previousPosition);

		Rect otherBoundingBox = other->GetBoundingBox();
		Vector2 currentPosition = GetPosition();

		Rect previousBounds = Rect(previousPosition, Vector2(previousPosition.x + GetWidth(), previousPosition.y + GetHeight()));

		bool horizontalOverlap = currentPosition.x < otherBoundingBox.max.x && currentPosition.x + GetWidth() > otherBoundingBox.min.x;
		bool verticalOverlap = currentPosition.y < otherBoundingBox.max.y && currentPosition.y + GetHeight() > otherBoundingBox.min.y;

		if (!horizontalOverlap || !verticalOverlap)
		{
			return;
		}

		bool horizontalCollision = previousBounds.max.x <= otherBoundingBox.min.x || previousBounds.min.x >= otherBoundingBox.max.x;
		bool verticalCollision = previousBounds.max.y <= otherBoundingBox.min.y || previousBounds.min.y >= otherBoundingBox.max.y;

		if (horizontalCollision)
		{
			SetPosition(Vector2(previousPosition.x, currentPosition.y));

		}
		if (verticalCollision)
		{
			SetPosition(Vector2(currentPosition.x, previousPosition.y));
		}
	}
	virtual void Draw(GamesEngineeringBase::Window& window, Camera& camera) override
	{
		VisualEntity::Draw(window, camera);
		Render_Rect(window, boundingBox, camera.GetPosition(), GetColliding() ? 0 : 255, GetColliding() ? 255 : 0, 0, 1);
	}

	virtual void MeleeAttack(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager)
	{
		Rect attackBounds;
		Rect playerBounds = GetBoundingBox();

		if (orientation.x > 0)
		{
			// Attacking to the right
			attackBounds.Set({ playerBounds.max.x, playerBounds.min.y }, { playerBounds.max.x + meleeRange, playerBounds.max.y });
		}
		else if (orientation.x < 0)
		{
			// Attacking to the left
			attackBounds.Set({ playerBounds.min.x - meleeRange, playerBounds.min.y }, { playerBounds.min.x, playerBounds.max.y });
		}
		else if (orientation.y > 0)
		{
			// Attacking downwards
			attackBounds.Set({ playerBounds.min.x, playerBounds.max.y }, { playerBounds.max.x, playerBounds.max.y + meleeRange});
		}
		else
		{
			// Attacking upwards
			attackBounds.Set({ playerBounds.min.x, playerBounds.min.y - meleeRange }, { playerBounds.max.x, playerBounds.min.y });
		}

		for (int i = 0; i < entities.GetSize(); i++)
		{
			VisualEntity* target = entities[i];

			if (target == nullptr || target == this)
			{
				continue;
			}

			if (!target->CanBeDamaged())
			{
				continue;
			}

			if (quadtree.Intersects(attackBounds, target->GetBoundingBox()))
			{
				Vector2 direction(target->GetPosition().x - GetPosition().x, target->GetPosition().y - GetPosition().y);

				target->ApplyKnockback(direction, 250.0f);

				bool wasAlive = !target->GetHealth().IsDead();
				int previousHealth = target->GetHealth().GetCurrentHealth();

				target->GetHealth().RemoveHealth(meleeDamage);

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

				//std::cout << "Enemy Hit! Health: " << target->GetHealth().GetCurrentHealth() << std::endl;
			}
		}
	}

	virtual void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager)
	{
		std::cout << "Only calling here" << std::endl;
	}

	virtual bool CanBeDamaged() const override
	{
		return true;
	}
	bool ConsumeMeleeRequest()
	{
		if (!requestingMelee)
		{
			return false;
		}

		requestingMelee = false;
		return true;
	}
	bool ConsumeAbilityRequest()
	{
		if (!requestingAbility)
		{
			return false;
		}

		requestingAbility = false;
		return true;
	}
	void SetInput(const Vector2& _inputAxis)
	{
		inputAxis = _inputAxis;
	}
	void SetMeleeInput(bool _meleeInput)
	{
		meleeInput = _meleeInput;
	}
	void SetAbilityInput(bool _abilityInput)
	{
		abilityInput = _abilityInput;
	}
	void SetMoveSpeed(float _moveSpeed)
	{
		moveSpeed = _moveSpeed;
	}
	void SetMeleeDamage(float _meleeDamage)
	{
		meleeDamage = _meleeDamage;
	}
	const Vector2& GetInput() const
	{
		return inputAxis;
	}
	int GetCurrentHealth()
	{
		return GetHealth().GetCurrentHealth();
	}
	float GetMoveSpeed()
	{
		return moveSpeed;
	}
	float GetMeleeDamage()
	{
		return meleeDamage;
	}
	
private:
	float moveSpeed = 128.0f;
	float meleeDamage = 25.0f;
	float meleeRange = 32.0f;

	bool requestingMelee = false;
	bool previousMeleeInput = false;
	bool meleeInput = false;
	float meleeCooldown = 0.5f;
	float meleeTimer = 0.0f;

	bool requestingAbility = false;
	bool previousAbilityInput = false;
	bool abilityInput = false;
	float abilityCooldown = 10.0f;
	float abilityTimer = 0.0f;

	Vector2 inputAxis;
	Vector2 orientation = { 0, 1 };
	Vector2 previousPosition;

	Rect boundingBox{ Vector2(GetPosition().x , GetPosition().y), Vector2(GetPosition().x + GetWidth(), GetPosition().y + GetHeight())};
};

class PlayerWarrior : public Player
{
public:
	PlayerWarrior(const Vector2 _position, std::string _filePath) : Player(_position, _filePath)
	{
		GetHealth().SetMaxHealth(150);
		
	}
	void Update(float deltaTime, Tilemap& tilemap) override
	{
		// Implement specific update logic for PlayerWarrior
		Player::Update(deltaTime, tilemap);
	}
};

class PlayerArcher : public Player
{
public:
	PlayerArcher(const Vector2 _position, std::string _filePath) : Player(_position, _filePath)
	{
		std::cout << "Init Archer" << std::endl;
		GetHealth().SetMaxHealth(85);
		SetMeleeDamage(20.0f);
		SetMoveSpeed(192.0f);
	}

	// Ricochet ability for archer - bounces an attack between up to a max amount of enemies
	// Uses queues to add entities within range, and a linked list to ensure that an enemy that has been previously visited cannot be visited again
	void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager) override
	{
		Queue<VisualEntity*> targets;
		LinkedList<VisualEntity*> visitedTargets;

		Vector2 searchPosition(GetPosition().x + GetWidth() * 0.5f, GetPosition().y + GetHeight() * 0.5f);

		const float range = 200.0f;
		const float damage = GetMeleeDamage() * 0.75f;
		const int maxTargets = 5;

		// Loop until attained all targets nearby
		for (int i = 0; i < maxTargets; i++)
		{
			VisualEntity* closestTarget = nullptr;
			float closestDistance = 999.0f;

			// Loop through all entities in the array (not optimal), and find the closest enemy
			for (int j = 0; j < entities.GetSize(); j++)
			{
				VisualEntity* candidate = entities[j];

				if (candidate == nullptr || candidate == this)
				{
					continue;
				}

				if (!candidate->CanBeDamaged())
				{
					continue;
				}

				if (visitedTargets.Contains(candidate))
				{
					continue;
				}

				Vector2 candidatePosition(candidate->GetPosition().x + candidate->GetWidth() * 0.5f, candidate->GetPosition().y + candidate->GetHeight() * 0.5f);
				Vector2 positionDifference = candidatePosition - searchPosition;
				float distance = positionDifference.Length();

				if (distance <= range && distance <= closestDistance)
				{
					closestDistance = distance;
					closestTarget = candidate;
				}
			}

			if (closestTarget == nullptr)
			{
				break;
			}

			// Add closest target to both the queue and list
			targets.Enqueue(closestTarget);
			visitedTargets.Add(closestTarget);

			// Reset search pos to the closest target so it can search for more nearby itself
			searchPosition.Set(closestTarget->GetPosition().x + closestTarget->GetWidth() * 0.5f, closestTarget->GetPosition().y + closestTarget->GetHeight() * 0.5f);
		}

		// Loop until the queue is empty, do usual damage calculations
		while (!targets.IsEmpty())
		{
			VisualEntity* target = targets.Dequeue();

			if (target == nullptr || target->GetHealth().IsDead())
			{
				continue;
			}

			Vector2 direction(target->GetPosition().x - GetPosition().x, target->GetPosition().y - GetPosition().y);

			target->ApplyKnockback(direction, 250.0f);

			bool wasAlive = !target->GetHealth().IsDead();
			int previousHealth = target->GetHealth().GetCurrentHealth();

			target->GetHealth().RemoveHealth(damage);

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
		}
	}
};

class PlayerMage : public Player
{
public:
	PlayerMage(const Vector2 _position, std::string _filePath) : Player(_position, _filePath)
	{
		GetHealth().SetMaxHealth(100);
		SetMeleeDamage(20.0f);
		SetMoveSpeed(160.0f);
	}

	// Magic Ability - Get all the nearby enemies and pick out 3 with the highest health -> deal damage (no knockback)
	void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager) override
	{
		DynamicArray<VisualEntity*> targets;

		Vector2 searchPosition(GetPosition().x + GetWidth() * 0.5f, GetPosition().y + GetHeight() * 0.5f);

		const float range = 250.0f;
		const float damage = GetMeleeDamage() * 1.5f;
		const int maxTargets = 3;

		// Loop through all entities in the array (probably not too efficient) to find the ones within range of the player
		for (int i = 0; i < entities.GetSize(); i++)
		{
			VisualEntity* candidate = entities[i];

			if (candidate == nullptr || candidate == this)
			{
				continue;
			}

			if (!candidate->CanBeDamaged())
			{
				continue;
			}

			// Get difference between current candidate and the player
			Vector2 candidatePosition(candidate->GetPosition().x + candidate->GetWidth() * 0.5f, candidate->GetPosition().y + candidate->GetHeight() * 0.5f);
			Vector2 positionDifference = candidatePosition - searchPosition;
			float distance = positionDifference.Length();

			if (distance <= range)
			{
				targets.Push(candidate);
			}
		}

		// Using selection sorting as it shouldn't be taking too many targets at once, should be quick to sort
		// Taken from https://www.geeksforgeeks.org/dsa/selection-sort-algorithm-2/
		// Sort by highest health
		for (int i = 0; i < targets.GetSize() - 1; i++)
		{
			int highestIndex = i;

			for (int j = i + 1; j < targets.GetSize(); j++)
			{
				if (targets[j]->GetHealth().GetCurrentHealth() > targets[highestIndex]->GetHealth().GetCurrentHealth())
				{
					highestIndex = j;
				}
			}

			if (highestIndex != i)
			{
				VisualEntity* temp = targets[i];
				targets[i] = targets[highestIndex];
				targets[highestIndex] = temp;
			}
		}

		// Clamp the amount of targets being hit to the value of maxTargets
		int hitTargets = targets.GetSize();

		if (hitTargets > maxTargets)
		{
			hitTargets = maxTargets;
		}

		// Loop through and do usual damage routine
		// Should move all this stuff to a universal function in Player, it's used a lot
		for (int i = 0; i < hitTargets; i++)
		{
			VisualEntity* target = targets[i];

			if (target == nullptr || target->GetHealth().IsDead())
			{
				continue;
			}

			bool wasAlive = !target->GetHealth().IsDead();
			int previousHealth = target->GetHealth().GetCurrentHealth();

			target->GetHealth().RemoveHealth(damage);

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
		}
	}
};

//class Player
//{
//public:
//	Player(const GamesEngineeringBase::Window& _canvas)
//	{
//		canvas = _canvas;
//	}
//
//	void Init();
//	void HandleInput();
//	void Update();
//	void Draw();
//	void Destroy();
//
//	void MoveLeft()
//	{
//		position.x -= moveSpeed;
//		orientation = 3;
//
//		if (position.x < 0)
//		{
//			position.x = 0;
//		}
//	}
//
//	void MoveRight()
//	{
//		position.x += moveSpeed;
//		orientation = 1;
//
//		if (position.x > canvas.getWidth() - image.width)
//		{
//			position.x = canvas.getWidth() - image.width;
//		}
//	}
//
//	void MoveDown()
//	{
//		position.y += moveSpeed;
//		orientation = 2;
//
//		if (position.y > (canvas.getHeight() - image.height))
//		{
//			position.y = canvas.getHeight() - image.height;
//		}
//	}
//
//	void MoveUp()
//	{
//		position.y -= moveSpeed;
//		orientation = 0;
//
//		if (position.y < 0)
//		{
//			position.y = 0;
//		}
//	}
//
//	void MoveAbility()
//	{
//		if (abilityTimer.dt() < abilityCooldown)
//		{
//			return;
//		}
//
//		switch (orientation)
//		{
//		case 0:
//			position.y -= moveSpeed * 50;
//			if (position.y < 0)
//			{
//				position.y = 0;
//			}
//			break;
//		case 1:
//			position.x += moveSpeed * 50;
//			if (position.x > canvas.getWidth() - image.width)
//			{
//				position.x = canvas.getWidth() - image.width;
//			}
//			break;
//		case 2:
//			position.y += moveSpeed * 50;
//			if (position.y > (canvas.getHeight() - image.height))
//			{
//				position.y = canvas.getHeight() - image.height;
//			}
//			break;
//		case 3:
//			position.x -= moveSpeed * 50;
//			if (position.x < 0)
//			{
//				position.x = 0;
//			}
//			break;
//		default:
//			break;
//		}
//
//		abilityTimer.reset();
//	}
//
//private:
//	int moveSpeed = 1;
//	Vector2 position = Vector2(100, 100);
//	unsigned int orientation = 0;
//
//	float abilityCooldown = 1.0f;
//
//	GamesEngineeringBase::Window canvas;
//	GamesEngineeringBase::Image image;
//	GamesEngineeringBase::Timer abilityTimer;
//};

