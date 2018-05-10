#include "OclusionCulling.h"

#include "GameObject.h"


OclusionCulling::OclusionCulling()
{
	g_map.resize(50);
	for (uint i = 0; i < 50; i++)
	{
		g_map[i].resize(50);
	}
}

OclusionCulling::~OclusionCulling()
{
	// Delete Vector
	for (uint i = 0; i < 50; i++)
	{
		for (uint j = 0; j < 50; j++)
			g_map[i][j].clear();
		g_map[i].clear();
	}
	g_map.clear();
}

void OclusionCulling::Clear()
{
	// Clear GameObjects
	for (uint i = 0; i < 50; i++)
		for (uint j = 0; j < 50; j++)
			g_map[i][j].clear();
}

void OclusionCulling::LoadOclusionMap(int map[100][100], int width, int height)
{
	int max_x = 0;
	if (width >= 49) max_x = 48;
	else if (width < 0) max_x = 0;
	else max_x = width;

	int max_y = 0;
	if (height >= 49) max_y = 48;
	else if (height < 0) max_y = 0;
	else max_y = height;

	for (int x = 0; x < max_x; x++)
	{
		for(int y = 0; y < max_y; y++)
		{
			if (map[x][y] == 0)
			{
				o_map[x][y].ocludes = true;
			}
			else if (map[x][y] == 1)
			{
				o_map[x][y].state = TILE_IGNORE;
			}
		}
	}

}

void OclusionCulling::InsertCandidate(GameObject * obj)
{
	if (obj->box_fixed.IsFinite())
	{
		float3 center_pos = obj->box_fixed.CenterPoint();
		uint x = (uint)((center_pos.x + 12.7f) / tile_size);
		uint y = (uint)((center_pos.z + 12.7f) / tile_size);

		if (x >= 0 && x < 50 && y >= 0 && y < 50)
		{
			g_map[x][y].push_back(obj);
		}
	}
}

void OclusionCulling::AddTilesToQueue()
{
	float2 curr = float2::zero;
	float2 adj = float2::zero;

	std::queue<float2> open;

	// Push Origin
	open.push(origin);
	o_map[(int)origin.x][(int)origin.y].added_to_queue = true;

	while (!open.empty())
	{
		curr = open.front();
		open.pop();

		// Check Tile State
		bfs_oclusion.push(curr);

		if (o_map[(int)curr.x][(int)curr.y].ocludes)
		{
			OcludeTiles(curr);
		}

		// Add Adjacent Tiles
		adj = curr + dir.forward;
		if (o_map[(int)adj.x][(int)adj.y].state == TILE_TO_CHECK && !o_map[(int)adj.x][(int)adj.y].added_to_queue)
		{
			o_map[(int)adj.x][(int)adj.y].added_to_queue = true;
			open.push(adj);
		}
		
		adj = curr + dir.right;
		if (o_map[(int)adj.x][(int)adj.y].state == TILE_TO_CHECK && !o_map[(int)adj.x][(int)adj.y].added_to_queue)
		{
			o_map[(int)adj.x][(int)adj.y].added_to_queue = true;
			open.push(adj);
		}
		
		adj = curr + dir.left;
		if (o_map[(int)adj.x][(int)adj.y].state == TILE_TO_CHECK && !o_map[(int)adj.x][(int)adj.y].added_to_queue)
		{
			o_map[(int)adj.x][(int)adj.y].added_to_queue = true;
			open.push(adj);
		}
		
		adj = curr - dir.forward;
		if (o_map[(int)adj.x][(int)adj.y].state == TILE_TO_CHECK && !o_map[(int)adj.x][(int)adj.y].added_to_queue)
		{
			o_map[(int)adj.x][(int)adj.y].added_to_queue = true;
			open.push(adj);
		}
	}
}

void OclusionCulling::CheckOclusionMap(Frustum &frustum)
{
	RevertOclusionFrame();

	AABB frust_box = frustum.MinimalEnclosingAABB();

	minpos.x = (frust_box.minPoint.x + 12.7f) / tile_size;
	minpos.y = (frust_box.minPoint.z + 12.7f) / tile_size;

	(minpos.x < 0) ? minpos.x = 1 : minpos.x = (uint)minpos.x;
	(minpos.y < 0) ? minpos.y = 1 : minpos.y = (uint)minpos.y;

	maxpos.x = (frust_box.maxPoint.x + 12.7f) / tile_size;
	maxpos.y = (frust_box.maxPoint.z + 12.7f) / tile_size;

	(maxpos.x < 0) ? maxpos.x = 1 : maxpos.x = (uint)(maxpos.x+0.95);
	(maxpos.y < 0) ? maxpos.y = 1 : maxpos.y = (uint)(maxpos.y+0.95);

	origin.x = (frustum.pos.x + 12.7f) / tile_size;
	if (origin.x >= 49) origin.x = 48;
	else if (origin.x <= 0) origin.x = 1;
	else origin.x = (uint)origin.x;

	origin.y = (frustum.pos.z + 12.7f) / tile_size;
	if (origin.y >= 49) origin.y = 48;
	else if (origin.y <= 0) origin.y = 1;
	else origin.y = (uint)origin.y;

	float3 front = frustum.front;

	// up, right, down
	if (front.x >= 0)
	{
		if (front.z >= 0)
		{
			if (front.x >= front.z)
			{
				curr_orientation = OCLUDE_RIGHT;
			}
			else
			{
				curr_orientation = OCLUDE_DOWN;
			}
		}
		else
		{
			if (front.x >= -front.z)
			{
				curr_orientation = OCLUDE_RIGHT;
			}
			else
			{
				curr_orientation = OCLUDE_UP;
			}
		}
	}
	// up, left, down
	else
	{
		if (front.z >= 0)
		{
			if (front.x <= -front.z)
			{
				curr_orientation = OCLUDE_LEFT;
			}
			else
			{
				curr_orientation = OCLUDE_DOWN;
			}
		}
		else
		{
			if (front.x <= front.z)
			{
				curr_orientation = OCLUDE_LEFT;
			}
			else
			{
				curr_orientation = OCLUDE_UP;
			}
		}
	}

	switch (curr_orientation)
	{
	case OCLUDE_UP:
		dir.left.Set(-1, 0);
		dir.forward.Set(0, -1);
		dir.right.Set(1, 0);
		break;
	case OCLUDE_DOWN:
		dir.left.Set(1, 0);
		dir.forward.Set(0, 1);
		dir.right.Set(-1, 0);
		break;
	case OCLUDE_LEFT:
		dir.left.Set(0, 1);
		dir.forward.Set(-1, 0);
		dir.right.Set(0, -1);
		break;
	case OCLUDE_RIGHT:
		dir.left.Set(0, -1);
		dir.forward.Set(1, 0);
		dir.right.Set(0, 1);
		break;
	}

	// Set Tiles inside Frustum to Draw
	SetOclusionFrame(frustum);

	// Chose Tiles to draw
	AddTilesToQueue();

	// Check Oclusion
	DrawOcludedScene();
}

