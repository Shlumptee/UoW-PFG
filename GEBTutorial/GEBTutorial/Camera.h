#pragma once

#include "Vector2.h"
class VisualEntity;

class Camera
{
public:
	Camera();
	Camera(Vector2 _cameraSizeMin, Vector2 _cameraSizeMax);

	void Follow(VisualEntity& target);

	void SetWorldBounds(Vector2 boundsMin, Vector2 boundsMax);
	Vector2 GetPosition();

private:
	Vector2 position;
	Vector2 viewportPos;
	Vector2 worldPosMax;
	Vector2 worldPosMin;

	void ToWorld();
};