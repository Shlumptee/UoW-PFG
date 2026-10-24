#pragma once
#include <iostream>

#include "GamesEngineeringBase.h"
#include "Entity.h"

#include "Shape.h"
#include "Vector2.h"

class Enemy : public VisualEntity
{
public:
	Enemy(const Vector2 position, std::string filePath) : VisualEntity(position, filePath)
	{
	}

	virtual void Update(float deltaTime) override
	{

	}
	virtual void Draw(GamesEngineeringBase::Window& window) override
	{
		VisualEntity::Draw(window);
	}

private:
	float moveSpeed = 128.0f;

	Rect boundingBox{ Vector2(GetPosition().x , GetPosition().y), Vector2(GetPosition().x + GetWidth(), GetPosition().y - GetHeight()) };
};