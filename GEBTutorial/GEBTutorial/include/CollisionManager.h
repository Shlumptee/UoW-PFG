#pragma once

#include <iostream>

#include "Quadtree.h"
#include "Entity.h"

class CollisionManager
{
public:
    // Run checks to handle any possible collisions
	void Update(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities)
	{
        // Loop through every entity in the dynamic array
        for (int i = 0; i < entities.GetSize(); i++)
        {
            VisualEntity* entity = entities[i];
            // Store potential candidates here
            DynamicArray<CollisionObject> candidates;

            // Retrieve objects from the quadtree that could overlap with this entity
            quadtree.Retrieve(candidates, entity->GetBoundingBox());

            //std::cout << "Entity " << i << " has " << candidates.GetSize() << " potential collision candidates.\n";

            // Loop through potential candidates
            for (int c = 0; c < candidates.GetSize(); c++)
            {
                CollisionObject& candidate = candidates[c];

                // Ensure candidate is not invalid or the entity itself
                if (candidate.entity == nullptr || candidate.entity == entity)
                {
                    continue;
                }

                int otherIndex = -1;

                // Find the candidate's position in the dynamic array in order to avoid checking collisions twice
                for (int j = 0; j < entities.GetSize(); j++)
                {
                    if (entities[j] == candidate.entity)
                    {
                        otherIndex = j;
                        break;
                    }
                }

                // If the other entity has already been checked with this entity (which it would have if it's index is lower than the current entity), skip past it
                if (otherIndex <= i)
                {
                    continue;
                }

                // Check to see if the bounding boxes intersect with each other, if so there is a collision
                if (quadtree.Intersects(entity->GetBoundingBox(), candidate.boundingBox))
                {
                    //std::cout << "Collision detected between entity " << i << " and entity " << otherIndex << std::endl;
                    // Call collision resolution for both entities
                    OnCollision(entity, candidate.entity);
                }
            }
        }
	}

private:
    // Calls OnCollision for both entities so they can resolve the collision
    void OnCollision(VisualEntity* entityA, VisualEntity* entityB)
    {
        entityA->OnCollision(entityB);
        entityB->OnCollision(entityA);
    }
};