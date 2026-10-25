#pragma once

#include "Shape.h"
#include "Entity.h"

struct CollisionObject
{
	VisualEntity* entity = nullptr;
	Rect boundingBox;

	CollisionObject() = default;
	CollisionObject(VisualEntity* _entity, const Rect& _boundingBox)
	{
		entity = _entity;
		boundingBox = _boundingBox;
	}
};

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
		float midX = (bounds.min.x - bounds.max.x) / 2.0f;
		float midY = (bounds.min.y - bounds.max.y) / 2.0f;
		float x = bounds.max.x;
		float y = bounds.max.y;

		// Init new nodes
		nodes[0] = new Quadtree(level + 1, Rect(Vector2(midX, midY), Vector2(x, y)));
		nodes[1] = new Quadtree(level + 1, Rect(Vector2(x, midY), Vector2(midX, y)));
		nodes[2] = new Quadtree(level + 1, Rect(Vector2(x, y), Vector2(midX, midY)));
		nodes[3] = new Quadtree(level + 1, Rect(Vector2(midX, y), Vector2(x, midY)));
	}

	// Determine which node an object belongs to
	// -1 = object cannot fit within child node and is instead a part of parent node
	int GetIndex(Rect rect)
	{
		float midX = (bounds.min.x + bounds.max.x) / 2.0f;
		float midY = (bounds.min.y + bounds.max.y) / 2.0f;

		bool left = rect.max.x <= midX;
		bool right = rect.min.x >= midX;

		bool lower = rect.max.y <= midY;
		bool upper = rect.min.y >= midY;

		if (left)
		{
			if (upper)
			{
				return 1; // Top-left
			}
			if (lower)
			{
				return 2; // Bottom-left
			}
		}

		if (right)
		{
			if (upper)
			{
				return 0; // Top-right
			}
			if (lower)
			{
				return 3; // Bottom-right
			}
		}

		return -1;
	}

	// Insert an object into the quadtree
	// If the node exceeds the capacity, it will split the node into subnodes and add all objects to their corresponding subnodes
	void Insert(const CollisionObject& object)
	{
		if (nodes[0] != nullptr)
		{
			int index = GetIndex(object.boundingBox);

			if (index != -1)
			{
				nodes[index]->Insert(object);
				return;
			}
		}

		objects.Push(object);

		if (objects.GetSize() > maxObjects && level < maxLevels)
		{
			if (nodes[0] == nullptr)
			{
				Split();
			}

			int i = 0;
			while (i < objects.GetSize())
			{
				int index = GetIndex(objects[i].boundingBox);
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
	void Retrieve(DynamicArray<CollisionObject>& returnObjects, const Rect& query)
	{
		for (int i = 0; i < objects.GetSize(); i++)
		{
			returnObjects.Push(objects[i]);
		}

		if (nodes[0] == nullptr)
		{
			return;
		}

		for (int i = 0; i < 4; i++)
		{
			const Rect& childBounds = nodes[i]->bounds;

			bool isOverlapping = query.min.x < childBounds.max.x &&
				query.max.x > childBounds.min.x &&
				query.min.y < childBounds.max.y &&
				query.max.y > childBounds.min.y;

			if (isOverlapping)
			{
				nodes[i]->Retrieve(returnObjects, query);
			}
		}
	}

	bool Intersects(const Rect& a, const Rect& b)
	{
		return (a.min.x < b.max.x &&
				a.max.x > b.min.x &&
				a.min.y < b.max.y &&
				a.max.y > b.min.y);
	}

	// Clear the quadtree
	void Clear()
	{
		objects.Clear();
		for (int i = 0; i < 4; i++)
		{
			if (nodes[i] != nullptr)
			{
				delete nodes[i];
				nodes[i] = nullptr;
			}
		}
	}

	bool HasSplit() const
	{
		return nodes[0] != nullptr;
	}
	int GetObjectCount() const
	{
		int count = objects.GetSize();

		for (int i = 0; i < 4; i++)
		{
			if (nodes[i] != nullptr)
			{
				count += nodes[i]->GetObjectCount();
			}
		}

		return count;
	}
	Rect GetBounds() const
	{
		return bounds;
	}
private:
	int maxObjects = 8;
	int maxLevels = 5;

	int level;

	Rect bounds;
	DynamicArray<CollisionObject> objects;
	Quadtree* nodes[4];
};