#pragma once
#include <iostream>

#include "GamesEngineeringBase.h"
#include "Entity.h"

#include "Shape.h"
#include "Vector2.h"
#include "Player.h"
#include "ProjectileManager.h"

// Parent class for enemies, inherits from visual entity classes
class Enemy : public VisualEntity
{
public:
	// Constructors for the class
	Enemy(const Vector2 position, std::string filePath) : VisualEntity(position, filePath)
	{
		GetHealth().SetMaxHealth(100.0f);
	}
	Enemy(const Vector2 position, Player* _player, std::string filePath) : VisualEntity(position, filePath)
	{
		GetHealth().SetMaxHealth(100.0f);
		player = _player;
	}
	Enemy(const Vector2 position, Player* _player, std::string filePath, ProjectileManager* _projectileManager) : VisualEntity(position, filePath)
	{
		GetHealth().SetMaxHealth(100.0f);
		player = _player;
	}

	virtual void Update(float deltaTime, Tilemap& tilemap) override
	{
		// Check if health is 0, instead of removing the entity from memory keep pooled far away so that it can be retrieved when another enemy needs to spawn
		if (GetHealth().GetCurrentHealth() <= 0)
		{
			SetPosition(Vector2(-10000, -10000));
			boundingBox.Set(GetBoundingBox());
			return;
		}

		// Run standard update
		VisualEntity::Update(deltaTime, tilemap);

		// Check if the enemy can attack again
		if (damageTimer > 0)
		{
			damageTimer -= deltaTime;

			if (damageTimer < 0)
			{
				damageTimer = 0;
			}
		}

		// Calculations for movement direction (must be towards player)
		Vector2 direction(player->GetPosition().x - GetPosition().x, player->GetPosition().y - GetPosition().y);
		float distance = direction.Length();

		// Normalise direction so that enemy doesn't speed up or slow down depending on how close they are
		if (distance > 0.0f)
		{
			direction = direction * (1.0f / distance);
		}

		// Store previous pos and attempt to move
		previousPosition = GetPosition();
		Vector2 movement = (direction * moveSpeed) + GetKnockbackVelocity();
		TileMove(movement, deltaTime, tilemap);

		// Update knockback
		UpdateKnockback(deltaTime);

		boundingBox.Set(GetBoundingBox());
	}
	virtual void OnCollision(VisualEntity* other) override
	{
		VisualEntity::OnCollision(other);

		if (other == nullptr)
		{
			return;
		}

		// If other entity is the player, handle slightly differently by damaging them
		// SHOULD PROBABLY BE CHANGED TO DAMAGE ANY ENTITY WITH THE PLAYER TEAM TAG
		Player* player = dynamic_cast<Player*>(other);

		if (player != nullptr && damageTimer <= 0)
		{
			// Reset cooldown so it doesn't damage every frame
			damageTimer = damageCooldown;

			// Calculate knockback for the player and deal damage
			Vector2 direction(player->GetPosition().x - GetPosition().x, player->GetPosition().y - GetPosition().y);

			player->ApplyKnockback(direction, 250.0f);
			player->GetHealth().RemoveHealth(damage);

			//std::cout << "Player hit! Health: " << player->GetHealth().GetCurrentHealth() << std::endl;
		}

		// get other entity bounding box
		Rect otherBounds = other->GetBoundingBox();
		Vector2 currentPosition = GetPosition();

		Rect previousBounds = Rect(previousPosition, Vector2(previousPosition.x + GetWidth(), previousPosition.y + GetHeight()));

		// Calculate overlap between both boxes
		float leftOverlap = GetBoundingBox().max.x - otherBounds.min.x;
		float rightOverlap = otherBounds.max.x - GetBoundingBox().min.x;
		float topOverlap = GetBoundingBox().max.y - otherBounds.min.y;
		float bottomOverlap = otherBounds.max.y - GetBoundingBox().min.y;

		// If no overlap, quit early
		// Not likely to occur unless other entity's collision ran first
		if (leftOverlap <= 0 || rightOverlap <= 0 || topOverlap <= 0 || bottomOverlap <= 0)
		{
			return;
		}

		// Find smallest overlap to work out which side of each axis needs correcting
		float overlapX = leftOverlap < rightOverlap ? leftOverlap : rightOverlap;
		float overlapY = topOverlap < bottomOverlap ? topOverlap : bottomOverlap;

		// Move the entity on the axis with the smallest overlap
		if (overlapX < overlapY)
		{
			if (leftOverlap < rightOverlap)
			{
				currentPosition.x -= leftOverlap;
			}
			else
			{
				currentPosition.x += rightOverlap;
			}
		}
		else
		{
			if (topOverlap < bottomOverlap)
			{
				currentPosition.y -= topOverlap;
			}
			else
			{
				currentPosition.y += bottomOverlap;
			}
		}

		SetPosition(currentPosition);
	}
	virtual void Draw(GamesEngineeringBase::Window& window, Camera& camera) override
	{
		VisualEntity::Draw(window, camera);
		//Render_Rect(window, boundingBox, camera.GetPosition(), GetColliding() ? 0 : 255, GetColliding() ? 255 : 0, 0, 1);
	}

