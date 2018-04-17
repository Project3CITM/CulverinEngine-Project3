#include <algorithm>
#include "Octree.h"
#include "GameObject.h"
#include "GL3W/include/glew.h"



// Octree NODE -------------------------

OctreeNode::OctreeNode(const AABB& box, OctreeNode* parent) : box(box), parent(parent)
{
	for (uint i = 0; i < 8; i++)
		childs[i] = nullptr;
}

OctreeNode::~OctreeNode()
{
	for (uint i = 0; i < 8; i++)
		if (childs[i] != nullptr)
		{
			childs[i]->objects.clear();
			RELEASE(childs[i]);
		}
}

bool OctreeNode::isLeaf() const
{
	if (childs[0] == nullptr)
		return true;

	return false;
}

bool OctreeNode::Insert(GameObject* obj, OctreeLimits& limits)
{
	AABB otherBox;
	otherBox = obj->box_fixed;
	if (!box.IsFinite() || !box.Intersects(otherBox))
		return false;

	
	if (isLeaf() && box.Contains(otherBox) && (objects.size() < limits.octreeMaxItems || (box.HalfSize().LengthSq() <= limits.octreeMinSize * limits.octreeMinSize)) )
	{
		objects.push_back(obj);
		return true;
	}
	else
	{
		//Check if insert failed becouse box is too big, if so try to insert it to parent node
		if (!box.Contains(otherBox))
		{
			if (parent != nullptr)
				return parent->Insert(obj, limits);
			else
			{
				LOG("Object too big to enter octree");
				return false;
			}
		}
		
		//If object is inside octree and failed beacouse the octree needs subdivision divide the node
		if (isLeaf())
		{
			CreateChilds();

			for (std::list<GameObject*>::iterator item = objects.begin(); item != objects.cend();)
			{
				uint node_to_push = 0;
				uint multiple_collision = 0;
				for (uint i = 0; i < 8; i++)
				{
					if (childs[i]->box.Intersects(item._Ptr->_Myval->box_fixed))
					{
						multiple_collision++;
						node_to_push = i;
					}
				}
				if (multiple_collision == 1)
				{
					childs[node_to_push]->Insert(item._Ptr->_Myval, limits);
					item = objects.erase(item);
				}
				else
				{
					item++;
				}
			}

			
		}
		else if (box.Contains(otherBox))
		{
			bool push_in_childs = false;
			//If one of the childs can containt the box contine to insert in childs
			for (uint i = 0; i < 8; i++)
			{
				if (childs[i]->box.Contains(otherBox))
					push_in_childs = true;
			}
			
			if (!push_in_childs)
			{
				objects.push_back(obj);
				return true;
			}
		}

		for (uint i = 0; i < 8; i++)
		{
			if (childs[i]->Insert(obj, limits))
				return true;
		}
	}

	return false;
}

void OctreeNode::Remove(GameObject* obj)
{
	std::list<GameObject*>::iterator it = std::find(objects.begin(), objects.end(), obj);

	if (it != objects.end())
		objects.erase(it);

	if (!isLeaf())
		for (uint i = 0; i < 8; i++)
			childs[i]->Remove(obj);
}

void OctreeNode::DebugDraw()
{
	//App->renderer3D->DrawDebugBox(box.CornerPoint(0), box.CornerPoint(1), box.CornerPoint(2), box.CornerPoint(3), box.CornerPoint(4), box.CornerPoint(5), box.CornerPoint(6), box.CornerPoint(7));
	
	
	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(box.Edge(i).a.x, box.Edge(i).a.y, box.Edge(i).a.z);
		glVertex3f(box.Edge(i).b.x, box.Edge(i).b.y, box.Edge(i).b.z);
	}
	
	if (childs[0] != nullptr)
		for (uint i = 0; i < 8; i++)
			childs[i]->DebugDraw();
			
}

void OctreeNode::CreateChilds()
{
	//LOGP("subdivide");
	// We divide the node into 8 equal parts
	float3 size_new = box.HalfSize();

	float3 center(box.CenterPoint());
	float3 center_new;

	AABB box_new;

	// -X / -Y / -Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y - size_new.y * 0.5f, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[0] = new OctreeNode(box_new, this);

	// +X / -Y / -Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y - size_new.y * 0.5f, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[1] = new OctreeNode(box_new, this);

	// -X / +Y / -Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y + size_new.y * 0.5f, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[2] = new OctreeNode(box_new, this);

	// -X / -Y / +Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y - size_new.y * 0.5f, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[3] = new OctreeNode(box_new, this);

	// +X / +Y / +Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y + size_new.y * 0.5f, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[4] = new OctreeNode(box_new, this);

	// +X / +Y / -Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y + size_new.y * 0.5f, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[5] = new OctreeNode(box_new, this);

	// +X / -Y / +Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y - size_new.y * 0.5f, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[6] = new OctreeNode(box_new, this);

	// -X / +Y / +Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y + size_new.y * 0.5f, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[7] = new OctreeNode(box_new, this);
}

