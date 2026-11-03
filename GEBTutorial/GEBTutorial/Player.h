#pragma once
#include <iostream>

#include "GamesEngineeringBase.h"
#include "Entity.h"

#include "Shape.h"
#include "Vector2.h"
#include "Quadtree.h"
#include "ScoreManager.h"

class Player : public VisualEntity
{
public:
	Player(const Vector2 position, std::string filePath) : VisualEntity(position, filePath)
	{
		GetHealth().SetMaxHealth(100);
	}

	virtual void Update(float deltaTime) override
	{
		VisualEntity::Update(deltaTime);

		if (inputAxis.x != 0 || inputAxis.y != 0)
		{
			orientation = inputAxis;
		}

		previousPosition = GetPosition();
		Move(((inputAxis * moveSpeed) + GetKnockbackVelocity()) * deltaTime);
		UpdateKnockback(deltaTime);

		boundingBox.Set(GetBoundingBox());

		if (meleeInput && !previousMeleeInput)
		{
			requestingMelee = true;
		}

		previousMeleeInput = meleeInput;
	}
	virtual void OnCollision(VisualEntity* other) override
	{
		VisualEntity::OnCollision(other);
		SetPosition(previousPosition);
	}
	virtual void Draw(GamesEngineeringBase::Window& window, Camera& camera) override
	{
		VisualEntity::Draw(window, camera);
		Render_Rect(window, boundingBox, camera.GetPosition(), GetColliding() ? 0 : 255, GetColliding() ? 255 : 0, 0, 1);
	}

	virtual void MeleeAttack(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities, ScoreManager& scoreManager)
	{
		Rect attackBounds;
		Rect playerBounds = GetBoundingBox();

		if (orientation.x > 0)
		{
			// Attacking to the right
			attackBounds.Set({ playerBounds.max.x, playerBounds.min.y }, { playerBounds.max.x + meleeRange, playerBounds.max.y });
		}
		else if (orientation.x < 0)
		{
			// Attacking to the left
			attackBounds.Set({ playerBounds.min.x - meleeRange, playerBounds.min.y }, { playerBounds.min.x, playerBounds.max.y });
		}
		else if (orientation.y > 0)
		{
			// Attacking downwards
			attackBounds.Set({ playerBounds.min.x, playerBounds.max.y }, { playerBounds.max.x, playerBounds.max.y + meleeRange});
		}
		else
		{
			// Attacking upwards
			attackBounds.Set({ playerBounds.min.x, playerBounds.min.y - meleeRange }, { playerBounds.max.x, playerBounds.min.y });
		}

		for (int i = 0; i < entities.GetSize(); i++)
		{
			VisualEntity* target = entities[i];

			if (target == nullptr || target == this)
			{
				continue;
			}

			if (!target->CanBeDamaged())
			{
				continue;
			}

			if (quadtree.Intersects(attackBounds, target->GetBoundingBox()))
			{
				Vector2 direction(target->GetPosition().x - GetPosition().x, target->GetPosition().y - GetPosition().y);

				target->ApplyKnockback(direction, 250.0f);

				bool wasAlive = target->GetHealth().IsDead();
				int previousHealth = target->GetHealth().GetCurrentHealth();

				target->GetHealth().RemoveHealth(meleeDamage);

				int damageDealt = previousHealth - target->GetHealth().GetCurrentHealth();

				if (damageDealt > 0)
				{
					scoreManager.AddScore(damageDealt * 0.75f);
				}
				if (!wasAlive && target->GetHealth().IsDead())
				{
					std::cout << "Enemy killed! Score: " << scoreManager.GetScore() << std::endl;
					scoreManager.AddKill(150);
				}

				//std::cout << "Enemy Hit! Health: " << target->GetHealth().GetCurrentHealth() << std::endl;
			}
		}
	}

	virtual bool CanBeDamaged() const override
	{
		return true;
	}
	bool ConsumeMeleeRequest()
	{
		if (!requestingMelee)
		{
			return false;
		}

		requestingMelee = false;
		return true;
	}
	void SetInput(const Vector2& _inputAxis)
	{
		inputAxis = _inputAxis;
	}
	void SetMeleeInput(bool _meleeInput)
	{
		meleeInput = _meleeInput;
	}
	const Vector2& GetInput() const
	{
		return inputAxis;
	}
	int GetCurrentHealth()
	{
		return GetHealth().GetCurrentHealth();
	}
	
private:
	float moveSpeed = 128.0f;
	float meleeDamage = 25.0f;
	float meleeRange = 32.0f;

	bool requestingMelee = false;
	bool previousMeleeInput = false;
	bool meleeInput = false;

	Vector2 inputAxis;
	Vector2 orientation = { 0, 1 };
	Vector2 previousPosition;

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

