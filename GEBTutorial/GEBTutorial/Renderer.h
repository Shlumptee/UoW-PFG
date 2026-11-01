#pragma once

#include "GamesEngineeringBase.h"
#include "Shape.h"

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

			if (image.channels == 3 || image.data[pixelIndex + 3] != 0)
			{
				window.draw(x, y, &image.data[pixelIndex]);
			}
		}
	}
}

inline void Render_Tile(GamesEngineeringBase::Window& window, int tileID, int sourceTileSize, Vector2 position, Vector2 size, const GamesEngineeringBase::Image& image)
{
	if (image.data == nullptr)
	{
		return;
	}

	int columns = image.width / sourceTileSize;

	int xPos = (tileID % columns) * sourceTileSize;
	int yPos = (tileID / columns) * sourceTileSize;

	if (xPos + sourceTileSize > image.width || yPos + sourceTileSize > image.height)
	{
		return;
	}

	for (int y = 0; y < size.y; y++)
	{
		int screenY = position.y + y;

		if (screenY < 0 || screenY >= window.getHeight())
		{
			continue;
		}

		for (int x = 0; x < size.x; x++)
		{
			int screenX = position.x + x;

			if (screenX < 0 || screenX >= window.getWidth())
			{
				continue;
			}

			const unsigned int pixelIndex = ((yPos + (y * sourceTileSize) / size.y) * image.width + (xPos + (x * sourceTileSize) / size.x)) * image.channels;

			if (image.channels != 4 || image.data[pixelIndex + 3] != 0)
			{
				window.draw(screenX, screenY, &image.data[pixelIndex]);
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