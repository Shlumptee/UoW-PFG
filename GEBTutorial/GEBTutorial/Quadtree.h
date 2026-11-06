#pragma once

#include "Shape.h"
#include "Entity.h"

// Struct to store an entity that is collidable
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

	// Symbol operator to compare collision objects to each other, used in dynamic arrays when searching for a value
	bool operator==(CollisionObject other)
	{
		return entity == other.entity;
	}
};

// Class for managing quadtrees, which are a type of collision detection set up of splitting the screen into 4 quads and running collision between objects that belong to the same quad
// Each quad can be further broken down into subquads, etc etc until maximum level depth is reached
// Inspired by learning about quadtrees from this website: https://code.tutsplus.com/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space--gamedev-374t, had to convert from Java to C++ but implementation is similar
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
	// It will then move objects into the appropriate subnode
	void Insert(const CollisionObject& object)
	{
		if (nodes[0] != nullptr)
		{
			// Get the what level the bounding box belongs to
			int index = GetIndex(object.boundingBox);

			// If an object fully fits into a node, insert it there
			if (index != -1)
			{
				nodes[index]->Insert(object);
				return;
			}
		}

		// Objects that overlap multiple quadrants (eg: across the midpoint line between 2 quads) will remain in the current node instead of going to a subnode
		// Push to dynamic array so other components can access what is stored in the quadtree
		objects.Push(object);

		// Check if the current subnode has exceeded it's maximum number of objects and whether there is too many subnode levels present
		if (objects.GetSize() > maxObjects && level < maxLevels)
		{
			// Split current node into further subnodes
			if (nodes[0] == nullptr)
			{
				Split();
			}

			// Check objects stored in node and move them
			int i = 0;
			while (i < objects.GetSize())
			{
				// Find which quadrant the object belongs to, and run same check as earlier
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
		// Add all objects stored in the node into the return objects
		for (int i = 0; i < objects.GetSize(); i++)
		{
			returnObjects.Push(objects[i]);
		}

		// If the node hasn't been split, return as all objects will collide with each other and there are no subnodes to check
		if (nodes[0] == nullptr)
		{
			return;
		}

		// Check all 4 subnodes
		for (int i = 0; i < 4; i++)
		{
			const Rect& childBounds = nodes[i]->bounds;

			// Check whether the query object overlaps with the subnode
			bool isOverlapping = query.min.x < childBounds.max.x &&
				query.max.x > childBounds.min.x &&
				query.min.y < childBounds.max.y &&
				query.max.y > childBounds.min.y;

			// Only search for subnodes that overlap the query object
			if (isOverlapping)
			{
				nodes[i]->Retrieve(returnObjects, query);
			}
		}
	}

	// Check whether two rect colliders intersect
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
		// Loop through all quadrants of the tree and delete
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
	int maxObjects = 6;
	int maxLevels = 5;

	int level;

	Rect bounds;
	DynamicArray<CollisionObject> objects;
	Quadtree* nodes[4];
};