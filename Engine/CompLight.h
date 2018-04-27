#ifndef COMPONENT_LIGHT
#define COMPONENT_LIGHT
#include "Component.h"
#include "MathGeoLib.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
class ResourceMesh;

enum Light_type {
	NO_LIGHT_TYPE=-1,
	POINT_LIGHT,
	DIRECTIONAL_LIGHT	
};

class CompLight :	public Component
{
public:
	CompLight(Comp_Type t, GameObject * parent);
	CompLight(const CompLight & copy, GameObject * parent);

	~CompLight();

	bool Enable();
	bool Disable();
	void Init();
	void PreUpdate(float dt);
	void Update(float dt);
	void Draw();
	void Clear();

	// EDITOR METHODS -----------------
	virtual void ShowOptions();
	virtual void ShowInspectorInfo();
	// --------------------------------

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	void SyncComponent(GameObject* sync_parent);

	void GetOwnBufferSize(uint& buffer_size);
	void SaveBinary(char** cursor, int position) const;
	void LoadBinary(char** cursor);
	// -------------------------------------

	void UpdateFrustum();
	void FrustumDebug();

public:

	//---------------------------------------------
	float4 color; //a.k.a. the color of the light

	float ambientCoefficient=0.2;
	float radius = 50;

	float4 properties;
	Light_type type;
	Frustum frustum;
	glm::mat4 depthMVPMat;
	glm::mat4 depthBiasMat;
	//UI variables------------
	float color_temp[4];
	std::string types_lights;
	int ui_light_type=0;
	//---------

	bool use_light_to_render = false;
	bool to_delete = false;


};

#endif