//template<typename TYPE>
int OctreeNode::CollectIntersections(std::list<GameObject*>& nodes, const Frustum& frustum) const
{
	uint ret = 0;

	// If range is not in the octree, return
	if (!box.Intersects(frustum))
		return ret;
	
	if (frustum.Contains(box))
	{
		return ret += CollectAllChilds(nodes);
	}

	Plane planes[6];
	frustum.GetPlanes(planes);
	AABB Box;
	//Custom intersec, MathGeoLib was too slow
	//Should be changed if there is more time
	for (std::list<GameObject*>::const_iterator item = objects.begin(); item != objects.cend(); ++item)
	{
		
		AABB Box = item._Ptr->_Myval->box_fixed;

		int iTotalIn = 0;
		bool is_in = true;
		for (int p = 0; p < 6; ++p) {
			int iInCount = 8;
			int iPtIn = 1;
			for (int i = 0; i < 8; ++i) {
				// test this point against the planes
				if (planes[p].normal.Dot(Box.CornerPoint(i)) >= planes[p].d) {
					iPtIn = 0;
					--iInCount;
				}
			}
			// were all the points outside of plane p?
			if (iInCount == 0)
			{
				is_in = false;
				break;
			}
			// check if they were all on the right side of the plane
			iTotalIn += iPtIn;
		}

		if (is_in)
		{
			nodes.push_back(*item);
		}
	
	}

	// If there is no children, end
	if (isLeaf())
		return ret;

	// Otherwise, add the points from the children
	for (uint i = 0; i < 8; i++)
		ret += childs[i]->CollectIntersections(nodes, frustum);

	return ret;
}

int OctreeNode::CollectAllChilds(std::list<GameObject*>& nodes) const
{
	int ret = 0;

	for (std::list<GameObject*>::const_iterator item = objects.begin(); item != objects.cend(); ++item)
	{
		nodes.push_back(*item);
	}

	if (isLeaf())
		return ret;

	for (uint i = 0; i < 8; i++)
		ret += childs[i]->CollectAllChilds(nodes);

	return ret;
}

void OctreeNode::CollectAllObjects(std::list<GameObject*>& all_obj) const
{
	for (std::list<GameObject*>::const_iterator item = objects.begin(); item != objects.cend(); ++item)
	{
		std::list<GameObject*>::iterator it = std::find(all_obj.begin(), all_obj.end(), item._Ptr->_Myval);
		if (it != all_obj.end())
			continue;
		else
			all_obj.push_back(*item);
	}

	// If there is no children, end
	if (isLeaf())
		return;

	// Otherwise, add the points from the children
	for (uint i = 0; i < 8; i++)
		childs[i]->CollectAllObjects(all_obj);
}

// Octree ------------------------------
Octree::Octree()
{
}

Octree::~Octree()
{
	Clear();
}

void Octree::Boundaries(AABB limits)
{
	Clear();
	root_node = new OctreeNode(limits);
}

void Octree::Clear(bool fullclear)
{
	if (fullclear)
	{
		if (root_node != nullptr)
			root_node->objects.clear();
		RELEASE(root_node);
	}
	else
	{
		if ((root_node != nullptr) && (root_node->childs[0] != nullptr))
		{
			for (uint i = 0; i < 8; i++)
				RELEASE(root_node->childs[i]);
		}
		if (root_node != nullptr)
			root_node->objects.clear();
	}
	
}

void Octree::Remove(GameObject* mesh)
{
	if (root_node != nullptr)
		root_node->Remove(mesh);
}

void Octree::Insert(GameObject* obj)
{
	if (obj->box_fixed.IsFinite())
		root_node->Insert(obj, limits);
}


void Octree::DebugDraw()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor3f(1.00f, 0.761f, 0.00f);

	if (root_node != nullptr)
		root_node->DebugDraw();

	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
}

//template<typename TYPE>
int Octree::CollectIntersections(std::list<GameObject*>& nodes, const Frustum& frustum) const
{
	int tests = 1;

	if (root_node != nullptr)
		tests = root_node->CollectIntersections(nodes, frustum);
	return tests;
}

void Octree::CollectAllObjects(std::list<GameObject*>& all_obj) const
{
	if (root_node != nullptr)
		root_node->CollectAllObjects(all_obj);
}

// --------------------------------------