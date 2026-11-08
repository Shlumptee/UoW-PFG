#pragma once

#include "GamesEngineeringBase.h"

#include "Renderer.h"
#include "Camera.h"
#include "Vector2.h"
#include "Health.h"
#include "Tilemap.h"

// Enum to determine what team the entity is on
enum class Team
{
	Player,
	Enemy,
	Neutral
};

// Base entity class
class Entity
{
public:
	Entity() = default;
	virtual ~Entity() = default;
};

// A position entity is a base entity that holds a vector2 position
class PositionEntity : public Entity
{
public:
	// Constructor
	PositionEntity(const Vector2& _position) : Entity()
	{
		position = _position;
	}

	// Getter + setter
	const Vector2& GetPosition() const
	{
		return position;
	}
	void SetPosition(const Vector2& _position)
	{
		position = _position;
	}

	// Move function to move the entity on the screen
	void Move(const Vector2& delta)
	{
		position = position + delta;
	}

private:
	Vector2 position;
};

// A visual entity inherits from positionentity, can be seen on screen and has collision etc
class VisualEntity : public PositionEntity
{
public:
	VisualEntity() : PositionEntity({ 0, 0 }), health()
	{

	}
	VisualEntity(const Vector2& _position, const std::string& _imagePath) : PositionEntity(_position)
	{
		imagePath = _imagePath;
		image.load(imagePath);
	}
	// Set collider to false
	virtual void Update(float deltaTime, Tilemap& tilemap)
	{
		isColliding = false;
	}
	// If called, there has been a collision
	virtual void OnCollision(VisualEntity* other)
	{
		isColliding = true;
	}
	// Draw the entity at the world position relative to the camera
	virtual void Draw(GamesEngineeringBase::Window& window, Camera& camera)
	{
		int screenX = GetPosition().x - camera.GetPosition().x;
		int screenY = GetPosition().y - camera.GetPosition().y;

		Render_Image(window, Vector2(screenX, screenY), image);
	}

	// Applies a knockback velocity
	virtual void ApplyKnockback(Vector2 direction, float strength)
	{
		// Get the length of the vector
		float length = direction.Length();

		if (length == 0.0f)
		{
			return;
		}

		// Normalise so it doesn't get super fast or slow
		direction = direction * (1.0f / length);
		knockbackVelocity = direction * strength;
	}
	// Update the knockback velocity
	void UpdateKnockback(float deltaTime)
	{
		// Get speed and how much the knockback decay should be reduced by
		float speed = knockbackVelocity.Length();
		float knockbackReduction = knockbackDecay * deltaTime;

		// If the speed is less that the reduction, stop enforcing a knockback
		if (speed <= knockbackReduction)
		{
			knockbackVelocity.Set(0, 0);
		}
		else
		{
			// Reduce knockback velocity over time
			knockbackVelocity = knockbackVelocity * ((speed - knockbackReduction) / speed);
		}
	}

	// Get what tiles the entity can interact with based off of type
	virtual bool TileInteraction(TileType tile) const
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
			return false;
		default:
			return true;
		}
	}

	// Move the entity based on the tilemap and its bounding box
	void TileMove(const Vector2& movement, float deltaTime, const Tilemap& tilemap)
	{
		// Split horizontal and vertical movement (NEED TO DO THIS FOR QUADTREE COLLISIONS)
		Vector2 originalposition = GetPosition();
		
		// Move on the horizontal axis
		Move(Vector2(movement.x * deltaTime, 0));

		// Check if the entity can move through the current tiles it overlaps
		if (!tilemap.TileMapCollision(GetBoundingBox(), *this))
		{
			// If colliding a wall or other tile it cannot move into, go back
			SetPosition(originalposition);
		}
		
		Vector2 horizontalPosition = GetPosition();

		// Move on the vertical axis
		Move(Vector2(0, movement.y * deltaTime));

		if (!tilemap.TileMapCollision(GetBoundingBox(), *this))
		{
			SetPosition(horizontalPosition);
		}
	}

	// Getters and setters
	virtual const GamesEngineeringBase::Image& GetImage() const
	{
		return image;
	}
	virtual bool CanBeDamaged() const
	{
		return false;
	}
	virtual int GetWidth() const
	{
		return image.width;
	}
	virtual int GetHeight() const
	{
		return image.height;
	}
	virtual bool GetColliding()
	{
		return isColliding;
	}
	Rect GetBoundingBox() const
	{
		return Rect(Vector2(GetPosition().x, GetPosition().y), Vector2(GetPosition().x + GetWidth(), GetPosition().y + GetHeight()));
	}
	const std::string& GetImagePath() const
	{
		return imagePath;
	}
	Health& GetHealth()
	{
		return health;
	}
	float GetKnockbackDecay() const
	{
		return knockbackDecay;
	}
	Vector2 GetKnockbackVelocity() const
	{
		return knockbackVelocity;
	}
	virtual Team GetTeam()
	{
		return Team::Neutral;
	}
	void SetImagePath(const std::string& _imagePath)
	{
		imagePath = _imagePath;
	}
	void SetKnockbackDecay(float _knockbackDecay)
	{
		knockbackDecay = _knockbackDecay;
	}

private:
	bool isColliding = false;

	std::string imagePath;
	GamesEngineeringBase::Image image;

	Health health;
	Vector2 knockbackVelocity;
	float knockbackDecay = 600.0f;
};
