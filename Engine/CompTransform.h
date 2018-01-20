#ifndef _COMPONENT_TRANSFORM_
#define _COMPONENT_TRANSFORM_

#include "Component.h"
#include "MathGeoLib.h"
#include "Assimp/include/matrix4x4.h"
#include "ImGui/ImGuizmo.h"
#include <vector>

class GameObject;

struct Axis
{
	float3 x = { 1, 0, 0 };
	float3 y = { 0, 1, 0 };
	float3 z = { 0, 0, 1 };
};

class CompTransform :public Component
{
public:
	CompTransform(Comp_Type t, GameObject* parent);
	CompTransform(const CompTransform& copy, GameObject* parent);
	~CompTransform();

	void Init(float3 p, float4 r, float3 s);
	void Init(float3 p, float3 r, float3 s);
	void Update(float dt);

	void ShowOptions();
	void ShowInspectorInfo();
	void ShowTransform(float drag_speed);

	void SetPosGlobal(float3 pos);
	void SetRotGlobal(float3 rot);
	void SetScaleGlobal(float3 scale);
	void SetPos(float3 pos);
	void IncrementRot(float3 rot);
	void SetRot(float3 rot);	//"rot" is "rotation_euler" updated, so we don't need to update it inside this method
	void SetRot(Quat rot);		//"rot" is the quaternion we want to set to our rotation quaternion
	void SetScale(float3 scale);
	void SetLocalTransform();
	void SetGlobalTransform();
	void UpdateLocalTransform();
	void UpdateGlobalTransform();
	void UpdateGlobalMatrixRecursive();

	void ResetMatrix();
	void UpdateMatrix(ImGuizmo::MODE mode);

	float3 GetPos() const;
	float3 GetPosGlobal() const;
	Quat GetRot() const;
	float3 GetRotEuler() const;
	float3 GetScale() const;
	float4x4 GetLocalTransform() const;
	float4x4 GetGlobalTransform() const;
	ImGuizmo::MODE GetMode() const;

	void Freeze(bool freeze);

	const float* GetMultMatrixForOpenGL() const;

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);

private:
	Axis axis;
	bool toUpdate = false;
	bool freeze = false;
	bool editing_transform = false;

	// Output Values ----------------------
	float3 position = { 0, 0, 0 };
	float3 rotation_euler = { 0, 0, 0 };
	
	float3 scale = { 0, 0, 0 };
	float3 scale_global = { 0, 0, 0 };

	float3 position_global = { 0, 0, 0 };
	float3 rotation_euler_global = { 0, 0, 0 };
	// ------------------------------------

	Quat rotation = math::Quat::identity;
	Quat rotation_global = math::Quat::identity;

	float4x4 global_transform = math::float4x4::identity;
	float4x4 local_transform = math::float4x4::identity;

	float4 screen = math::float4::zero;
	float4x4 global_transposed = float4x4::identity;
	ImGuizmo::MODE transform_mode = ImGuizmo::LOCAL;
};

#endif