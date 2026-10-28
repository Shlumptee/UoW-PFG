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
		VisualEntity::Update(deltaTime);

		boundingBox.Set(GetBoundingBox());
	}
	virtual void Draw(GamesEngineeringBase::Window& window, Camera& camera) override
	{
		VisualEntity::Draw(window, camera);
		Render_Rect(window, boundingBox, GetColliding() ? 0 : 255, GetColliding() ? 255 : 0, 0, 1);
	}

private:
	float moveSpeed = 128.0f;

	Rect boundingBox{ Vector2(GetPosition().x , GetPosition().y), Vector2(GetPosition().x + GetWidth(), GetPosition().y + GetHeight()) };
};