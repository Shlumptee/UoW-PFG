#pragma once

#include "Vector2.h"

class Shape
{
public:
	virtual float Area() = 0;
	virtual bool IsInside(Vector2 p) = 0;
};

class Rectangle : public Shape
{
public:
	Rectangle()
	{
		length.Set(0, 0);
		width.Set(0, 0);
	}
	Rectangle(Vector2 _length, Vector2 _width)
	{
		length = _length;
		width = _width;
	}

	float Area() override
	{
		return (length.x - width.x) * (length.y - width.y);
	}

	bool IsInside(Vector2 p) override
	{
		return (p.x >= width.x && p.x <= length.x && p.y >= width.y && p.y <= length.y);
	}

private:
	Vector2 length;
	Vector2 width;
};