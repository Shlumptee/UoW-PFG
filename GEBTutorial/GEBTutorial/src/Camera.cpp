#include "Camera.h"
#include "Entity.h"

// Camera constructors
Camera::Camera()
{
	position = { 0, 0 };
	viewportPos = { 0, 0 };
	worldPosMax = { 0, 0 };
	worldPosMin = { 0, 0 };
}
Camera::Camera(Vector2 _cameraSizeMin, Vector2 _cameraSizeMax)
{
	position = { 0, 0 };
	viewportPos = _cameraSizeMax;
	worldPosMin = _cameraSizeMin;
	worldPosMax = _cameraSizeMax;
}

// Function to make the camera follow a target
void Camera::Follow(VisualEntity& target)
{
	Vector2 targetPos = target.GetPosition();

	// Need to account for entitypos being in top left, need to get centre
	// Delete if changing the GetPosition() function
	position.x = targetPos.x + (target.GetWidth() * 0.5f) - viewportPos.x * 0.5f;
	position.y = targetPos.y + (target.GetHeight() * 0.5f) - viewportPos.y * 0.5f;

	// skip oif no bounds are assigned
	if (worldPosMax.x == 0 && worldPosMax.y == 0)
	{
		return;
	}

	// Keep camera inside world
	ToWorld();
}

// Setters and Getters
// SHOULD PROBABLY MOVE TO HEADER FILE TO STAY IN LINE WITH REST OF CODE FORMATTING IN THIS PROJECT
void Camera::SetWorldBounds(Vector2 boundsMin, Vector2 boundsMax)
{
	worldPosMin = boundsMin;
	worldPosMax = boundsMax;

	ToWorld();
}
// Get camera position so other entities can adjust where their position is relative to the camera
Vector2 Camera::GetPosition()
{
	return position;
}

void Camera::ToWorld()
{
	// Calculate furthest max position the camera can reach
	float maxX = worldPosMax.x - viewportPos.x;
	float maxY = worldPosMax.y - viewportPos.y;

	// Prevent the max positions from going negative
	if (maxX < 0)
	{
		maxX = 0;
	}
	if (maxY < 0)
	{
		maxY = 0;
	}

	// Stop camera moving past minimum boundary
	if (position.x < worldPosMin.x)
	{
		position.x = 0;
	}
	if (position.y < worldPosMin.y)
	{
		position.y = 0;
	}

	// Stop camera moving past maximum boundary
	if (position.x > maxX)
	{
		position.x = maxX;
	}
	if (position.y > maxY)
	{
		position.y = maxY;
	}
}