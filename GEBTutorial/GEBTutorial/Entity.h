#pragma once

#include "GamesEngineeringBase.h"

#include "Renderer.h"
#include "Camera.h"
#include "Vector2.h"
#include "Health.h"

class Entity
{
public:
	Entity() = default;
	virtual ~Entity() = default;
};

class PositionEntity : public Entity
{
public:
	PositionEntity(const Vector2& _position) : Entity()
	{
		position = _position;
	}
	const Vector2& GetPosition() const
	{
		return position;
	}
	void SetPosition(const Vector2& _position)
	{
		position = _position;
	}

	void Move(const Vector2& delta)
	{
		position = position + delta;
	}

private:
	Vector2 position;
};

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

	virtual void Update(float deltaTime)
	{
		isColliding = false;
	}
	virtual void OnCollision(VisualEntity* other)
	{
		isColliding = true;
	}
	virtual void Draw(GamesEngineeringBase::Window& window, Camera& camera)
	{
		int screenX = GetPosition().x - camera.GetPosition().x;
		int screenY = GetPosition().y - camera.GetPosition().y;

		Render_Image(window, Vector2(screenX, screenY), image);
	}

	virtual void ApplyKnockback(Vector2 direction, float strength)
	{
		float length = direction.Length();

		if (length == 0.0f)
		{
			return;
		}

		direction = direction * (1.0f / length);
		knockbackVelocity = direction * strength;
	}
	void UpdateKnockback(float deltaTime)
	{
		float speed = knockbackVelocity.Length();
		float knockbackReduction = knockbackDecay * deltaTime;

		if (speed <= knockbackReduction)
			knockbackVelocity.Set(0, 0);
		else
			knockbackVelocity = knockbackVelocity * ((speed - knockbackReduction) / speed);
	}
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
