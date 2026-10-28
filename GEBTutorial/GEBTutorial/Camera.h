#pragma once

#include "Vector2.h"
class VisualEntity;

class Camera
{
public:
	Camera();
	Camera(Vector2 _cameraSize);

	void Follow(VisualEntity& target);

	void SetWorldBounds(Vector2 bounds);
	Vector2 GetPosition();

private:
	Vector2 position;
	Vector2 viewportPos;
	Vector2 worldPos;

	void ToWorld();
};