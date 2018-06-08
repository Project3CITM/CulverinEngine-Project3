#ifndef _OCLUSION_CULLING_H_
#define _OCLUSION_CULLING_H_

#include <vector>
#include <queue>
#include "Geometry\Frustum.h"
#include "Geometry\AABB.h"
#include "Globals.h"

class GameObject;

enum Oclussion_orientation
{
	OCLUDE_UP,
	OCLUDE_DOWN,
	OCLUDE_LEFT,
	OCLUDE_RIGHT
};

enum Tile_State
{
	TILE_IGNORE,
	TILE_SKIP,
	TILE_TO_CHECK
};

struct bfs_dir
{
	float2 left = float2::zero;
	float2 forward = float2::zero;
	float2 right = float2::zero;
};

struct tile_data
{
	Tile_State state = TILE_SKIP;
	bool ocludes = false;
	bool added_to_queue = false;
};

class OclusionCulling
{
public:
	OclusionCulling();
	~OclusionCulling();

	void Clear();

	void LoadOclusionMap(int map[100][100], int width, int height);

	void InsertCandidate(GameObject* obj);

	void AddTilesToQueue();
	void CheckOclusionMap(Frustum &frustum);
	void DrawOcludedScene();

	void DrawTile(uint curr_x,uint curr_y);

	void SetOclusionFrame(Frustum &frust);
	void RevertOclusionFrame();

	void OcludeTiles(float2 tile_pos);
	void OcludeFront(float2 curr_pos);
	void OcludeRight(float2 curr_pos);
	void OcludeLeft(float2 curr_pos);

private:

	tile_data o_map[50][50];

	std::vector<std::vector<std::vector<GameObject*>>> g_map;

	std::queue<float2> bfs_oclusion;

	float tile_size = 25.4f;
	bool map_loaded = false;

	float2 minpos = float2::zero;
	float2 maxpos = float2::zero;

	float2 origin = float2::zero;
	float2 fpos = float2::zero;

	Oclussion_orientation curr_orientation = Oclussion_orientation::OCLUDE_DOWN;

	bfs_dir dir;
};

#endif
