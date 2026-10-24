#pragma once

#include "Shape.h"

class Quadtree
{
public:
	Quadtree(int _level, Rect _bounds) : level(_level), bounds(_bounds)
	{
		for (int i = 0; i < 4; i++)
		{
			nodes[i] = nullptr;
		}
	}
	~Quadtree()
	{
		for (int i = 0; i < 4; i++)
		{
			if (nodes[i] != nullptr)
			{
				delete nodes[i];
			}
		}
	}

	// Split a node within the quadtree into 4 subnodes
	void Split()
	{
		float subWidth = (bounds.min.x - bounds.max.x) / 2.0f;
		float subHeight = (bounds.min.y - bounds.max.y) / 2.0f;
		float x = bounds.max.x;
		float y = bounds.max.y;

		// Init new nodes
		nodes[0] = new Quadtree(level + 1, Rect(Vector2(x + subWidth, y), Vector2(x + subWidth * 2, y + subHeight)));
		nodes[1] = new Quadtree(level + 1, Rect(Vector2(x, y), Vector2(x + subWidth, y + subHeight)));
		nodes[2] = new Quadtree(level + 1, Rect(Vector2(x, y + subHeight), Vector2(x + subWidth, y + subHeight * 2)));
		nodes[3] = new Quadtree(level + 1, Rect(Vector2(x + subWidth, y + subHeight), Vector2(x + subWidth * 2, y + subHeight * 2)));
	}

	// Determine which node an object belongs to
	// -1 = object cannot fit within child node and is instead a part of parent node
	int GetIndex(Rect rect)
	{
		int index = -1;

		float verticalMidpoint = bounds.max.x + (bounds.min.x - bounds.max.x) / 2.0f;
		float horizontalMidpoint = bounds.max.y + (bounds.min.y - bounds.max.y) / 2.0f;

		bool topQuadrant = (rect.max.y < horizontalMidpoint && rect.min.y < horizontalMidpoint);
		bool bottomQuadrant = (rect.max.y > horizontalMidpoint);

		if (rect.max.x < verticalMidpoint && rect.min.x < verticalMidpoint)
		{
			if (topQuadrant)
			{
				index = 1;
			}
			else if (bottomQuadrant)
			{
				index = 2;
			}
		}
		else if (rect.max.x > verticalMidpoint)
		{
			if (topQuadrant)
			{
				index = 0;
			}
			else if (bottomQuadrant)
			{
				index = 3;
			}
		}
		return index;
	}

	// Insert an object into the quadtree
	// If the node exceeds the capacity, it will split the node into subnodes and add all objects to their corresponding subnodes
	void Insert(Rect rect)
	{
		if (nodes[0] != nullptr)
		{
			int index = GetIndex(rect);
			if (index != -1)
			{
				nodes[index]->Insert(rect);
				return;
			}
		}

		objects.Push(rect);

		if (objects.GetSize() > maxObjects && level < maxLevels)
		{
			if (nodes[0] == nullptr)
			{
				Split();
			}

			int i = 0;
			while (i < objects.GetSize())
			{
				int index = GetIndex(objects[i]);
				if (index != -1)
				{
					nodes[index]->Insert(objects[i]);
					objects.Remove(i);
				}
				else
				{
					i++;
				}
			}
		}
	}

	// Return all objects that could collide with a given object
	DynamicArray<Rect> Retrieve(DynamicArray<Rect>& returnObjects, Rect rect)
	{
		int index = GetIndex(rect);
		if (index != -1 && nodes[0] != nullptr)
		{
			nodes[index]->Retrieve(returnObjects, rect);
		}
		for (int i = 0; i < objects.GetSize(); i++)
		{
			returnObjects.Push(objects[i]);
		}
		return returnObjects;
	}

	// Clear the quadtree
	void Clear()
	{
		objects.Clear();
		for (int i = 0; i < 4; i++)
		{
			if (nodes[i] != nullptr)
			{
				nodes[i]->Clear();
				delete nodes[i];
				nodes[i] = nullptr;
			}
		}
	}
private:
	int maxObjects = 10;
	int maxLevels = 5;

	int level;

	Rect bounds;
	DynamicArray<Rect> objects;
	Quadtree* nodes[4];
};