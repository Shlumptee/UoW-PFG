#pragma once
#include <iostream>

#include "GamesEngineeringBase.h"
#include "Entity.h"

#include "Shape.h"
#include "Vector2.h"
#include "Quadtree.h"
#include "ScoreManager.h"
#include "ProjectileManager.h"

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

		requestingAbility = false;
		requestingAttack = false;
		requestingUtility = false;

		if (inputAxis.x != 0 || inputAxis.y != 0)
		{
			orientation = inputAxis;
		}

		abilityTimer -= deltaTime;
		if (abilityTimer < 0)
		{
			abilityTimer = 0;
		}

		attackTimer -= deltaTime;
		if (attackTimer < 0)
		{
			attackTimer = 0;
		}

		utilityTimer -= deltaTime;
		if (utilityTimer < 0)
		{
			utilityTimer = 0;
		}

		previousPosition = GetPosition();
		Vector2 movement = (inputAxis * moveSpeed) + GetKnockbackVelocity();
		TileMove(movement, deltaTime, tilemap);

		UpdateKnockback(deltaTime);

		boundingBox.Set(GetBoundingBox());

		if (attackInput && !previousAttackInput)
		{
			requestingAttack = true;
		}
		if (abilityInput && !previousAbilityInput)
		{
			requestingAbility = true;
		}
		if (utilityInput && !previousUtilityInput)
		{
			requestingUtility = true;
		}

		previousAttackInput = attackInput;
		previousAbilityInput = abilityInput;
		previousUtilityInput = utilityInput;
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

	virtual void MeleeAttack(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager, ProjectileManager& projectileManager)
	{
		Rect attackBounds;
		Rect playerBounds = GetBoundingBox();

		if (orientation.x > 0)
		{
			// Attacking to the right
			attackBounds.Set({ playerBounds.max.x, playerBounds.min.y }, { playerBounds.max.x + attackRange, playerBounds.max.y });
		}
		else if (orientation.x < 0)
		{
			// Attacking to the left
			attackBounds.Set({ playerBounds.min.x - attackRange, playerBounds.min.y }, { playerBounds.min.x, playerBounds.max.y });
		}
		else if (orientation.y > 0)
		{
			// Attacking downwards
			attackBounds.Set({ playerBounds.min.x, playerBounds.max.y }, { playerBounds.max.x, playerBounds.max.y + attackRange});
		}
		else
		{
			// Attacking upwards
			attackBounds.Set({ playerBounds.min.x, playerBounds.min.y - attackRange }, { playerBounds.max.x, playerBounds.min.y });
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

				target->GetHealth().RemoveHealth(attackDamage);

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
	}
	virtual void Utility()
	{
		int horizontal = 0;
		int vertical = 0;
		if (orientation.x < 0)
		{
			horizontal = -50;
		}
		else if (orientation.x > 0)
		{
			horizontal = 50;
		}
		if (orientation.y < 0)
		{
			vertical = -50;
		}
		else if (orientation.y > 0)
		{
			vertical = 50;
		}
		Vector2 direction(GetPosition().x - (GetPosition().x - horizontal), GetPosition().y - (GetPosition().y - vertical));
		ApplyKnockback(direction, 400.0f);
	}

	virtual bool CanBeDamaged() const override
	{
		return true;
	}
	bool ConsumeMeleeRequest()
	{
		if (!requestingAttack)
		{
			return false;
		}
		if (attackTimer > 0)
		{
			return false;
		}

		attackTimer = attackCooldown;
		requestingAttack = false;
		return true;
	}
	bool ConsumeAbilityRequest()
	{
		if (!requestingAbility)
		{
			return false;
		}
		if (abilityTimer > 0)
		{
			return false;
		}

		abilityTimer = abilityCooldown;
		requestingAbility = false;
		return true;
	}
	bool ConsumeUtilityRequest()
	{
		if (!requestingUtility)
		{
			return false;
		}
		if (utilityTimer > 0)
		{
			return false;
		}

		utilityTimer = utilityCooldown;
		requestingUtility = false;
		return true;
	}
	void SetInput(const Vector2& _inputAxis)
	{
		inputAxis = _inputAxis;
	}
	void SetattackInput(bool _attackInput)
	{
		attackInput = _attackInput;
	}
	void SetAbilityInput(bool _abilityInput)
	{
		abilityInput = _abilityInput;
	}
	void SetUtilityInput(bool _utilityInput)
	{
		utilityInput = _utilityInput;
	}
	void SetMoveSpeed(float _moveSpeed)
	{
		moveSpeed = _moveSpeed;
	}
	void SetattackDamage(float _attackDamage)
	{
		attackDamage = _attackDamage;
	}
	void SetAttackCooldown(float _attackCooldown)
	{
		attackCooldown = _attackCooldown;
	}
	void SetAbilityCooldown(float _abilityCooldown)
	{
		abilityCooldown = _abilityCooldown;
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
	float GetattackDamage()
	{
		return attackDamage;
	}
	float GetAttackCooldown()
	{
		return attackCooldown;
	}
	float GetAbilityCooldown()
	{
		return abilityCooldown;
	}
	Vector2 GetOrientation()
	{
		return orientation;
	}
	Team GetTeam() override
	{
		return Team::Player;
	}
	
private:
	float moveSpeed = 128.0f;
	float attackDamage = 25.0f;
	float attackRange = 32.0f;

	bool requestingAttack = false;
	bool previousAttackInput = false;
	bool attackInput = false;
	float attackCooldown = 0.5f;
	float attackTimer = 0.0f;

	bool requestingAbility = false;
	bool previousAbilityInput = false;
	bool abilityInput = false;
	float abilityCooldown = 10.0f;
	float abilityTimer = 0.0f;

	bool requestingUtility = false;
	bool previousUtilityInput = false;
	bool utilityInput = false;
	float utilityCooldown = 5.0f;
	float utilityTimer = 0.0f;

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

		if (defenseTimer > 0)
		{
			defenseTimer -= deltaTime;
			if (defenseTimer <= 0)
			{
				defenseTimer = 0;
				ToggleAbility(false);
			}
		}

		Player::Update(deltaTime, tilemap);
	}

	void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager) override
	{
		ToggleAbility(true);
		defenseTimer = defenseDuration;
	}

	void ToggleAbility(bool _toggle)
	{
		if (_toggle)
		{
			GetHealth().SetDamageReduction(0.75f);
		}
		else
		{
			GetHealth().SetDamageReduction(0.0f);
		}
	}
