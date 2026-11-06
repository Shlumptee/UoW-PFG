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
		std::cout << "Failed to load image atlas: " << _imagePath << std::endl;

		return;
	}
}

Tilemap::~Tilemap()
{
	delete[] map;
}

void Tilemap::Draw(GamesEngineeringBase::Window& window, Camera& camera)
{
	// Shouldn't happen as constructor already checks to see if the image can be found
	if (image.data == nullptr)
	{
		std::cout << "Failed to load image atlas" << std::endl;

		return;
	}

	// Loop through every row and column in the mapSize
	for (int row = 0; row < mapSize.x; row++)
	{
		for (int col = 0; col < mapSize.y; col++)
		{
			// Get the ID number of the tile found at the column position (eg: tile found at [1][3] could be 1, render wall)
			int tileID = GetTile(row, col);

			// Calculate where the tile should be rendered based off of it's current position in row and col + size of tile + world pos
			Vector2 tilePosition(row * tileSize.x + position.x, col * tileSize.y + position.y);

			Render_Tile(window, tileID, sourceTileSize, tilePosition - camera.GetPosition(), tileSize, image);
		}
	}
}

// Load the tilemap from a mapfile .txt
void Tilemap::LoadTilemap(std::string& mapFile)
{
	// Open mapfile
	std::ifstream tilemapStream(mapFile);

	// If it is not open, it's likely the file can't be found
	if (!tilemapStream.is_open())
	{
		std::cout << "Failed to open tilemap: " << mapFile << std::endl;

		return;
	}

	// Get the mapsize values from the .txt file
	int value;
	tilemapStream >> value;
	mapSize.x = value;
	tilemapStream >> value;
	mapSize.y = value;

	//std::cout << "Map size: x: " << mapSize.x << ", y: " << mapSize.y << std::endl;

	// Set a new map to the size described in the .txt file
	map = new int[mapSize.x * mapSize.y];

	// Initialise all of the tiles with a value of 0 (floor)
	for (int i = 0; i < mapSize.x * mapSize.y; i++)
	{
		map[i] = 0;
	}

	// Loop through every row and column to assign the correct tile ID
	for (int col = 0; col < mapSize.y; col++)
	{
		for (int row = 0; row < mapSize.x; row++)
		{
			// Get the ID value of the tile
			// If a value cannot be found, that must mean the tilemap text file is incomplete or not valid with the mapSize parameters passed in
			if (!(tilemapStream >> value))
			{
				std::cout << "Invalid or incomplete tilemap: " << mapFile << std::endl;

				return;
			}

			// This is a terrible way to implement this, but it works
			// Get the max step of the tile (aka how many times in a row this tile will be used)
			int maxStep;
			tilemapStream >> maxStep;

			// Run through until max step is reached, assign all of those index values to the read ID value
			for (int step = 0; step < maxStep; step++)
			{
				int currentRow = row + step;
				// Not sure why it gives a buffer overrun but it works???
				// It's what I get for trying to convert old SDL code, maybe try and find a fix in case it crashes in extremities!!!
				map[currentRow + col * (int)mapSize.x] = value;
			}

			// Add the amount of steps to the row
			row += maxStep - 1;
		}
	}
}

// Taken from older C++ project that used tilemap collision, also somewhat jank but at least uses AABB collision detection
bool Tilemap::TileMapCollision(const Rect& bounds, const VisualEntity& entity) const
{
	// Calculate the tile indices that the bounds occupy

	// Stuff to check if the entity is actually within the map (otherwise it will think of the top and left edges as colliding with a wall)
	float mapLeft = position.x;
	float mapRight = position.x + mapSize.x * tileSize.x;
	float mapTop = position.y;
	float mapBottom = position.y + mapSize.y * tileSize.y;

	// Allow movement if outside of the tile boundaries
	if (bounds.max.x <= mapLeft || bounds.min.x >= mapRight || bounds.max.y <= mapTop || bounds.min.y >= mapBottom)
	{
		return true;
	}

	// Calculate the tiles that the entity is overlapping with
	int leftTile = (int)((bounds.min.x - position.x) / tileSize.x);
    int rightTile = (int)((bounds.max.x - position.x) / tileSize.x);

    int topTile = (int)((bounds.min.y - position.y) / tileSize.y);
    int bottomTile = (int)((bounds.max.y - position.y) / tileSize.y);

	// run through every collided tile, check for tile ID
	for (int col = bottomTile; col >= topTile; col--)
	{
		for (int row = leftTile; row <= rightTile; row++)
		{
			// Get tile ID
			int tileID = GetTile(row, col);

			// If there is nothing there, then collision is out of bounds and should be skipped
			if (tileID == -1)
			{
				//std::cout << "Tilemap collision check out of bounds" << std::endl;
				continue;
			}

			// Get the tiletype from tile ID and compare with the entity's list of tile's it is allowed to traverse through
			TileType tileType = GetTileType(tileID);

			// If entity cannot walk through the tile, return false
			if (!entity.TileInteraction(tileType))
			{
				//std::cout << "Collision with tile: " << tileID << std::endl;
				return false;
			}
		}
	}
	//std::cout << "No collision" << std::endl;
	// No collision, return true
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

// Get tile ID at a position indicated by row number and column number
int Tilemap::GetTile(int row, int col) const
{
	// If values are negative or outside of mapsize, return nothing
	if (row < 0 || row >= mapSize.x || col < 0 || col >= mapSize.y)
	{
		return -1;
	}

	// Return value found at the position in the array
	return map[row + col * (int)mapSize.x];
}

// Change from tile ID to a tile enum
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