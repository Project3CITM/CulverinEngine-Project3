#ifndef _MODULECAMERA_
#define _MODULECAMERA_

#include "Module.h"
#include "Globals.h"
#include "Geometry/LineSegment.h"
#include <map>

#define MARGE_MIN 5
#define MARGE_MAX 25

class GameObject;
class CompCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(JSON_Object* node);
	bool Start();
	//update_status PreUpdate(float dt);
	update_status Update(float dt);
	//update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);
	bool CleanUp();

	// Camera Movement/Rotation -----------------------
	void MoveWithKeyboard(float dt);
	void MoveWithMouse(int dx, int dy, float dt);

	void Orbit(float dx, float dy);
	void LookAt(const float3& spot);
	void LookAround(float dx, float dy);
	void Zoom(float zoom);
	// --------------------------------------------

	// Picking objects with mouse -----------------------
	void MousePick(float x, float y, float w, float h);
	void CheckAABBIntersection(GameObject* candidate, float& entry_dist, float& exit_dist);
	void CheckGeometryIntersection();
	// --------------------------------------------

	// Focus (Object selected) methods ----------------------------------
	void SetFocus(const GameObject* selected);
	const GameObject* GetFocus() const;
	void SetFocusNull();
	void CenterToObject();
	// --------------------------------------------

	bool isMouseOnWindow();

	float* GetViewMatrix() const;
	float* GetProjMatrix() const;
	float3 GetPos() const;

private:
	void CheckOut();

public:
	bool changecam = false;
	bool CanMoveCamera = false;
	LineSegment ray;

private:
	CompCamera* cam = nullptr;
	const GameObject* focus = nullptr;
	float3 point_to_look = { 0, 0, 0 };

	/* Mouse Picking */
	std::map<float, GameObject*> possible_intersections;
	const AABB* box = nullptr;
	float norm_x = 0.0f;
	float norm_y = 0.0f;
	float entry_dist = 0.0f;
	float exit_dist = ray.Length();
	bool hit = false;
	float3 hit_point = float3::zero;
	float min_distance = INFINITY;
	LineSegment ray_local_space = ray;
	GameObject* best_candidate = nullptr;
	std::map<float, GameObject*>::iterator it;

	/* Camera Movement */
	float3 cam_move = { 0, 0, 0 };
	float move_speed = 1.0f;
	float rotate_speed = 1.0f;
	float scroll_speed = 1.0f;

	bool canOut = false;
};

#endif