#include "CompLight.h"


CompLight::CompLight(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	position = float3::zero;
	color = float4(255, 255, 255, 255);
	type = NO_LIGHT_TYPE;
	attenuation = 1;
	ambientCoefficient = 1;
}

CompLight::~CompLight()
{
}

bool CompLight::Enable()
{
	return false;
}

bool CompLight::Disable()
{
	return false;
}

void CompLight::Init()
{
}

void CompLight::PreUpdate(float dt)
{
}

void CompLight::Update(float dt)
{
}

void CompLight::Draw()
{
}

void CompLight::Clear()
{
}

