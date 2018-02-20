#ifndef _MODULERENDERER3D_
#define _MODULERENDERER3D_

#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "parson.h"

#include "ModuleShaders.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#define MAX_LIGHTS 8

class CompCamera;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	//update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);

	bool CleanUp();

	void SetActiveCamera(CompCamera* cam);
	void SetSceneCamera(CompCamera* cam);
	void SetGameCamera(CompCamera* cam);
	CompCamera* GetActiveCamera();

	void UpdateProjection(CompCamera* cam);

	void OnResize(int width, int height);

	float2 LoadImage_devil(const char * theFileName, GLuint *buff);
	bool loadTextureFromPixels32(GLuint * id_pixels, GLuint width_img, GLuint height_img, GLuint *buff);

public:

	
	SDL_GLContext context;
	CompCamera* active_camera = nullptr; /* Render the scene through the active camera (it can be SCENE camera or GAME camera)*/
	CompCamera* scene_camera = nullptr;
	CompCamera* game_camera = nullptr;

	// Configuration Options -----
	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;
	bool texture_2d = false;
	bool wireframe = false;
	bool smooth = true;
	bool fog_active = false;
	bool normals = false;
	bool bounding_box = false;
	GLfloat fog_density = 0;
	// --------------------------
	//Texture Creation Default
	uint id_checkImage = 0;
	unsigned char checkImage[64][32][4];

	// Shaders
	ShaderProgram* default_shader = nullptr;
	ResourceMaterial* default_mat = nullptr;

};

#endif