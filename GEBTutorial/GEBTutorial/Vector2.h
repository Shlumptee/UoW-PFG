#pragma once
#include <iostream>

class Vector2
{
public:
	float x;
	float y;

	Vector2()
	{
		x = 0;
		y = 0;
	}
	Vector2(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	void Set(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	float Length()
	{
		return std::sqrt(x * x + y * y);
	}

	float dot(const Vector2& other)
	{
		return x * other.x + y * other.y;
	}

	Vector2 operator+(const Vector2& other)
	{
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 operator*(float scalar)
	{
		return Vector2(x * scalar, y * scalar);
	}

	~Vector2()
	{
		delete this;
	}
};