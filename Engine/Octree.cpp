#include <algorithm>
#include "Octree.h"
#include "GameObject.h"
#include "GL3W/include/glew.h"



// Octree NODE -------------------------

OctreeNode::OctreeNode(const AABB& box) : box(box)
{
	parent = nullptr;

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

void OctreeNode::Insert(GameObject* obj, OctreeLimits& limits)
{
	AABB otherBox;
	otherBox = *obj->bounding_box;
	if (!box.Intersects(otherBox))
		return;

	if (isLeaf() && (objects.size() < limits.octreeMaxItems || (box.HalfSize().LengthSq() <= limits.octreeMinSize * limits.octreeMinSize)))
	{
		//std::list<ComponentMesh*>::iterator it = std::find(objects.begin(), objects.end(), mesh);
		//if (it == objects.end())
			objects.push_back(obj);
	}
		
	else
	{
		if (isLeaf())
		{
			if (childs[0] == nullptr)
				CreateChilds();
			for (std::list<GameObject*>::iterator item = objects.begin(); item != objects.cend(); ++item)
			{
				AABB meshBox;
				meshBox = item._Ptr->_Myval->box_fixed;
				for (uint i = 0; i < 8; i++)
				{
					if (childs[i]->box.Intersects(meshBox))
						childs[i]->Insert(item._Ptr->_Myval, limits);
				}
			}
			objects.clear();
		}
		for (uint i = 0; i < 8; i++)
			childs[i]->Insert(obj, limits);
	}
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
	childs[0] = new OctreeNode(box_new);

	// +X / -Y / -Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y - size_new.y * 0.5f, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[1] = new OctreeNode(box_new);

	// -X / +Y / -Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y + size_new.y * 0.5f, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[2] = new OctreeNode(box_new);

	// -X / -Y / +Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y - size_new.y * 0.5f, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[3] = new OctreeNode(box_new);

	// +X / +Y / +Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y + size_new.y * 0.5f, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[4] = new OctreeNode(box_new);

	// +X / +Y / -Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y + size_new.y * 0.5f, center.z - size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[5] = new OctreeNode(box_new);

	// +X / -Y / +Z
	center_new.Set(center.x + size_new.x * 0.5f, center.y - size_new.y * 0.5f, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[6] = new OctreeNode(box_new);

	// -X / +Y / +Z
	center_new.Set(center.x - size_new.x * 0.5f, center.y + size_new.y * 0.5f, center.z + size_new.z * 0.5f);
	box_new.SetFromCenterAndSize(center_new, size_new);
	childs[7] = new OctreeNode(box_new);
}

//template<typename TYPE>
int OctreeNode::CollectIntersections(std::list<GameObject*>& nodes, const Frustum& frustum) const
{
	uint ret = 0;

	// If range is not in the octree, return
	if (!box.Intersects(frustum))
		return ret;

	for (std::list<GameObject*>::const_iterator item = objects.begin(); item != objects.cend(); ++item)
	{
		std::list<GameObject*>::iterator it = std::find(nodes.begin(), nodes.end(), item._Ptr->_Myval);
		if (it != nodes.end())
				continue;
		ret++;
		AABB Box;
		Box = item._Ptr->_Myval->box_fixed;
		for (int i = 0; i < 8; ++i)
		{
			if (frustum.Contains(Box.CornerPoint(i)))
			{
				nodes.push_back(*item);
				break;
			}
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
	if (obj->bounding_box != nullptr)
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