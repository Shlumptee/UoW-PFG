#pragma once

#include <fstream>

#include "GamesEngineeringBase.h"
#include "Shape.h"
#include "Renderer.h"

class Tilemap
{
public:
	Tilemap(Vector2 _mapSize, Vector2 _tileSize, int _sourceTileSize, Vector2 _position);
	~Tilemap();

	void Init(std::string mapfile, std::string imagePath);
	void Draw(GamesEngineeringBase::Window& window);

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