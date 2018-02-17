#ifndef COMPONENT_LIGHT
#define COMPONENT_LIGHT
#include "Component.h"
#include"MathGeoLib.h"

enum Light_type {
	NO_LIGHT_TYPE=-1,
	POINT_LIGHT,
	DIRECTIONAL_LIGHT	
};

class CompLight :
	public Component
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
	// -------------------------------------

public:
	//Billboard to show where are the lights----
	uint texture_bulb = 0;
	ResourceMesh* plane;
	//---------------------------------------------
	float4 color; //a.k.a. the color of the light
	float attenuation;
	float ambientCoefficient;
	Light_type type;

	//UI variables------------
	float color_temp[4];
	std::string types_lights;
	int ui_light_type;
	//---------

};

#endif