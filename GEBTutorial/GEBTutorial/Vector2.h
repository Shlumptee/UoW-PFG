#pragma once
#include <iostream>

class Vector2
{
public:
	int x;
	int y;

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

	~Vector2()
	{
		delete this;
	}
};