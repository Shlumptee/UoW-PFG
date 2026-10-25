#pragma once

#include <iostream>

#include "Quadtree.h"
#include "Entity.h"

class CollisionManager
{
public:
	void Update(Quadtree& quadtree, DynamicArray<VisualEntity*>& entities)
	{
        for (int i = 0; i < entities.GetSize(); i++)
        {
            VisualEntity* entity = entities[i];

            DynamicArray<CollisionObject> candidates;

            quadtree.Retrieve(candidates, entity->GetBoundingBox());

            //std::cout << "Entity " << i << " has " << candidates.GetSize() << " potential collision candidates.\n";

            for (int c = 0; c < candidates.GetSize(); c++)
            {
                CollisionObject& candidate = candidates[c];

                if (candidate.entity == nullptr || candidate.entity == entity)
                {
                    continue;
                }

                int otherIndex = -1;

                for (int j = 0; j < entities.GetSize(); j++)
                {
                    if (entities[j] == candidate.entity)
                    {
                        otherIndex = j;
                        break;
                    }
                }

                if (otherIndex <= i)
                {
                    continue;
                }

                if (quadtree.Intersects(entity->GetBoundingBox(), candidate.boundingBox))
                {
                    std::cout << "Collision detected between entity " << i << " and entity " << otherIndex << std::endl;
                    OnCollision(entity, candidate.entity);
                }
            }
        }
	}

private:
    void OnCollision(VisualEntity* entityA, VisualEntity* entityB)
    {
        entityA->OnCollision(entityB);
        entityB->OnCollision(entityA);
    }
};