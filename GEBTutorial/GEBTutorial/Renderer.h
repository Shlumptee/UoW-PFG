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