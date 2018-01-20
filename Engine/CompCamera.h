#ifndef _COMPONENT_CAMERA_
#define _COMPONENT_CAMERA_

#include "Component.h"
#include "Geometry/Frustum.h"
#include <queue>

class GameObject;

enum Culling
{
	CULL_IN = 0,
	CULL_OUT,
	CULL_INTERSECT
};

class CompCamera : public Component
{
public:
	CompCamera(Comp_Type t, GameObject* parent);
	CompCamera(const CompCamera& copy, GameObject* parent);
	~CompCamera();

	void preUpdate(float dt);
	void Update(float dt);
	void UpdateFrustum();
	void Draw();
	void DebugDraw();

	void Clear();

	void ShowOptions();
	void ShowInspectorInfo();
	void ShowCameraPopup();

	void DoCulling();
	void CullStaticObjects();
	void CullDynamicObjects();
	void UnCull();

	void LookAt(const float3& position);

	Culling ContainsAABox(const AABB& refBox) const;

	void SetMain(bool isMain);
	void SetPos(float3 pos);
	void SetNear(float near_p);
	void SetFar(float far_p);
	void SetFov(float vertical);
	void SetRatio(float ratio);

	float GetNear() const;
	float GetFar() const;
	float GetFOV() const;
	float GetRatio() const;

	float* GetViewMatrix() const;
	float* GetProjectionMatrix() const;

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);

public:
	Frustum frustum;

private:
	bool showPopup = false;

	// Editor Variables -------------
	bool culling = false;
	bool is_main = false;
	float width = 0.0f;
	float height = 0.0f;
	float aspect_ratio = 0.0f;
	float near_plane = 0.0f;
	float far_plane = 0.0f;
	float vertical_fov = 0.0f;
	// -------------------------------

	std::queue<GameObject*> candidates_to_cull;
};

#endif