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

	void AddHealth(float amount)
	{
		if (IsDead() || amount <= 0)
		{
			return;
		}

		currentHealth += amount;

		if (currentHealth > maxHealth)
		{
			currentHealth = maxHealth;
		}
	}
	void RemoveHealth(float amount)
	{
		if (IsDead() || amount <= 0)
		{
			return;
		}

		currentHealth -= amount * (1.0f - damageReduction);

		if (currentHealth < 0)
		{
			currentHealth = 0;
		}
	}
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