private:
	bool abilityActive = false;
	float defenseTimer = 0.0f;
	float defenseDuration = 2.0f;
};

class PlayerArcher : public Player
{
public:
	PlayerArcher(const Vector2 _position, std::string _filePath) : Player(_position, _filePath)
	{
		std::cout << "Init Archer" << std::endl;
		GetHealth().SetMaxHealth(85);
		SetattackDamage(20.0f);
		SetMoveSpeed(192.0f);

		SetAttackCooldown(0.4f);
		SetAbilityCooldown(8.0f);
	}

	void MeleeAttack(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager, ProjectileManager& projectileManager) override
	{
		projectileManager.FireProjectile(GetPosition(), GetOrientation(), GetattackDamage(), ProjectileOwner::Player, "./assets/Projectile.png");
	}

	// Ricochet ability for archer - bounces an attack between up to a max amount of enemies
	// Uses queues to add entities within range, and a linked list to ensure that an enemy that has been previously visited cannot be visited again
	void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager) override
	{
		Queue<VisualEntity*> targets;
		LinkedList<VisualEntity*> visitedTargets;

		Vector2 searchPosition(GetPosition().x + GetWidth() * 0.5f, GetPosition().y + GetHeight() * 0.5f);

		const float range = 200.0f;
		const float damage = GetattackDamage() * 0.75f;
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
		SetattackDamage(20.0f);
		SetMoveSpeed(160.0f);

		SetAttackCooldown(0.6f);
		SetAbilityCooldown(6.0f);
	}

	void Update(float deltaTime, Tilemap& tilemap) override
	{
		Player::Update(deltaTime, tilemap);
		if (manaTimer > 0.0f || mana < 100)
		{
			manaTimer -= deltaTime;

			if (manaTimer <= 0.0f)
			{
				mana++;
				manaTimer = manaCooldown;
			}
		}
	}

	void MeleeAttack(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager, ProjectileManager& projectileManager) override
	{
		if (mana >= 20)
		{
			mana -= 20;
			projectileManager.FireProjectile(GetPosition(), GetOrientation(), GetattackDamage(), ProjectileOwner::Player, "./assets/Projectile.png");
		}
	}

	// Magic Ability - Get all the nearby enemies and pick out 3 with the highest health -> deal damage (no knockback)
	void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager) override
	{
		DynamicArray<VisualEntity*> targets;

		Vector2 searchPosition(GetPosition().x + GetWidth() * 0.5f, GetPosition().y + GetHeight() * 0.5f);

		const float range = 250.0f;
		const float damage = GetattackDamage() * 1.5f;
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

	private:
		float mana = 100.0f;
		float manaCooldown = 0.1f;
		float manaTimer = 0.0f;
};