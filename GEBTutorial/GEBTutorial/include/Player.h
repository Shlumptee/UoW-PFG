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
	// Constructor for player
	Player(const Vector2 position, std::string filePath) : VisualEntity(position, filePath)
	{
		//std::cout << "Init Player" << std::endl;
	}


	virtual void Update(float deltaTime, Tilemap& tilemap) override
	{
		// Run base update in visualentity
		VisualEntity::Update(deltaTime, tilemap);

		// Disable input requests
		requestingAbility = false;
		requestingAttack = false;
		requestingUtility = false;

		// if there is input set to the axis, update orientation accordingly
		if (inputAxis.x != 0 || inputAxis.y != 0)
		{
			orientation = inputAxis;
		}

		// Timers for attacks, abilities, etc
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

		if (manaTimer > 0.0f)
		{
			manaTimer -= deltaTime;

			if (manaTimer <= 0.0f && mana < maxMana)
			{
				mana++;
				manaTimer = manaCooldown;
			}
		}

		// Get previous pos and perform movement calculations to see if player can move in requested direction
		previousPosition = GetPosition();
		Vector2 movement = (inputAxis * moveSpeed) + GetKnockbackVelocity();
		TileMove(movement, deltaTime, tilemap);

		// Update any knockback received
		UpdateKnockback(deltaTime);

		boundingBox.Set(GetBoundingBox());

		// Check for additional inputs
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
		// Run base collision
		VisualEntity::OnCollision(other);
		//SetPosition(previousPosition);


		Rect otherBounds = other->GetBoundingBox();
		Vector2 currentPosition = GetPosition();

		Rect previousBounds = Rect(previousPosition, Vector2(previousPosition.x + GetWidth(), previousPosition.y + GetHeight()));

		bool horizontalOverlap = currentPosition.x < otherBounds.max.x && currentPosition.x + GetWidth() > otherBounds.min.x;
		bool verticalOverlap = currentPosition.y < otherBounds.max.y && currentPosition.y + GetHeight() > otherBounds.min.y;

		if (!horizontalOverlap || !verticalOverlap)
		{
			return;
		}

		bool horizontalCollision = previousBounds.max.x <= otherBounds.min.x || previousBounds.min.x >= otherBounds.max.x;
		bool verticalCollision = previousBounds.max.y <= otherBounds.min.y || previousBounds.min.y >= otherBounds.max.y;

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
		//Render_Rect(window, boundingBox, camera.GetPosition(), GetColliding() ? 0 : 255, GetColliding() ? 255 : 0, 0, 1);
	}

	// Function to perform a melee attack
	virtual void MeleeAttack(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager, ProjectileManager& projectileManager)
	{
		// Set up attack bounding box
		Rect attackBounds;
		Rect playerBounds = GetBoundingBox();

		// Check what side of the player the attack box should be set
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

		// Loop through all entities
		// !! NOT EXACTLY IDEAL, SHOULD SEE IF CAN RETRIVE QUAD THE PLAYER IS IN!!
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

			// See if Both the target and attack boxes intersect
			if (quadtree.Intersects(attackBounds, target->GetBoundingBox()))
			{
				// Get direction and apply knockback
				Vector2 direction(target->GetPosition().x - GetPosition().x, target->GetPosition().y - GetPosition().y);

				target->ApplyKnockback(direction, 250.0f);

				bool wasAlive = !target->GetHealth().IsDead();
				int previousHealth = target->GetHealth().GetCurrentHealth();

				// Deal damage
				target->GetHealth().RemoveHealth(attackDamage);

				int damageDealt = previousHealth - target->GetHealth().GetCurrentHealth();

				// Check what scoremanager function to call - should only give large points for kills
				if (damageDealt > 0)
				{
					scoreManager.AddScore(damageDealt * 0.75f);
				}
				if (wasAlive && target->GetHealth().IsDead())
				{
					scoreManager.AddKill(150);
					//std::cout << "Enemy killed! Score: " << scoreManager.GetScore() << std::endl;
				}

				//std::cout << "Enemy Hit! Health: " << target->GetHealth().GetCurrentHealth() << std::endl;
			}
		}
	}

	virtual void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager)
	{
	}

	// Utility movement - make a short dash in the direction the player is facing
	virtual void Utility()
	{
		int horizontal = 0;
		int vertical = 0;

		// Check orientation for where player should move
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
		// Caluclate direction and use the knockback function to fake dashing
		Vector2 direction(GetPosition().x - (GetPosition().x - horizontal), GetPosition().y - (GetPosition().y - vertical));
		ApplyKnockback(direction, 400.0f);
	}

	// Override from visualentity, confirms entity can be damaged
	virtual bool CanBeDamaged() const override
	{
		return true;
	}

	// Functions for consuming input requests, will check if input can be made and go back on cooldown
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
		if (mana < 20)
		{
			return false;
		}
		mana -= 20;
		manaTimer = manaCooldown;
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
	
	// Lotsa getters and setters
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
	void SetManaCooldown(float _manaCooldown)
	{
		manaCooldown = _manaCooldown;
	}
	void SetMaxMana(float _maxMana)
	{
		maxMana = _maxMana;
		mana = _maxMana;
	}
	void SetMana(float _mana)
	{
		mana = _mana;
	}
	void SetManaTimer(float _manaTimer)
	{
		manaTimer = _manaTimer;
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
	float GetMaxMana()
	{
		return maxMana;
	}
	float GetMana()
	{
		return mana;
	}
	float GetManaCooldown()
	{
		return manaCooldown;
	}
	
private:
	float moveSpeed = 128.0f;
	float attackDamage = 25.0f;
	float attackRange = 32.0f;

	float mana = 100.0f;
	float maxMana = 100.0f;
	float manaCooldown = 0.5f;
	float manaTimer = manaCooldown;

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
	// most settings are the same for the warrior calss
	PlayerWarrior(const Vector2 _position, std::string _filePath) : Player(_position, _filePath)
	{
		GetHealth().SetMaxHealth(150);
		
	}
	void Update(float deltaTime, Tilemap& tilemap) override
	{
		// Implement specific update logic for PlayerWarrior

		// Update timer for ability duration
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

	// Override for a high resistance ability - while active, damage is reduced
	void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager) override
	{
		ToggleAbility(true);
		defenseTimer = defenseDuration;
	}

	// Toggle to choose whether damage reduction should be on or off
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
	// Set variables for the archer class
	PlayerArcher(const Vector2 _position, std::string _filePath) : Player(_position, _filePath)
	{
		//std::cout << "Init Archer" << std::endl;
		GetHealth().SetMaxHealth(85);
		SetattackDamage(20.0f);
		SetMoveSpeed(192.0f);

		SetAttackCooldown(0.4f);
		SetAbilityCooldown(8.0f);
	}

	// Different attack - archer uses projectilemanager to shoot projectiles that can damage
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

				// If candidate is visited, skip past it
				if (visitedTargets.Contains(candidate))
				{
					continue;
				}

				// Get position difference between current search pos and the candidate
				Vector2 candidatePosition(candidate->GetPosition().x + candidate->GetWidth() * 0.5f, candidate->GetPosition().y + candidate->GetHeight() * 0.5f);
				Vector2 positionDifference = candidatePosition - searchPosition;
				float distance = positionDifference.Length();

				// Check if candidate is within range
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
				//std::cout << "Enemy killed! Score: " << scoreManager.GetScore() << std::endl;
			}
		}
	}
};

