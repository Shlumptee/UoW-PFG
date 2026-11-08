#pragma once

#include "GamesEngineeringBase.h"
#include "Shape.h"

// Draws an image at the given position
// CHANGE TO SAME METHOD AS RENDER_TILE
inline void Render_Image(GamesEngineeringBase::Window& window, Vector2 position, const GamesEngineeringBase::Image& image)
{
	// Return if insufficient data is found for the image
	if (image.data == nullptr)
	{
		return;
	}

	// Loop through the height and width in pixels of the image
	for (unsigned int h = 0; h < image.height; h++)
	{
		// Calculate screent position of the current pixel and ensure it is on screen
		const int y = position.y + static_cast<int>(h);

		// Skip pixels outside of the screen
		if (y < 0 || y >= window.getHeight())
		{
			continue;
		}

		for (unsigned int w = 0; w < image.width; w++)
		{
			// Calculate screent position of the current pixel and ensure it is on screen
			const int x = position.x + static_cast<int>(w);

			// Skip pixels outside the screen
			if (x < 0 || x >= window.getWidth())
			{
				continue;
			}

			// Get the index (taking into account width and amount of channels (rgb, rgba)) at the position of the pixel in the data
			const unsigned int pixelIndex = (h * image.width + w) * image.channels;

			// Only draw pixels without an alpha value
			if (image.channels == 4 && image.data[pixelIndex + 3] != 0)
			{
				window.draw(x, y, &image.data[pixelIndex]);
			}
		}
	}
}

// Draw a single tile from a tileset or tile atlas
// Better version of previous Render_Tile, this one gets the backbuffer directly to save on time. Receiving a faster tilemap render after this change.
inline void Render_Tile(GamesEngineeringBase::Window& window, int tileID, int sourceTileSize, Vector2 position, Vector2 size, const GamesEngineeringBase::Image& image)
{
	if (image.data == nullptr)
	{
		return;
	}

	// Calculate how many tiles are in a row and get X + Y pos from it depending on tileID passed through
	int row = image.width / size.x;

	int xPos = (tileID % row) * size.x;
	int yPos = (tileID / row) * size.y;

	// Get backbuffer and window size
	unsigned char* backBuffer = window.backBuffer();
	int windowWidth = window.getWidth();
	int windowHeight = window.getHeight();

	// Get start and end positions for where the tile should be drawn
	int startX = position.x;
	int startY = position.y;
	int endX = startX + size.x;
	int endY = startY + size.y;

	// Ensure that it can only draw within the window boundaries
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

	// Loop through every pixel in the selected tile
	for (int y = startY; y < endY; y++)
	{
		int yDiff = y - position.y;
		int screenY = yPos + (yDiff * size.y) / size.y;

		for (int x = startX; x < endX; x++)
		{
			int xDiff = x - position.x;
			int screenX = xPos + (xDiff * size.x) / size.x;

			// Calculate the position of the pixel in the image data and get the number of channels
			const unsigned int pixelIndex = (screenY * image.width + screenX) * image.channels;

			// Don't render any pixels with alpha data
			if (image.channels == 4 && image.data[pixelIndex + 3] != 0)
			{
				// Every pixel in the buffer uses 3 channels (r, g, b)
				// Get data for those channels and copy to the backbuffer
				unsigned int bufferIndex = ((y * windowWidth) + x) * 3;
				backBuffer[bufferIndex] = image.data[pixelIndex];
				backBuffer[bufferIndex + 1] = image.data[pixelIndex + 1];
				backBuffer[bufferIndex + 2] = image.data[pixelIndex + 2];
			}
		}
	}
}

// Uses Render_Tile for the most part
// Requires the font tilemap to be in ASCII format or else it will break
inline void Render_Character(GamesEngineeringBase::Window& window, const GamesEngineeringBase::Image& image, char character, Vector2 position, int tileSize = 32)
{
	if (image.data == nullptr)
	{
		return;
	}
	
	// Only support ASCII characters that can be printed
	// So long as the tilemap is in the ASCII format, each character should equate to the right character tile
	if (character < 32 || character > 126)
	{
		return;
	}

	// Convert the character into it's correct tile ID (direct conversion)
	int tileID = static_cast<unsigned char>(character);
	int row = image.width / tileSize;
	int xPos = (tileID % row) * tileSize;
	int yPos = (tileID / row) * tileSize;

	// Render the tile, pass through all relevant information
	Render_Tile(window, tileID, tileSize, position, Vector2(tileSize, tileSize), image);
}

