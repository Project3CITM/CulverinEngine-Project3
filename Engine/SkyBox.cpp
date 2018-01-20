#include "SkyBox.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRenderer3D.h"

#define NUM_SKYBOX 2

SkyBox::SkyBox()
{
}


SkyBox::~SkyBox()
{
}


void SkyBox::InitSkybox()
{
	// SKYBOX 1 -----------------------------------------
	texture[0][SkyBoxSide::SKY_LEFT] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox1/left.png");
	texture[0][SkyBoxSide::SKY_BACK] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox1/back.png");
	texture[0][SkyBoxSide::SKY_RIGHT] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox1/right.png");
	texture[0][SkyBoxSide::SKY_FRONT] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox1/front.png");
	texture[0][SkyBoxSide::SKY_TOP] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox1/top.png");
	texture[0][SkyBoxSide::SKY_BOTTOM] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox1/bottom.png");

	// SKYBOX 2 -------------------------------------------
	texture[1][SkyBoxSide::SKY_LEFT] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox2/left.png");
	texture[1][SkyBoxSide::SKY_BACK] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox2/back.png");
	texture[1][SkyBoxSide::SKY_RIGHT] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox2/right.png");
	texture[1][SkyBoxSide::SKY_FRONT] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox2/front.png");
	texture[1][SkyBoxSide::SKY_TOP] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox2/top.png");
	texture[1][SkyBoxSide::SKY_BOTTOM] = App->textures->LoadSkyboxTexture("Images/Skybox/Skybox2/bottom.png");

	// For now, we load and draw these 2 unique skyboxes using direct mode (changed for the new release).
}

void SkyBox::DrawSkybox(float size, float3 pos, uint i)
{
	if (i >= NUM_SKYBOX)
	{
		LOG("Invalid skybox index.");
		return;
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//back face 
	glBindTexture(GL_TEXTURE_2D, texture[i][SkyBoxSide::SKY_BACK]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-size / 2 + pos.x, size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(1, 1); glVertex3f(size / 2 + pos.x, size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(1, 0); glVertex3f(size / 2 + pos.x, -size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(0, 0); glVertex3f(-size / 2 + pos.x, -size / 2 + pos.y, size / 2 + pos.z);
	glEnd();

	//left face 
	glBindTexture(GL_TEXTURE_2D, texture[i][SkyBoxSide::SKY_LEFT]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(-size / 2 + pos.x, -size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(0, 0); glVertex3f(-size / 2 + pos.x, -size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(0, 1); glVertex3f(-size / 2 + pos.x, size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(1, 1); glVertex3f(-size / 2 + pos.x, size / 2 + pos.y, size / 2 + pos.z);
	glEnd();

	//front face
	glBindTexture(GL_TEXTURE_2D, texture[i][SkyBoxSide::SKY_FRONT]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(size / 2 + pos.x, size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(1, 1); glVertex3f(-size / 2 + pos.x, size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(1, 0); glVertex3f(-size / 2 + pos.x, -size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(0, 0); glVertex3f(size / 2 + pos.x, -size / 2 + pos.y, -size / 2 + pos.z);
	glEnd();

	//right face 
	glBindTexture(GL_TEXTURE_2D, texture[i][SkyBoxSide::SKY_RIGHT]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(size / 2 + pos.x, -size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(0, 0); glVertex3f(size / 2 + pos.x, -size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(0, 1); glVertex3f(size / 2 + pos.x, size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(1, 1); glVertex3f(size / 2 + pos.x, size / 2 + pos.y, -size / 2 + pos.z);
	glEnd();

	//top face 
	glBindTexture(GL_TEXTURE_2D, texture[i][SkyBoxSide::SKY_TOP]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(size / 2 + pos.x, size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(0, 0); glVertex3f(-size / 2 + pos.x, size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(0, 1); glVertex3f(-size / 2 + pos.x, size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(1, 1); glVertex3f(size / 2 + pos.x, size / 2 + pos.y, -size / 2 + pos.z);
	glEnd();

	//bottom face 
	glBindTexture(GL_TEXTURE_2D, texture[i][SkyBoxSide::SKY_BOTTOM]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(size / 2 + pos.x, -size / 2 + pos.y, size / 2 + pos.z);
	glTexCoord2f(1, 0); glVertex3f(size / 2 + pos.x, -size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(0, 0); glVertex3f(-size / 2 + pos.x, -size / 2 + pos.y, -size / 2 + pos.z);
	glTexCoord2f(0, 1); glVertex3f(-size / 2 + pos.x, -size / 2 + pos.y, size / 2 + pos.z);

	glEnd();

	if (App->renderer3D->lighting)
	{
		glEnable(GL_LIGHTING);
	}
	if (App->renderer3D->depth_test)
	{
		glEnable(GL_DEPTH_TEST);
	}
	if (!App->renderer3D->texture_2d)
	{
		glDisable(GL_TEXTURE_2D);
	}
}


void SkyBox::DeleteSkyboxTex()
{
	// Deleting textures from the 2 default skyboxes
	glDeleteTextures(6, &texture[0][0]);
	glDeleteTextures(6, &texture[1][0]);
}

uint SkyBox::GetTextureID(uint i) const
{
	if (i <= NUM_SKYBOX)
	{
		return texture[i][1]; // Show an iconic image to recognize the skybox
	}
	LOG("Invalid skybox index.");
	return 0;
}
