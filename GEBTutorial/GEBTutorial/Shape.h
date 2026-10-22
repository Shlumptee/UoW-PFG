#pragma once

#include "Vector2.h"

class Shape
{
public:
	virtual float Area() = 0;
	virtual bool IsInside(Vector2 p) = 0;
};

class Rect : public Shape
{
public:
	Rect()
	{
		length.Set(0, 0);
		width.Set(0, 0);
	}
	Rect(Vector2 _length, Vector2 _width)
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

class Circle : public Shape
{
public:
	Circle()
	{
		radius = 1;
		center.Set(0, 0);
	}
	Circle(float _radius, Vector2 _center)
	{
		radius = _radius;
		center = _center;
	}

	float Area() override
	{
		return 3.14159f * radius * radius;
	}
	bool IsInside(Vector2 p) override
	{
		Vector2 diff = Vector2(p.x - center.x, p.y - center.y);
		return (diff.Length() <= radius);
	}

	float GetRadius()
	{
		return radius;
	}
private:
	float radius;
	Vector2 center;
};