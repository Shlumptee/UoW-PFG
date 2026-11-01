#pragma once
#include <iostream>

#include "GamesEngineeringBase.h"
#include "Entity.h"

#include "Shape.h"
#include "Vector2.h"
#include "Player.h"

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

	virtual void Update(float deltaTime) override
	{
		VisualEntity::Update(deltaTime);

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
		Move(((direction * moveSpeed) + GetKnockbackVelocity()) * deltaTime);
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

		SetPosition(previousPosition);

		Player* player = dynamic_cast<Player*>(other);

		if (player == nullptr || damageTimer > 0)
		{
			return;
		}

		Vector2 direction(player->GetPosition().x - GetPosition().x, player->GetPosition().y - GetPosition().y);

		player->ApplyKnockback(direction, 250.0f);
		player->GetHealth().RemoveHealth(damage);

		damageTimer = damageCooldown;

		std::cout << "Player hit! Health: " << player->GetHealth().GetCurrentHealth() << std::endl;
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
private:
	float moveSpeed = 32.0f;
	float damage = 10.0f;

	float damageCooldown = 0.75f;
	float damageTimer = 0.0f;

	Rect boundingBox{ Vector2(GetPosition().x , GetPosition().y), Vector2(GetPosition().x + GetWidth(), GetPosition().y + GetHeight()) };
	Player* player = nullptr;
	Vector2 previousPosition;
};