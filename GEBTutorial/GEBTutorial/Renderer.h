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

inline void Render_Rect(GamesEngineeringBase::Window& window, const Rect& shape, unsigned int r = 255, unsigned int g = 0, unsigned int b = 0, int thickness = 1)
{
	unsigned char color[3] = 
	{
		static_cast<unsigned char>(r),
		static_cast<unsigned char>(g),
		static_cast<unsigned char>(b)
	};

	int left = static_cast<int>(shape.min.x);
	int top = window.getHeight() - static_cast<int>(shape.max.y);
	int right = static_cast<int>(shape.max.x) - 1;
	int bottom = window.getHeight() - static_cast<int>(shape.min.y) - 1;

	if (right < left || bottom < top || thickness <= 0)
	{
		return;
	}

	auto DrawPixel = [&](int x, int y)
		{
			if (x >= 0 && x < window.getWidth() && y >= 0 && y < window.getHeight())
			{
				window.draw(x, window.getHeight() + (shape.max.y - shape.min.y) - y, color);
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