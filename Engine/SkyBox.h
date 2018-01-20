#ifndef _SKYBOX_
#define _SKYBOX_

#include "Math\float3.h"
#include "Globals.h"

enum SkyBoxSide
{
	SKY_LEFT = 0,
	SKY_BACK,
	SKY_RIGHT,
	SKY_FRONT,
	SKY_TOP,
	SKY_BOTTOM
};

class SkyBox
{
public:
	SkyBox();
	virtual ~SkyBox();

	void InitSkybox();
	void DrawSkybox(float size, float3 pos, uint i);
	void DeleteSkyboxTex();

	uint GetTextureID(uint i) const;

private:
	uint texture[2][6];
};


#endif