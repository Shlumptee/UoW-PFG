#pragma once

#include "GamesEngineeringBase.h"

#include "Renderer.h"
#include "Vector2.h"

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
	VisualEntity() : PositionEntity({ 0, 0 })
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
	virtual void Draw(GamesEngineeringBase::Window& window)
	{
		Render_Image(window, GetPosition(), image);
	}
	virtual const GamesEngineeringBase::Image& GetImage() const
	{
		return image;
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
	void SetImagePath(const std::string& _imagePath)
	{
		imagePath = _imagePath;
	}

private:
	bool isColliding = false;

	std::string imagePath;
	GamesEngineeringBase::Image image;
};
