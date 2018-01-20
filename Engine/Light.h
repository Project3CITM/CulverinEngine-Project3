#ifndef _LIGHT_
#define _LIGHT_

#include "Color.h"
#include "Math/float3.h"

struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	float3 position;

	int ref;
	bool on;
};

#endif