#include "Camera.h"
#include "Entity.h"

Camera::Camera()
{
	position = { 0, 0 };
	viewportPos = { 0, 0 };
	worldPos = { 0, 0 };
}
Camera::Camera(Vector2 _cameraSize)
{
	position = { 0, 0 };
	viewportPos = _cameraSize;
	worldPos = _cameraSize;
}

void Camera::Follow(VisualEntity& target)
{
	Vector2 targetPos = target.GetPosition();

	// Need to account for entitypos being in top left, need to get centre
	// Delete if changing the GetPosition() function
	position.x = targetPos.x + (target.GetWidth() * 0.5f) - viewportPos.x * 0.5f;
	position.y = targetPos.y + (target.GetHeight() * 0.5f) - viewportPos.y * 0.5f;

	ToWorld();
}

void Camera::SetWorldBounds(Vector2 bounds)
{
	worldPos = bounds;

	ToWorld();
}
Vector2 Camera::GetPosition()
{
	return position;
}

void Camera::ToWorld()
{
	float maxX = worldPos.x - viewportPos.x;
	float maxY = worldPos.y - viewportPos.y;

	if (maxX < 0)
	{
		maxX = 0;
	}
	if (maxY < 0)
	{
		maxY = 0;
	}

	if (position.x < 0)
	{
		position.x = 0;
	}
	if (position.y < 0)
	{
		position.y = 0;
	}

	if (position.x > maxX)
	{
		position.x = maxX;
	}
	if (position.y > maxY)
	{
		position.y = maxY;
	}
}