class PlayerMage : public Player
{
public:
	// Set variables for the mage class
	PlayerMage(const Vector2 _position, std::string _filePath) : Player(_position, _filePath)
	{
		GetHealth().SetMaxHealth(100);
		SetattackDamage(20.0f);
		SetMoveSpeed(160.0f);

		SetAttackCooldown(0.6f);
		SetAbilityCooldown(6.0f);

		SetManaCooldown(0.2f);
		SetMaxMana(120.0f);
	}

	void Update(float deltaTime, Tilemap& tilemap) override
	{
		Player::Update(deltaTime, tilemap);
	}

	// Different attack, shoots projectiles that cost mana. This is offset by a faster mana regen and higher maxmana
	void MeleeAttack(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager, ProjectileManager& projectileManager) override
	{
		if (GetMana() >= 15)
		{
			SetMana(GetMana() - 15.0f);
			SetManaTimer(GetManaCooldown());
			projectileManager.FireProjectile(GetPosition(), GetOrientation(), GetattackDamage(), ProjectileOwner::Player, "./assets/Projectile.png");
		}
	}

	// Magic Ability - Get all the nearby enemies and pick out 3 with the highest health -> deal damage (no knockback)
	void Ability(DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager) override
	{
		DynamicArray<VisualEntity*> targets;

		// Get midpoint of the player
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

			// Check if within range
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

			// Loop through targets
			for (int j = i + 1; j < targets.GetSize(); j++)
			{
				if (targets[j]->GetHealth().GetCurrentHealth() > targets[highestIndex]->GetHealth().GetCurrentHealth())
				{
					highestIndex = j;
				}
			}

			// Perform a swap if highestindex has changed
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
				//std::cout << "Enemy killed! Score: " << scoreManager.GetScore() << std::endl;
			}
		}
	}
};