	// Override from visualentity to confirm that entity can be damaged
	virtual bool CanBeDamaged() const override
	{
		return true;
	}

	// Getters + setters
	void SetMoveSpeed(float _moveSpeed)
	{
		moveSpeed = _moveSpeed;
	}
	void SetDamage(float _damage)
	{
		damage = _damage;
	}
	float GetMoveSpeed()
	{
		return moveSpeed;
	}
	float GetDamage()
	{
		return damage;
	}
	Team GetTeam() override
	{
		return Team::Enemy;
	}
	Player* GetPlayer()
	{
		return player;
	}
private:
	float moveSpeed = 32.0f;
	float damage = 10.0f;

	float damageCooldown = 0.75f;
	float damageTimer = 0.0f;

	Rect boundingBox{ Vector2(GetPosition().x , GetPosition().y), Vector2(GetPosition().x + GetWidth(), GetPosition().y + GetHeight()) };
	Player* player = nullptr;
	Vector2 previousPosition;
};

class ZombieEnemy : public Enemy
{
public:
	// Change necessary values for zombie enemy
	ZombieEnemy(const Vector2 _position, Player* _player, std::string _filePath) : Enemy(_position, _player, _filePath)
	{
		GetHealth().SetMaxHealth(130.0f);
		SetMoveSpeed(48.0f);
		SetDamage(15.0f);
	}
private:
	std::string filePath = "./assets/Enemy.png";
};

class SlimeEnemy : public Enemy
{
public:
	// Change necessary values for slime enemy
	SlimeEnemy(const Vector2 _position, Player* _player, std::string _filePath) : Enemy(_position, _player, _filePath)
	{
		GetHealth().SetMaxHealth(100.0f);
		SetMoveSpeed(96.0f);
		SetDamage(5.0f);
	}
private:
	std::string filePath = "./assets/SlimeEnemy.png";
};

class SkeletonEnemy : public Enemy
{
public:
	// Change necessary values for skeleton enemy
	// Also requires projectileManager in order to shoot projectiles
	SkeletonEnemy(const Vector2 _position, Player* _player, std::string _filePath, ProjectileManager* _projectileManager) : Enemy(_position, _player, _filePath, _projectileManager)
	{
		projectileManager = _projectileManager;
		GetHealth().SetMaxHealth(60.0f);
		SetMoveSpeed(64.0f);
		SetDamage(10.0f);
	}

	// Quick update to change entity's way of attacking, can still melee damage but should shoot
	virtual void Update(float deltaTime, Tilemap& tilemap) override
	{
		Enemy::Update(deltaTime, tilemap);

		// Stop skeleton from spam shooting
		if (projectileTimer > 0.0f)
		{
			projectileTimer -= deltaTime;
			return;
		}

		// Get distance and position of the player
		Vector2 direction(GetPlayer()->GetPosition().x - GetPosition().x, GetPlayer()->GetPosition().y - GetPosition().y);
		float distance = direction.Length();

		// Check if distance is within range
		if (distance <= projectileRange && distance >= 40.0f)
		{
			// Shoot and go on cooldown
			projectileManager->FireProjectile(GetPosition(), direction, GetDamage(), ProjectileOwner::Enemy, "./assets/Projectile.png");

			projectileTimer = projectileCooldown;
		}
	}
private:
	ProjectileManager* projectileManager = nullptr;
	std::string filePath = "./assets/SkeletonEnemy.png";

	float projectileRange = 300.0f;
	float projectileCooldown = 3.0f;
	float projectileTimer = 0.0f;
};