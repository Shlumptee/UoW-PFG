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
	void Update(float deltaTime)
	{
		timeAlive += deltaTime;
	}

	void AddScore(int amount)
	{
		if (amount > 0)
		{
			score += amount;
		}
	}
	void AddKill(int amount)
	{
		kills++;
		AddScore(amount);
	}

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