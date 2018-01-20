#include "Quadtree.h"
#include "GameObject.h"
#include "Geometry/AABB.h"
#include "GL3W/include/glew.h"
// QUADTREE NODE -------------------------

enum QuadTreeChild
{
	FRONT_LEFT = 0,
	FRONT_RIGHT,
	BACK_RIGHT,
	BACK_LEFT
};

QuadtreeNode::QuadtreeNode(const AABB& box, QuadtreeNode* parent) : box(box), parent(parent)
{
	for (uint i = 0; i < 4; i++)
	{
		childs[i] = nullptr;
	}
}

QuadtreeNode::~QuadtreeNode()
{
	for (uint i = 0; i < 4; i++)
	{
		if (childs[i] != nullptr)
		{
			childs[i]->objects.clear();
			RELEASE(childs[i]);
		}
	}
}

// To check if this node hasn't got childrens
bool QuadtreeNode::isLeaf() const
{
	if (childs[0] == nullptr)
	{
		return true;
	}
	return false;
}

void QuadtreeNode::Insert(GameObject* obj)
{
	// If the node has space for the gameobject, add it to its list
	if (isLeaf() && (objects.size() < QUADTREE_MAX_ITEMS ||
		(box.HalfSize().LengthSq() <= QUADTREE_MIN_SIZE * QUADTREE_MIN_SIZE)))
	{
		objects.push_back(obj);
		//LOG("Inserting %s into child %i", obj->GetName());
	}

	else
	{
		if (isLeaf())
		{
			//Divide the root node into 4 childs
			CreateChilds();

			objects.push_back(obj);
			// All gameobjects of the father node have to be inside at least one of the childs
			DistributeObjects();
		}
		else
		{
			for (uint i = 0; i < 4; i++)
			{
				if (childs[i]->box.Intersects(obj->box_fixed))
				{
					childs[i]->Insert(obj);
				}
			}
		}
	}
}

void QuadtreeNode::Remove(GameObject* obj)
{
	std::list<GameObject*>::iterator it = std::find(objects.begin(), objects.end(), obj);

	if (it != objects.end())
	{ 
		objects.erase(it);
	}

	if (!isLeaf())
	{
		for (uint i = 0; i < 4; i++)
		{
			childs[i]->Remove(obj);
		}
	}
}

void QuadtreeNode::Clear()
{
	if (childs[0] != nullptr)
	{
		for (uint i = 0; i < 4; i++)
		{
			childs[i]->Clear();
			RELEASE(childs[i]);
		}
	}
	objects.clear();
}

void QuadtreeNode::DebugDraw()
{
	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(box.Edge(i).a.x, box.Edge(i).a.y, box.Edge(i).a.z);
		glVertex3f(box.Edge(i).b.x, box.Edge(i).b.y, box.Edge(i).b.z);
	}

	if (childs[0] != nullptr)
	{
		for (uint i = 0; i < 4; i++)
		{
			childs[i]->DebugDraw();
		}
	}
}

// Subdivide the node into 4 childs
void QuadtreeNode::CreateChilds()
{
	// We divide the node into 4 equal parts
	float3 size(box.Size());
	float3 size_new(size.x*0.5f, size.y, size.z*0.5f);

	float3 center(box.CenterPoint());
	float3 center_new;
	
	AABB box_new;

	// -X / -Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[FRONT_LEFT] = new QuadtreeNode(box_new, this);

	// +X / -Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[FRONT_RIGHT] = new QuadtreeNode(box_new, this);

	// +X / +Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[BACK_RIGHT] = new QuadtreeNode(box_new, this);

	// -X / +Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[BACK_LEFT] = new QuadtreeNode(box_new, this);
}

// We distribute the Game Objects depending on its position respect to the new childs
void QuadtreeNode::DistributeObjects()
{	
	GameObject* object = nullptr;

	std::list<GameObject*>::iterator it;
	for (it = objects.begin(); it != objects.end();)
	{
		object = *it;

		// Check intersections between all 4 childs and the Game Object
		bool intersecting[4];
		uint num_intersections = 0;
		for (uint i = 0; i < 4; i++)
		{
			if (intersecting[i] = childs[i]->box.Intersects(object->box_fixed))
			{
				num_intersections++;
			}
		}

		if (num_intersections == 4)
		{
			it++; 
		}
		else
		{
			// Erase this game object from the father list to add it to childs nodes
			it = objects.erase(it); 
			for (uint i = 0; i < 4; i++)
			{
				if (intersecting[i])
				{
					// Insert the Game Object into the correct child
					childs[i]->Insert(object);
				}
			}
		}
	}
}
void QuadtreeNode::CollectObjects(std::vector<GameObject*>& vec_to_fill)
{
	// Fill the vector with the objects of this node
	for (std::list<GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		vec_to_fill.push_back(*it);
	}

	// If it has no children, end
	if (childs[0] == nullptr)
	{
		return;
	}

	// Otherwise, repeat this process for its 4 children
	for (int i = 0; i < 4; ++i)
	{
		if (childs[i] != nullptr)
		{
			childs[i]->CollectObjects(vec_to_fill);
		}
	}
}
// ---------------------------------------


// QUADTREE ------------------------------
Quadtree::Quadtree()
{
}

Quadtree::~Quadtree()
{
	RELEASE(root_node);
}

void Quadtree::Init(float new_size)
{
	size = new_size;
	min_size.Set(-size, -size, -size);
	max_size.Set(size, size, size);
	Boundaries(AABB(min_size, max_size));
}

void Quadtree::Boundaries(AABB limits)
{
	Clear();
	root_node = new QuadtreeNode(limits,nullptr);
}

void Quadtree::Clear()
{
	if (root_node != nullptr)
	{
		root_node->Clear();
		root_node->objects.clear();
		RELEASE(root_node);
	}	
}

void Quadtree::Bake(const std::vector<GameObject*>& objects)
{
	for (uint i = 0; i < objects.size(); i++)
	{
		if (objects[i]->isActive())
		{
			if (objects[i]->isStatic())
			{
				Insert(objects[i]);
			}
			if (objects[i]->GetNumChilds() > 0)
			{
				Bake(objects[i]->GetChildsVec());
			}
		}
	}
}

void Quadtree::Insert(GameObject* obj)
{
	if (root_node != nullptr)
	{
		// If object is inside the Boundaries & has an AABB, insert it in a node
		if (obj->bounding_box != nullptr)
		{
			if (obj->box_fixed.Intersects(root_node->box))
			{
				root_node->Insert(obj);
			}
		}
		else
		{
			LOG("The Game Object you are trying to Insert hasn't got an AABB.");
		}
	}
}

void Quadtree::Remove(GameObject* obj)
{
	if (root_node != nullptr)
	{
		root_node->Remove(obj);
	}
}

void Quadtree::DebugDraw()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(1.00f, 0.761f, 0.00f, 1.00f);

	if(root_node != nullptr)
	{
		root_node->DebugDraw();
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

// Fill a vector with all the objects inside the quadtree
void Quadtree::CollectObjects(std::vector<GameObject*>& vec_to_fill)
{
	if (root_node != nullptr)
	{
		root_node->CollectObjects(vec_to_fill);
	}
}

// --------------------------------------
