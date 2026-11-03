#pragma once

#include <fstream>

#include "GamesEngineeringBase.h"
#include "Shape.h"
#include "Renderer.h"
#include "Camera.h"

class VisualEntity;

enum class TileType
{
	Floor = 0,
	Wall = 1,
	Pit = 2,
	Empty = 3
};

class Tilemap
{
public:
	Tilemap() = default;
	Tilemap(Vector2 _tileSize, int _sourceTileSize, Vector2 _position, std::string mapFile, std::string imagePath);
	~Tilemap();

	void Draw(GamesEngineeringBase::Window& window, Camera& camera);
	bool TileMapCollision(const Rect& boundingBox, const VisualEntity& entity) const;

	int GetTile(int row, int col) const;

private:
	void LoadTilemap(std::string& mapFile);

	TileType GetTileType(int tileID) const;

	int* map = nullptr;
	int sourceTileSize;

	Vector2 mapSize;
	Vector2 tileSize;
	Vector2 position;

	GamesEngineeringBase::Image image;
};

class InfiniteTilemap : public Tilemap
{
	InfiniteTilemap(Vector2 _tileSize, int _sourceTileSize, Vector2 _position, std::string mapFile, std::string imagePath)
		: Tilemap(_tileSize, _sourceTileSize, _position, mapFile, imagePath)
	{
	}
};