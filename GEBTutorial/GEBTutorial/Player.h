#pragma once
#include <iostream>

#include "GamesEngineeringBase.h"
#include "Entity.h"

#include "Shape.h"
#include "Vector2.h"

class Player : public VisualEntity
{
public:
	Player(const Vector2 position, std::string filePath) : VisualEntity(position, filePath)
	{
		std::cout << "Bounding Box: Min(" << boundingBox.min.x << ", " << boundingBox.min.y << "), Max(" << boundingBox.max.x << ", " << boundingBox.max.y << ")" << std::endl;
	}

	virtual void Update(float deltaTime) override
	{
		VisualEntity::Update(deltaTime);

		boundingBox.Set(GetBoundingBox());
		Move(inputAxis * moveSpeed * deltaTime);
	}
	virtual void OnCollision(VisualEntity* other) override
	{
		VisualEntity::OnCollision(other);

		std::cout << "Player collided with another entity!" << std::endl;
		SetPosition(boundingBox.min);
	}
	virtual void Draw(GamesEngineeringBase::Window& window, Camera& camera) override
	{
		VisualEntity::Draw(window, camera);
		Render_Rect(window, boundingBox, GetColliding() ? 0 : 255, GetColliding() ? 255 : 0, 0, 1);
	}

	void SetInput(const Vector2& _inputAxis)
	{
		inputAxis = _inputAxis;
	}
	const Vector2& GetInput() const
	{
		return inputAxis;
	}
	
private:
	float moveSpeed = 128.0f;

	Vector2 inputAxis;

	Rect boundingBox{ Vector2(GetPosition().x , GetPosition().y), Vector2(GetPosition().x + GetWidth(), GetPosition().y + GetHeight())};
};

class PlayerWarrior : public Player
{
	void Update(float deltaTime) override
	{
		// Implement specific update logic for PlayerWarrior
		Player::Update(deltaTime);
	}
};

//class Player
//{
//public:
//	Player(const GamesEngineeringBase::Window& _canvas)
//	{
//		canvas = _canvas;
//	}
//
//	void Init();
//	void HandleInput();
//	void Update();
//	void Draw();
//	void Destroy();
//
//	void MoveLeft()
//	{
//		position.x -= moveSpeed;
//		orientation = 3;
//
//		if (position.x < 0)
//		{
//			position.x = 0;
//		}
//	}
//
//	void MoveRight()
//	{
//		position.x += moveSpeed;
//		orientation = 1;
//
//		if (position.x > canvas.getWidth() - image.width)
//		{
//			position.x = canvas.getWidth() - image.width;
//		}
//	}
//
//	void MoveDown()
//	{
//		position.y += moveSpeed;
//		orientation = 2;
//
//		if (position.y > (canvas.getHeight() - image.height))
//		{
//			position.y = canvas.getHeight() - image.height;
//		}
//	}
//
//	void MoveUp()
//	{
//		position.y -= moveSpeed;
//		orientation = 0;
//
//		if (position.y < 0)
//		{
//			position.y = 0;
//		}
//	}
//
//	void MoveAbility()
//	{
//		if (abilityTimer.dt() < abilityCooldown)
//		{
//			return;
//		}
//
//		switch (orientation)
//		{
//		case 0:
//			position.y -= moveSpeed * 50;
//			if (position.y < 0)
//			{
//				position.y = 0;
//			}
//			break;
//		case 1:
//			position.x += moveSpeed * 50;
//			if (position.x > canvas.getWidth() - image.width)
//			{
//				position.x = canvas.getWidth() - image.width;
//			}
//			break;
//		case 2:
//			position.y += moveSpeed * 50;
//			if (position.y > (canvas.getHeight() - image.height))
//			{
//				position.y = canvas.getHeight() - image.height;
//			}
//			break;
//		case 3:
//			position.x -= moveSpeed * 50;
//			if (position.x < 0)
//			{
//				position.x = 0;
//			}
//			break;
//		default:
//			break;
//		}
//
//		abilityTimer.reset();
//	}
//
//private:
//	int moveSpeed = 1;
//	Vector2 position = Vector2(100, 100);
//	unsigned int orientation = 0;
//
//	float abilityCooldown = 1.0f;
//
//	GamesEngineeringBase::Window canvas;
//	GamesEngineeringBase::Image image;
//	GamesEngineeringBase::Timer abilityTimer;
//};

