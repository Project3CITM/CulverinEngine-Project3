#ifndef COMPONENT_RECT_TRANSFORM_H
#define COMPONENT_RECT_TRANSFORM_H
#include "CompTransform.h"
#include "AnimableComponent.h"
#include "Math\float2.h"

class CompRectTransform :public CompTransform, public AnimableComponent
{
public:
	CompRectTransform(Comp_Type t, GameObject* parent);
	CompRectTransform(const CompRectTransform& copy, GameObject* parent);
	~CompRectTransform();
	void Update(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void ShowTransform(float drag_speed);
	void SyncComponent(GameObject* sync_parent);
	void CopyValues(const CompRectTransform * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void DrawRectTransform();

	void Resize(float2 resize_factor, bool is_canvas = false);

	AnimationData ShowParameters();
	float2 GenerateResizeFactor(int width, int height);

	const char* ReturnParameterName(ParameterValue parameter);
	void SetNewAnimationValue(const AnimationData& value);

	void SetWidth(int set_width);
	void SetHeight(int set_height);
	void SetMaxAnchor(float2 set_max_anchor);
	void SetMinAnchor(float2 set_min_anchor);
	void SetPivot(float2 set_pivot);
	void SetUpdateRect(bool set_update_rect);

	std::vector<float3> GenerateQuadVertices();

	AnimationValue GetParameter(ParameterValue parameter);

	int GetWidth()const;
	int GetHeight()const;
	float2 GetResizeFactor()const;
	float2 GetMaxAnchor()const;
	float2 GetMinAnchor()const;
	float2 GetPivot()const;
	float2 GetUIPosition()const;
	bool GetUpdateRect()const;
	float4 GetRect()const;
	float4 GetGlobalRect() const;
	float3 GetGlobalPosition() const;
	float3 GetNorthEastPosition()const;
	float3 GetNorthWestPosition()const;
	float3 GetSouthEastPosition()const;
	float3 GetSouthWestPosition()const;

private:

	float2 ui_position;
	int width = 0;
	int height = 0;
	float2 resize_factor;
	float2 max_anchor;
	float2 min_anchor;
	float2 pivot;
	float2 left_pivot;
	float2 right_pivot;
	bool update_rect = false;
	bool ignore_z = false;
};

#endif //COMPONENT_RECT_TRANSFORM_H
