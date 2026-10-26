#include "Tilemap.h"

// Code taken and modified from an older C++ project that used tilemaps
// Conversion isn't 1-to-1, so might be janky
// Definitely isn't efficient either, should look at replacing with better version
Tilemap::Tilemap(Vector2 _mapSize, Vector2 _tileSize, int _sourceTileSize, Vector2 _position)
{
	mapSize = _mapSize;
	tileSize = _tileSize;
	sourceTileSize = _sourceTileSize;
	position = _position;

	map = new int[mapSize.x, mapSize.y];

	
}

Tilemap::~Tilemap()
{
	delete[] map;
}

void Tilemap::Init(std::string mapfile, std::string imagePath)
{
	LoadTilemap(mapfile);

	image.load(imagePath);
}

void Tilemap::Draw(GamesEngineeringBase::Window& window)
{
	for (int row = 0; row < mapSize.y; row++)
	{
		for (int col = 0; col < mapSize.x; col++)
		{
			int tileID = GetTile(row, col);

			Vector2 tilePosition(col * tileSize.x + position.x, row * tileSize.y + position.y);

			Render_Tile(window, tileID, sourceTileSize, tilePosition, tileSize, image);
		}
	}
}

void Tilemap::LoadTilemap(std::string& mapFile)
{
	std::ifstream tilemapStream(mapFile);

	for (int row = 0; row < mapSize.y; row++)
	{
		for (int col = 0; col < mapSize.x; col++)
		{
			int value;

			if (!(tilemapStream >> value))
			{
				std::cout << "Invalid or incomplete tilemap: " << mapFile << std::endl;

				return;
			}

			map[row * (int)mapSize.x + col] = value;
		}
	}
}

int Tilemap::GetTile(int row, int col) const
{
	if (row < 0 || row >= mapSize.y ||
		col < 0 || col >= mapSize.x)
	{
		return -1;
	}

	return map[row * (int)mapSize.x + col];
}