#pragma once

#include "Entity.h"

enum class ProjectileOwner
{
	Player,
	Enemy
};

class Projectile : public VisualEntity
{
public:
	Projectile(Vector2 _position, std::string _imagePath) : VisualEntity(_position, _imagePath)
	{
		isActive = false;
	}

	void Activate(Vector2 _position, Vector2 _direction, float _speed, float _lifeTime, ProjectileOwner _owner)
	{
		SetPosition(_position);
		lifeTime = _lifeTime;
		owner = _owner;
		isActive = true;

		float length = _direction.Length();
		velocity = _direction * (_speed / length);


	}
	void Deactivate()
	{
		isActive = false;
	}

	void Update(float deltaTime, Tilemap& tilemap) override
	{
		if (!isActive)
		{
			return;
		}

		VisualEntity::Update(deltaTime, tilemap);

		lifeTime -= deltaTime;
		if (lifeTime <= 0)
		{
			Deactivate();
			return;
		}

		Vector2 previousPosition = GetPosition();
		TileMove(velocity, deltaTime, tilemap);

		if (!tilemap.TileMapCollision(GetBoundingBox(), *this))
		{
			Deactivate();
		}
	}

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

private:
	ProjectileOwner owner = ProjectileOwner::Player;
	Vector2 velocity = { 0, 0 };

	float lifeTime = 0.0f;

	bool isActive = false;
};