// Renders a string of characters
inline void Render_String(GamesEngineeringBase::Window& window, const GamesEngineeringBase::Image& image, const char* text, Vector2 position, int tileSize = 32)
{
	// Check both the image passed in and text is valid
	if (image.data == nullptr || text == nullptr)
	{
		return;
	}

	// Loop through every character in the string until it has reached the end
	// The end of a string is marked by a \0
	for (int i = 0; text[i] != '\0'; i++)
	{
		// Draw the current character and adjust the position parameter so it doesn't overlap another character
		Render_Character(window, image, text[i], Vector2(position.x + i * tileSize, position.y), tileSize);
	}
}

// Renders one or more numbers together from an integer
inline void Render_Number(GamesEngineeringBase::Window& window, const GamesEngineeringBase::Image& image, int number, Vector2 position, int tileSize = 32)
{
	// Check the image to ensure it is valid
	if (image.data == nullptr)
	{
		return;
	}
	// Turn the number into a string and pass it through to Render_String
	std::string string = std::to_string(number);
	Render_String(window, image, string.c_str(), position, tileSize);
}

// Render the background as a solid colour
inline void Render_Background(GamesEngineeringBase::Window& window, unsigned int r = 0, unsigned int g = 0, unsigned int b = 128)
{
	// Loop through every pixel in the window
	for (int y = 0; y < window.getHeight(); y++)
	{
		for (int x = 0; x < window.getWidth(); x++)
		{
			// Get the backbuffer and assign the r g b values
			unsigned char* backBuffer = window.backBuffer() + (y * window.getWidth() + x) * 3;

			backBuffer[0] = static_cast<unsigned char>(r);
			backBuffer[1] = static_cast<unsigned char>(g);
			backBuffer[2] = static_cast<unsigned char>(b);
		}
	}
}

// DEBUG to render bounding boxes
inline void Render_Rect(GamesEngineeringBase::Window& window, const Rect& shape, Vector2 cameraOffset, unsigned int r = 255, unsigned int g = 0, unsigned int b = 0, int thickness = 1)
{
	// Get colour channels
	unsigned char colour[3] = 
	{
		static_cast<unsigned char>(r),
		static_cast<unsigned char>(g),
		static_cast<unsigned char>(b)
	};

	// Convert the world coordinates to screen coordinates and get each side of the box
	int left = static_cast<int>(shape.min.x - cameraOffset.x) - 1;
	int top = static_cast<int>(shape.min.y - cameraOffset.y) - 1;
	int right = static_cast<int>(shape.max.x - cameraOffset.x);
	int bottom = static_cast<int>(shape.max.y - cameraOffset.y);

	// Check all sides are valid and that thickness is not less than or equal to 0 (otherwise not drawing anything!)
	if (right < left || bottom < top || thickness <= 0)
	{
		return;
	}

	// Loop through for all egdes of the bounding box, thickness increases number of pixels drawn for the box
	for (int i = 0; i < thickness; i++)
	{
		// Draw top and bottom pixels
		for (int x = left; x <= right; x++)
		{
			int y = top + i;
			if (x >= 0 && x < window.getWidth() && y >= 0 && y < window.getHeight())
			{
				window.draw(x, y, colour);
			}
			y = bottom - i;
			if (x >= 0 && x < window.getWidth() && y >= 0 && y < window.getHeight())
			{
				window.draw(x, y, colour);
			}
		}
		// Draw left and right pixels
		for (int y = top; y <= bottom; y++)
		{
			int x = left + i;
			if (x >= 0 && x < window.getWidth() && y >= 0 && y < window.getHeight())
			{
				window.draw(x, y, colour);
			}
			x = right - i;
			if (x >= 0 && x < window.getWidth() && y >= 0 && y < window.getHeight())
			{
				window.draw(x, y, colour);
			}
		}
	}
}