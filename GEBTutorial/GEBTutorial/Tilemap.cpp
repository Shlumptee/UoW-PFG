#include "Tilemap.h"
#include "Entity.h"

// Code taken and modified from an older C++ project that used tilemaps
// Conversion isn't 1-to-1, so might be janky
// Definitely isn't efficient either, should look at replacing with better version
Tilemap::Tilemap(Vector2 _tileSize, int _sourceTileSize, Vector2 _position, std::string _mapFile, std::string _imagePath)
{
	tileSize = _tileSize;
	sourceTileSize = _sourceTileSize;
	position = _position;

	LoadTilemap(_mapFile);

	image.load(_imagePath);

	if (image.data == nullptr)
	{
		std::cout << "Failed to load tile atlas: " << _imagePath << std::endl;

		return;
	}
}

Tilemap::~Tilemap()
{
	delete[] map;
}

void Tilemap::Draw(GamesEngineeringBase::Window& window, Camera& camera)
{
	if (image.data == nullptr)
	{
		std::cout << "Failed to load tile atlas" << std::endl;

		return;
	}

	for (int row = 0; row < mapSize.x; row++)
	{
		for (int col = 0; col < mapSize.y; col++)
		{
			int tileID = GetTile(row, col);

			Vector2 tilePosition(row * tileSize.x + position.x, col * tileSize.y + position.y);

			Render_Tile(window, tileID, sourceTileSize, tilePosition - camera.GetPosition(), tileSize, image);
		}
	}
}

void Tilemap::LoadTilemap(std::string& mapFile)
{
	std::ifstream tilemapStream(mapFile);

	if (!tilemapStream.is_open())
	{
		std::cout << "Failed to open tilemap: " << mapFile << std::endl;

		return;
	}

	int value;
	tilemapStream >> value;
	mapSize.x = value;
	tilemapStream >> value;
	mapSize.y = value;

	std::cout << "Map size: x: " << mapSize.x << ", y: " << mapSize.y << std::endl;

	map = new int[mapSize.x * mapSize.y];

	for (int i = 0; i < mapSize.x * mapSize.y; i++)
	{
		map[i] = 0;
	}

	for (int col = 0; col < mapSize.y; col++)
	{
		for (int row = 0; row < mapSize.x; row++)
		{
			if (!(tilemapStream >> value))
			{
				std::cout << "Invalid or incomplete tilemap: " << mapFile << std::endl;

				return;
			}

			int maxStep;
			tilemapStream >> maxStep;

			for (int step = 0; step < maxStep; step++)
			{
				int currentRow = row + step;
				map[currentRow + col * (int)mapSize.x] = value;
			}

			row += maxStep - 1;
		}
	}
}

// Taken from older C++ project that used tilemap collision, also somewhat jank but at least uses AABB collision detection
bool Tilemap::TileMapCollision(const Rect& bounds, const VisualEntity& entity) const
{
	int leftTile = ((bounds.min.x - position.x) / tileSize.x);
    int rightTile = ((bounds.max.x - position.x) / tileSize.x);

    int topTile = ((bounds.min.y - position.y) / tileSize.y);
    int bottomTile = ((bounds.max.y - position.y) / tileSize.y);

	for (int col = topTile; col <= bottomTile; col++)
	{
		for (int row = leftTile; row <= rightTile; row++)
		{
			int tileID = GetTile(row, col);

			if (tileID == -1)
			{
				//std::cout << "Tilemap collision check out of bounds" << std::endl;
				continue;
			}

			TileType tileType = GetTileType(tileID);

			if (!entity.TileInteraction(tileType))
			{
				//std::cout << "Collision with tile: " << tileID << std::endl;
				return false;
			}
		}
	}
	//std::cout << "No collision" << std::endl;
	return true;
}

// Old load without run length encoding
//void Tilemap::LoadTilemap(std::string& mapFile)
//{
//	std::ifstream tilemapStream(mapFile);
//
//	if (!tilemapStream.is_open())
//	{
//		std::cout << "Failed to open tilemap: " << mapFile << std::endl;
//
//		return;
//	}
//
//	int value;
//	tilemapStream >> value;
//	mapSize.x = value;
//	tilemapStream >> value;
//	mapSize.y = value;
//
//	std::cout << "Map size: x: " << mapSize.x << ", y: " << mapSize.y << std::endl;
//
//	map = new int[mapSize.x * mapSize.y];
//
//	for (int i = 0; i < mapSize.x * mapSize.y; i++)
//	{
//		map[i] = 0;
//	}
//
//	for (int col = 0; col < mapSize.y; col++)
//	{
//		for (int row = 0; row < mapSize.x; row++)
//		{
//
//			if (!(tilemapStream >> value))
//			{
//				std::cout << "Invalid or incomplete tilemap: " << mapFile << std::endl;
//
//				return;
//			}
//
//			map[row + col * (int)mapSize.x] = value;
//		}
//	}
//}

int Tilemap::GetTile(int row, int col) const
{
	if (row < 0 || row >= mapSize.x || col < 0 || col >= mapSize.y)
	{
		return -1;
	}

	return map[row + col * (int)mapSize.x];
}

TileType Tilemap::GetTileType(int tileID) const
{
	switch (tileID)
	{
	case 0:
		return TileType::Floor;
	case 1:
		return TileType::Wall;
	case 2:
		return TileType::Pit;
	default:
		return TileType::Empty;
	}
}