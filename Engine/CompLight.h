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
	~CompLight();

	bool Enable();
	bool Disable();
	void Init();
	void PreUpdate(float dt);
	void Update(float dt);
	void Draw();
	void Clear();

public:
	//Billboard to show where are the lights----
	uint texture_bulb = 0;
	//---------------------------------------------
	float3 position;
	float4 color; //a.k.a. the color of the light
	float attenuation;
	float ambientCoefficient;
	Light_type type;
};

#endif