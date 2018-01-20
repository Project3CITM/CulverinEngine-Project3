#ifndef _QUADTREE_
#define _QUADTREE_

#include "Geometry/AABB.h"
#include <queue>
#include <list>

#define QUADTREE_MAX_ITEMS 4
#define QUADTREE_MIN_SIZE 5.0f

class GameObject;

class QuadtreeNode
{
public:
	QuadtreeNode(const AABB& box, QuadtreeNode* parent);
	virtual ~QuadtreeNode();

	bool isLeaf() const;

	void Insert(GameObject* obj);
	void Remove(GameObject* obj);

	void Clear();

	void DebugDraw();

	void CreateChilds();
	void DistributeObjects();

	template<typename TYPE>
	int CollectCandidates(std::queue<GameObject*>& objects, const TYPE& primitive) const;

	void CollectObjects(std::vector<GameObject*>& vec_to_fill);

public:

	AABB box;	// container to check colisions
	std::list<GameObject*> objects;	// Objects inside this node
	QuadtreeNode* parent = nullptr; // From which node comes this node
	QuadtreeNode* childs[4]; // 4 child nodes (it's a Quadtree)

};


class Quadtree
{
public:
	Quadtree();
	virtual ~Quadtree();

	void Init(float new_size);
	void Boundaries(AABB limits);
	void Clear();
	void Bake(const std::vector<GameObject*>& objects);
	void Insert(GameObject* obj);
	void Remove(GameObject* obj);

	void DebugDraw();

	template<typename TYPE>
	int CollectCandidates(std::queue<GameObject*>& objects, const TYPE& primitive) const;

	void CollectObjects(std::vector<GameObject*>& objects);

public:
	QuadtreeNode* root_node = nullptr;
	float size = 0;
	float3 min_size = float3::zero;
	float3 max_size = float3::zero;
};

template<typename TYPE>
inline int QuadtreeNode::CollectCandidates(std::queue<GameObject*>& candidates, const TYPE & primitive) const
{
	int ret = 0;

	// Check if primitive intersects with the node first
	if (!primitive.Intersects(box))
	{
		return ret;
	}

	// Then, check if primitive intersects with objects of this node
	for (std::list<GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if (primitive.Intersects((*it)->box_fixed))
		{
			// If intersects, add it to candidates vector
			candidates.push(*it);
		}
	}

	// If there is no children, end iteration
	if (childs[0] == nullptr)
	{
		return ret;
	}

	// Otherwise, repeat this process for its 4 children
	for (int i = 0; i < 4; ++i)
	{
		if (childs[i] != nullptr)
		{
			ret += childs[i]->CollectCandidates(candidates, primitive);
		}
	}

	return ret;
}

template<typename TYPE>
inline int Quadtree::CollectCandidates(std::queue<GameObject*>& objects, const TYPE& primitive) const
{
	int tests = 1;
	if (root_node != nullptr)
	{
		root_node->CollectCandidates(objects, primitive);
	}
	return tests;
}


#endif