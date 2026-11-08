#pragma once
#include <iostream>

#include "GamesEngineeringBase.h"
#include "Entity.h"

#include "Shape.h"
#include "Vector2.h"
#include "Player.h"
#include "ProjectileManager.h"

class Enemy : public VisualEntity
{
public:
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
		if (GetHealth().GetCurrentHealth() <= 0)
		{
			SetPosition(Vector2(-1000, -1000));
			boundingBox.Set(GetBoundingBox());
			return;
		}

		VisualEntity::Update(deltaTime, tilemap);

		if (damageTimer > 0)
		{
			damageTimer -= deltaTime;

			if (damageTimer < 0)
			{
				damageTimer = 0;
			}
		}

		Vector2 direction(player->GetPosition().x - GetPosition().x, player->GetPosition().y - GetPosition().y);
		float distance = direction.Length();

		if (distance > 0.0f)
		{
			direction = direction * (1.0f / distance);
		}

		previousPosition = GetPosition();
		Vector2 movement = (direction * moveSpeed) + GetKnockbackVelocity();
		TileMove(movement, deltaTime, tilemap);

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

		Player* player = dynamic_cast<Player*>(other);

		if (player != nullptr && damageTimer <= 0)
		{
			damageTimer = damageCooldown;

			Vector2 direction(player->GetPosition().x - GetPosition().x, player->GetPosition().y - GetPosition().y);

			player->ApplyKnockback(direction, 250.0f);
			player->GetHealth().RemoveHealth(damage);

			std::cout << "Player hit! Health: " << player->GetHealth().GetCurrentHealth() << std::endl;
		}

		Rect otherBoundingBox = other->GetBoundingBox();
		Vector2 currentPosition = GetPosition();

		Rect previousBounds = Rect(previousPosition, Vector2(previousPosition.x + GetWidth(), previousPosition.y + GetHeight()));

		float leftOverlap = GetBoundingBox().max.x - otherBoundingBox.min.x;
		float rightOverlap = otherBoundingBox.max.x - GetBoundingBox().min.x;
		float topOverlap = GetBoundingBox().max.y - otherBoundingBox.min.y;
		float bottomOverlap = otherBoundingBox.max.y - GetBoundingBox().min.y;

		if (leftOverlap <= 0 || rightOverlap <= 0 || topOverlap <= 0 || bottomOverlap <= 0)
		{
			return;
		}

		float OverlapX = leftOverlap < rightOverlap ? leftOverlap : rightOverlap;
		float overlapY = topOverlap < bottomOverlap ? topOverlap : bottomOverlap;

		if (OverlapX < overlapY)
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
		Render_Rect(window, boundingBox, camera.GetPosition(), GetColliding() ? 0 : 255, GetColliding() ? 255 : 0, 0, 1);
	}

	virtual bool CanBeDamaged() const override
	{
		return true;
	}

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
	SkeletonEnemy(const Vector2 _position, Player* _player, std::string _filePath, ProjectileManager* _projectileManager) : Enemy(_position, _player, _filePath, _projectileManager)
	{
		projectileManager = _projectileManager;
		GetHealth().SetMaxHealth(60.0f);
		SetMoveSpeed(64.0f);
		SetDamage(10.0f);
	}

	virtual void Update(float deltaTime, Tilemap& tilemap) override
	{
		Enemy::Update(deltaTime, tilemap);

		if (projectileTimer > 0.0f)
		{
			projectileTimer -= deltaTime;
			return;
		}

		Vector2 direction(GetPlayer()->GetPosition().x - GetPosition().x, GetPlayer()->GetPosition().y - GetPosition().y);
		float distance = direction.Length();

		if (distance <= projectileRange && distance >= 40.0f)
		{
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