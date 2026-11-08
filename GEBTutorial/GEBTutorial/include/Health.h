#pragma once

class Health
{
public:
	Health()
	{
		currentHealth = maxHealth = 100;
	}
	Health(float _maxHealth)
	{
		currentHealth = maxHealth = _maxHealth;
	}

	// Add health to the entity
	void AddHealth(float amount)
	{
		// Quit out if the entity is dead
		if (IsDead() || amount <= 0)
		{
			return;
		}

		// Add amount and check if it doesn't go over maxHealth
		currentHealth += amount;

		if (currentHealth > maxHealth)
		{
			currentHealth = maxHealth;
		}
	}
	// Remove health from entity
	void RemoveHealth(float amount)
	{
		// Quit out if entity is dead
		if (IsDead() || amount <= 0)
		{
			return;
		}

		// Reduce health by amount
		// Also reduce the amount of damage taken if the entity has damagereduction
		currentHealth -= amount * (1.0f - damageReduction);

		if (currentHealth < 0)
		{
			currentHealth = 0;
		}
	}

	// Getters and setters
	float IsDead()
	{
		return currentHealth <= 0;
	}

	float GetCurrentHealth()
	{
		return currentHealth;
	}
	float GetMaxHealth()
	{
		return maxHealth;
	}

	void SetCurrentHealth(float _currentHealth)
	{
		currentHealth = _currentHealth;
	}
	void SetMaxHealth(float _maxHealth)
	{
		maxHealth = _maxHealth;
		currentHealth = _maxHealth;
	}
	void SetDamageReduction(float _damageReduction)
	{
		damageReduction = _damageReduction;
	}
private:
	float currentHealth;
	float maxHealth;
	float damageReduction = 0.0f;
};