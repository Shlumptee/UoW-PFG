#pragma once

#include <fstream>

#include "GamesEngineeringBase.h"
#include "Shape.h"
#include "Renderer.h"
#include "Camera.h"

class Tilemap
{
public:
	Tilemap(Vector2 _tileSize, int _sourceTileSize, Vector2 _position, std::string mapFile, std::string imagePath);
	~Tilemap();

	void Draw(GamesEngineeringBase::Window& window, Camera& camera);

	int GetTile(int row, int col) const;

private:
	void LoadTilemap(std::string& mapFile);

	int* map = nullptr;
	int sourceTileSize;

	Vector2 mapSize;
	Vector2 tileSize;
	Vector2 position;

	GamesEngineeringBase::Image image;
};