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
	Vector2 min;
	Vector2 max;

	Rect()
	{
		min.Set(0, 0);
		max.Set(0, 0);
	}
	Rect(Vector2 _min, Vector2 _max)
	{
		min = _min;
		max = _max;
	}

	void Set(Vector2 _min, Vector2 _max)
	{
		min = _min;
		max = _max;
	}
	void Set(Rect rect)
	{
		min = rect.min;
		max = rect.max;
	}
	float Area() override
	{
		return (min.x - max.x) * (min.y - max.y);
	}
	bool IsInside(Vector2 p) override
	{
		return (p.x >= max.x && p.x <= min.x && p.y >= max.y && p.y <= min.y);
	}
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