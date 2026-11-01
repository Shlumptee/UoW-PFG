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
		float midX = (bounds.min.x + bounds.max.x) / 2.0f;
		float midY = (bounds.min.y + bounds.max.y) / 2.0f;

		// Init new nodes
		// Top-right
		nodes[0] = new Quadtree(level + 1, Rect(Vector2(midX, bounds.min.y), Vector2(bounds.max.x, midY)));
		// Top-left
		nodes[1] = new Quadtree(level + 1, Rect(Vector2(bounds.min.x, bounds.min.y), Vector2(midX, midY)));
		// Bottom-left
		nodes[2] = new Quadtree(level + 1, Rect(Vector2(bounds.min.x, midY), Vector2(midX, bounds.max.y)));
		// Bottom-right
		nodes[3] = new Quadtree(level + 1, Rect(Vector2(midX, midY), Vector2(bounds.max.x, bounds.max.y)));
	}

	// Determine which node an object belongs to
	// -1 = object cannot fit within child node and is instead a part of parent node
	int GetIndex(Rect rect)
	{
		float midX = (bounds.min.x + bounds.max.x) / 2.0f;
		float midY = (bounds.min.y + bounds.max.y) / 2.0f;

		bool left = rect.max.x <= midX;
		//std::cout << "Left: " << left << std::endl;
		bool right = rect.min.x >= midX;

		bool upper = rect.max.y <= midY;
		bool lower = rect.min.y >= midY;

		//std::cout << "Lower: " << lower << std::endl;

		if (left)
		{
			if (upper)
			{
				// Top-left
				return 1;
			}
			else if (lower)
			{
				// Bottom-left
				return 2;
			}
		}
		else if (right)
		{
			if (upper)
			{
				// Top-right
				return 0;
			}
			else if (lower)
			{
				// Bottom-right
				return 3;
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
	void SetBounds(Rect _bounds)
	{
		bounds = _bounds;
	}
	Rect GetBounds() const
	{
		return bounds;
	}
private:
	int maxObjects = 10;
	int maxLevels = 5;

	int level;

	Rect bounds;
	DynamicArray<CollisionObject> objects;
	Quadtree* nodes[4];
};