#pragma once

#include "Entity.h"

// Enum for projectiles, assigns them to an owner group
enum class ProjectileOwner
{
	Player,
	Enemy,
	Neutral
};

class Projectile : public VisualEntity
{
public:
	Projectile(Vector2 _position, std::string _imagePath) : VisualEntity(_position, _imagePath)
	{
		isActive = false;
	}

	// Activate the projectile from being inactive
	void Activate(Vector2 _position, Vector2 _direction, float _speed, float _damage, float _lifeTime, ProjectileOwner _owner)
	{
		// Projectile needs position of firing entity + direction, and a constant velocity to move at
		SetPosition(_position);
		lifeTime = _lifeTime;
		damage = _damage;
		owner = _owner;
		isActive = true;

		float length = _direction.Length();
		velocity = _direction * (_speed / length);


	}
	// Disable to stop projectile from running update functions
	void Deactivate()
	{
		isActive = false;
	}

	void Update(float deltaTime, Tilemap& tilemap) override
	{
		// Don't run any inactive projectiles
		if (!isActive)
		{
			return;
		}

		// Run standard update
		VisualEntity::Update(deltaTime, tilemap);

		// Decrease the lifetime of the projectile
		lifeTime -= deltaTime;
		if (lifeTime <= 0)
		{
			Deactivate();
			return;
		}

		// Move bullet and check collision with the tilemap
		Vector2 previousPosition = GetPosition();
		Move(velocity * deltaTime);

		if (!tilemap.TileMapCollision(GetBoundingBox(), *this))
		{
			Deactivate();
		}
	}

	// Override tile interaction from VisualEntity as bullets should pass over pits
	bool TileInteraction(TileType tile) const override
	{
		switch (tile)
		{
		case TileType::Floor:
			return true;
		case TileType::Empty:
			return true;
		case TileType::Wall:
			return false;
		case TileType::Pit:
			return true;
		default:
			return true;
		}
	}
	bool IsActive()
	{
		return isActive;
	}
	
	ProjectileOwner GetOwner()
	{
		return owner;
	}
	float GetDamage()
	{
		return damage;
	}

private:
	ProjectileOwner owner = ProjectileOwner::Player;
	Vector2 velocity = { 0, 0 };

	float lifeTime = 0.0f;
	float damage = 20.0f;

	bool isActive = false;
};