void OclusionCulling::DrawOcludedScene()
{
	float2 curr;
	while (!bfs_oclusion.empty())
	{
		curr = bfs_oclusion.front();
		bfs_oclusion.pop();

		DrawTile((uint)curr.x,(uint)curr.y);
	}
}

void OclusionCulling::DrawTile(uint curr_x, uint curr_y)
{
	if (g_map.size() > curr_x && g_map[curr_x].size() > curr_y)
	{
		const int num = g_map[curr_x][curr_y].size();
		GameObject** ptr = (num > 0) ? g_map[curr_x][curr_y].data() : nullptr;
		for (int i = 0; i < num; i++)
		{
			ptr[i]->SetVisible(true);
			ptr[i]->Draw();
		}
	}
}

void OclusionCulling::SetOclusionFrame(Frustum &frust)
{
	
	Plane planes[6];
	frust.GetPlanes(planes);

	float3 point = frust.pos;
	for (uint x = minpos.x; x < maxpos.x; x++)
	{
		for (uint y = minpos.y; y < maxpos.y; y++)
		{
			if (o_map[x][y].state != TILE_IGNORE)
			{
				bool is_in = true;
				for (int p = 0; p < 4; ++p) {
					int iInCount = 4;

					point.x = x * tile_size - 12.7f;
					point.z = y * tile_size - 12.7f;
					if (planes[p].normal.Dot(point) >= planes[p].d)
					{
						--iInCount;
					}

					point.x += tile_size;
					if (planes[p].normal.Dot(point) >= planes[p].d)
					{
						--iInCount;
					}

					point.z += tile_size;
					if (planes[p].normal.Dot(point) >= planes[p].d)
					{
						--iInCount;
					}

					point.x -= tile_size;
					if (planes[p].normal.Dot(point) >= planes[p].d)
					{
						--iInCount;
					}

					// were all the points outside of plane p?
					if (iInCount == 0)
					{
						is_in = false;
						break;
					}
				}

				if (is_in)
				{
					o_map[x][y].state = TILE_TO_CHECK;
				}
			}
		}
	}

}

void OclusionCulling::RevertOclusionFrame()
{
	for (uint x = minpos.x; x < maxpos.x; x++)
	{
		for (uint y = minpos.y; y < maxpos.y; y++)
		{
			if (o_map[x][y].state != TILE_IGNORE)
			{
				o_map[x][y].state = TILE_SKIP;
				o_map[x][y].added_to_queue = false;
			}
		}
	}
}

void OclusionCulling::OcludeTiles(float2 tile_pos)
{
	float2 curr_pos = tile_pos;
	switch (curr_orientation)
	{
	case OCLUDE_UP:
		if (curr_pos.x == origin.x)
		{
			OcludeFront(curr_pos);
		}
		else if (curr_pos.x < origin.x)
		{
			OcludeLeft(curr_pos);
		}
		else
		{
			OcludeRight(curr_pos);
		}
		break;
	case OCLUDE_DOWN:
		if (curr_pos.x == origin.x)
		{
			OcludeFront(curr_pos);

		}
		else if (curr_pos.x > origin.x)
		{
			OcludeLeft(curr_pos);
		}
		else
		{
			OcludeRight(curr_pos);
		}
		break;
	case OCLUDE_LEFT:
		if (curr_pos.y == origin.y)
		{
			OcludeFront(curr_pos);
		}
		else if (curr_pos.y > origin.y)
		{
			OcludeLeft(curr_pos);
		}
		else
		{
			OcludeRight(curr_pos);
		}
		break;
	case OCLUDE_RIGHT:
		if (curr_pos.y == origin.y)
		{
			OcludeFront(curr_pos);
		}
		else if (curr_pos.y < origin.y)
		{
			OcludeLeft(curr_pos);
		}
		else
		{
			OcludeRight(curr_pos);
		}
		break;
	}
}

void OclusionCulling::OcludeFront(float2 curr_pos)
{
	curr_pos += dir.forward;
	while (o_map[(int)curr_pos.x][(int)curr_pos.y].state == TILE_TO_CHECK)
	{
		o_map[(int)curr_pos.x][(int)curr_pos.y].state = TILE_SKIP;
		curr_pos += dir.forward;
	}
}

void OclusionCulling::OcludeRight(float2 curr_pos)
{
}

void OclusionCulling::OcludeLeft(float2 curr_pos)
{
}
