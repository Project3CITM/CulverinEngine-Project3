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
	void SetRotGlobal(Quat rot);
	void SetScaleGlobal(float3 scale);
	void SetPos(float3 pos);
	void Translate(float3 translation);
	void IncrementRot(float3 rot);
	void RotateAroundAxis(float3 rot, float angle);
	void SetRot(float3 rot);	//"rot" is "rotation_euler" updated, so we don't need to update it inside this method
	void SetRot(Quat rot);		//"rot" is the quaternion we want to set to our rotation quaternion
	void SetScale(float3 scale);
	void LookAt(float3 target_pos);
	void SetLocalTransform();
	void SetGlobalTransform();
	void UpdateGlobalTransform();
	void UpdateGlobalMatrixRecursive();
	void SetToUpdate();

	void ResetMatrix();
	void UpdateMatrix(ImGuizmo::MODE mode);

	float3 GetForwardVector()const;
	float3 GetBackwardVector()const;
	float3 GetUpVector()const;
	float3 GetDownVector()const;
	float3 GetRightVector()const;
	float3 GetLeftVector()const;
	void SetForwardVector(float3 vec);
	void SetBackwardVector(float3 vec);
	void SetUpVector(float3 vec);
	void SetDownVector(float3 vec);
	void SetRightVector(float3 vec);
	void SetLeftVector(float3 vec);
	float3 GetPos() const;
	float3 GetPosGlobal() const;
	float3 GetPosParent() const;
	Quat GetRot() const;
	Quat GetRotGlobal() const;
	Quat GetRotParent() const;
	float3 GetRotEuler() const;
	float3 GetGlobalRotEuler()const;
	float3 GetScale() const;
	float3 GetGlobalScale() const;
	float3 GetParentScale() const;
	float4x4 GetLocalTransform() const;
	float4x4 GetGlobalTransform() const;
	float3 GetEulerToDirection() const;
	ImGuizmo::MODE GetMode() const;
	bool GetToUpdate() const;
	bool GetUpdated() const;

	float3* GetPosPointer();
	float3* GetGlobalPosPointer();

	void Freeze(bool freeze);

	const float* GetMultMatrixForOpenGL() const;

	void CopyValues(const CompTransform* component);

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);

protected:
	Axis axis;
	bool toUpdate = false;
	bool updated = false;
	bool freeze = false;
	bool editing_transform = false;

	// Output Values ----------------------
	float3 position = { 0, 0, 0 }; 			
	float3 rotation_euler = { 0, 0, 0 };
	
	float3 scale = { 0, 0, 0 };				
	float3 scale_global = { 0, 0, 0 };

	float3 position_global = { 0, 0, 0 };
	float3 rotation_euler_global = { 0, 0, 0 };


	float3* position_pointer = nullptr;
	float3* global_position_pointer = nullptr;
	float3* scale_pointer = nullptr;
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