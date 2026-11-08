#pragma once

class ScoreManager
{
public:
	void Reset()
	{
		score = 0;
		kills = 0;
		timeAlive = 0.0f;
	}
	// Update how long the player has been alive for
	void Update(float deltaTime)
	{
		timeAlive += deltaTime;
	}

	// Add score
	void AddScore(int amount)
	{
		if (amount > 0)
		{
			score += amount;
		}
	}
	// Increase kill count and add score
	void AddKill(int amount)
	{
		kills++;
		AddScore(amount);
	}

	// Getters and setters
	int GetScore() const
	{
		return score;
	}
	int GetKills() const
	{
		return kills;
	}
	float GetTimeAlive() const
	{
		return timeAlive;
	}
private:
	int score = 0;
	int kills = 0;
	
	float timeAlive = 0.0f;
};