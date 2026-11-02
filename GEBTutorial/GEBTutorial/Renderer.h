#pragma once

#include "GamesEngineeringBase.h"
#include "Shape.h"

// CHANGE TO SAME METHOD AS RENDER_TILE
inline void Render_Image(GamesEngineeringBase::Window& window, Vector2 position, const GamesEngineeringBase::Image& image)
{
	if (image.data == nullptr)
	{
		return;
	}

	for (unsigned int h = 0; h < image.height; h++)
	{
		const int y = position.y + static_cast<int>(h);

		if (y < 0 || y >= window.getHeight())
		{
			continue;
		}

		for (unsigned int w = 0; w < image.width; w++)
		{
			const int x = position.x + static_cast<int>(w);

			if (x < 0 || x >= window.getWidth())
			{
				continue;
			}

			const unsigned int pixelIndex = (h * image.width + w) * image.channels;

			if (image.channels == 4 && image.data[pixelIndex + 3] != 0)
			{
				window.draw(x, y, &image.data[pixelIndex]);
			}
		}
	}
}

// Better version of previous Render_Tile, this one gets the backbuffer directly to save on time. Receiving a faster tilemap render after this change.
inline void Render_Tile(GamesEngineeringBase::Window& window, int tileID, int sourceTileSize, Vector2 position, Vector2 size, const GamesEngineeringBase::Image& image)
{
	if (image.data == nullptr)
	{
		return;
	}

	int row = image.width / size.x;

	int xPos = (tileID % row) * size.x;
	int yPos = (tileID / row) * size.y;

	unsigned char* backBuffer = window.backBuffer();
	int windowWidth = window.getWidth();
	int windowHeight = window.getHeight();

	int startX = position.x;
	int startY = position.y;
	int endX = startX + size.x;
	int endY = startY + size.y;

	if (startX < 0)
	{
		startX = 0;
	}
	if (startY < 0)
	{
		startY = 0;
	}
	if (endX > windowWidth)
	{
		endX = windowWidth;
	}
	if (endY > windowHeight)
	{
		endY = windowHeight;
	}

	for (int y = startY; y < endY; y++)
	{
		int yDiff = y - position.y;
		int screenY = yPos + (yDiff * size.y) / size.y;

		for (int x = startX; x < endX; x++)
		{
			int xDiff = x - position.x;
			int screenX = xPos + (xDiff * size.x) / size.x;

			const unsigned int pixelIndex = (screenY * image.width + screenX) * image.channels;

			if (image.channels == 4 && image.data[pixelIndex + 3] != 0)
			{
				unsigned int bufferIndex = ((y * windowWidth) + x) * 3;
				backBuffer[bufferIndex] = image.data[pixelIndex];
				backBuffer[bufferIndex + 1] = image.data[pixelIndex + 1];
				backBuffer[bufferIndex + 2] = image.data[pixelIndex + 2];
			}
		}
	}
}

inline void Render_Character(GamesEngineeringBase::Window& window, const GamesEngineeringBase::Image& image, char character, Vector2 position, int tileSize = 32)
{
	if (image.data == nullptr)
	{
		return;
	}
	
	if (character < 32 || character > 126)
	{
		return;
	}

	int tileID = static_cast<unsigned char>(character);
	int row = image.width / tileSize;
	int xPos = (tileID % row) * tileSize;
	int yPos = (tileID / row) * tileSize;

	Render_Tile(window, tileID, tileSize, position, Vector2(tileSize, tileSize), image);
}

inline void Render_String(GamesEngineeringBase::Window& window, const GamesEngineeringBase::Image& image, const char* text, Vector2 position, int tileSize = 32)
{
	if (image.data == nullptr || text == nullptr)
	{
		return;
	}

	for (int i = 0; text[i] != '\0'; i++)
	{
		Render_Character(window, image, text[i], Vector2(position.x + i * tileSize, position.y), tileSize);
	}
}

inline void Render_Number(GamesEngineeringBase::Window& window, const GamesEngineeringBase::Image& image, int number, Vector2 position, int tileSize = 32)
{
	if (image.data == nullptr)
	{
		return;
	}
	std::string string = std::to_string(number);
	Render_String(window, image, string.c_str(), position, tileSize);
}

inline void Render_Background(GamesEngineeringBase::Window& window, unsigned int r = 0, unsigned int g = 0, unsigned int b = 128)
{

	for (int y = 0; y < window.getHeight(); y++)
	{
		for (int x = 0; x < window.getWidth(); x++)
		{
			unsigned char* backBuffer = window.backBuffer() + (y * window.getWidth() + x) * 3;

			backBuffer[0] = static_cast<unsigned char>(r);
			backBuffer[1] = static_cast<unsigned char>(g);
			backBuffer[2] = static_cast<unsigned char>(b);
		}
	}
}

inline void Render_Rect(GamesEngineeringBase::Window& window, const Rect& shape, Vector2 cameraOffset, unsigned int r = 255, unsigned int g = 0, unsigned int b = 0, int thickness = 1)
{
	unsigned char color[3] = 
	{
		static_cast<unsigned char>(r),
		static_cast<unsigned char>(g),
		static_cast<unsigned char>(b)
	};

	int left = static_cast<int>(shape.min.x - cameraOffset.x) - 1;
	int top = static_cast<int>(shape.min.y - cameraOffset.y) - 1;
	int right = static_cast<int>(shape.max.x - cameraOffset.x);
	int bottom = static_cast<int>(shape.max.y - cameraOffset.y);

	if (right < left || bottom < top || thickness <= 0)
	{
		return;
	}

	auto DrawPixel = [&](int x, int y)
		{
			if (x >= 0 && x < window.getWidth() && y >= 0 && y < window.getHeight())
			{
				window.draw(x, y, color);
			}
		};

	for (int i = 0; i < thickness; i++)
	{
		for (int x = left; x <= right; x++)
		{
			DrawPixel(x, top + i);
			DrawPixel(x, bottom - i);
		}
		for (int y = top; y <= bottom; y++)
		{
			DrawPixel(left + i, y);
			DrawPixel(right - i, y);
		}
	}
}