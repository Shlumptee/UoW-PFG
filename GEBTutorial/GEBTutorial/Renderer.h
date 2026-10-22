#pragma once

#include "GamesEngineeringBase.h"
#include "Shape.h"

void RenderImage(GamesEngineeringBase::Window& window, Vector2 position, const GamesEngineeringBase::Image& image)
{
	if (image.data == nullptr)
	{
		return;
	}

	for (unsigned int h = 0; h < image.height; h++)
	{
		if ((position.y + h) < 0 || (position.y + h) >= window.getHeight())
		{
			continue;
		}

		for (unsigned int w = 0; w < image.width; w++)
		{
			if ((position.y + w) < 0 || (position.y + w) >= window.getWidth())
			{
				continue;
			}

			if (!image.alphaAt(w, h))
			{
				continue;
			}

			unsigned char* pixel = image.at(w, h);
			window.draw(position.x + w, position.y + h, pixel[0], pixel[1], pixel[2]);
		}
	}
}