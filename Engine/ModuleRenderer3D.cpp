#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "Scene.h"
#include "GameObject.h"
#include "WindowSceneWorld.h"
#include "parson.h"
#include "SkyBox.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"

#include "SDL/include/SDL_opengl.h"
#include "GL3W/include/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;
	postUpdate_enabled = true;
	
	haveConfig = true;

	name = "Renderer";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(JSON_Object* node)
{
	perf_timer.Start();

	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if(ret == true)
	{
		//Use Vsync
		if(App->GetVSYNC() && SDL_GL_SetSwapInterval(1) < 0)
			LOG("[error]Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error]Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error]Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		error = glewInit();
		if (error != GL_NO_ERROR)
		{
			LOG("[error]Error initializing GL3W! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error]Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.5f, 0.5f, 0.5f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 5.0f, 5.0f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		//Load render config info -------
		depth_test = json_object_get_boolean(node, "Depth Test");
		cull_face = json_object_get_boolean(node, "Cull Face");
		lighting = json_object_get_boolean(node, "Lighting");
		color_material = json_object_get_boolean(node, "Color Material");
		texture_2d = json_object_get_boolean(node, "Texture 2D");
		wireframe = json_object_get_boolean(node, "Wireframe");
		normals = json_object_get_boolean(node, "Normals");
		smooth = json_object_get_boolean(node, "Smooth");
		
		node = json_object_get_object(node, "Fog");
		fog_active = json_object_get_boolean(node, "Active");
		fog_density = json_object_get_number(node, "Density");	
	}

	Awake_t = perf_timer.ReadMs();
	return ret;
}

bool ModuleRenderer3D::Start()
{
	perf_timer.Start();

	lights[0].Active(true);
	(depth_test) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	(cull_face) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	(lighting) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
	(color_material) ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);
	(texture_2d) ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
	(smooth) ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);

	if (fog_active)
	{
		glEnable(GL_FOG);
		glFogfv(GL_FOG_DENSITY, &fog_density);
	}

	Start_t = perf_timer.ReadMs();
	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	perf_timer.Start();

	// 
	App->scene->sceneBuff->Bind("Scene");

	// Refresh Projection of the camera
	UpdateProjection(active_camera);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(active_camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(active_camera->frustum.pos.x, active_camera->frustum.pos.y, active_camera->frustum.pos.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

//update_status ModuleRenderer3D::Update(float dt)
//{
//	perf_timer.Start();
//	Update_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	perf_timer.Start();
	
	// Draw Skybox (direct mode for now)
	if (App->scene->draw_skybox)
	{
		App->scene->skybox->DrawSkybox(800, active_camera->frustum.pos, App->scene->skybox_index);
	}
	//Draw Test Cube
	//App->scene->DrawCube(5);

	// Draw Plane
	App->scene->DrawPlane();

	// Draw GameObjects
	for (uint i = 0; i < App->scene->gameobjects.size(); i++)
	{
		App->scene->gameobjects[i]->Draw();
	}

	// Draw Quadtree
	if (App->scene->quadtree_draw)
	{
		App->scene->quadtree.DebugDraw();
	}

	// Draw Mouse Picking Ray
	//glBegin(GL_LINES);
	//glLineWidth(3.0f);
	//glColor4f(1.00f, 0.761f, 0.00f, 1.00f);
	//glVertex3f(App->camera->ray.a.x, App->camera->ray.a.y, App->camera->ray.a.z); glVertex3f(App->camera->ray.b.x, App->camera->ray.b.y, App->camera->ray.b.z);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//glEnd();

	App->scene->sceneBuff->UnBind("Scene");

	//if (game_camera != nullptr)
	//{
	//	App->scene->sceneBuff->Bind("Game");
	//	UpdateProjection(game_camera);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glLoadIdentity();
	//	glMatrixMode(GL_MODELVIEW);
	//	glLoadMatrixf(game_camera->GetViewMatrix());

	//	// Render Lights
	//	for (uint i = 0; i < MAX_LIGHTS; ++i)
	//		lights[i].Render();

	//	// Draw Plane
	//	App->scene->DrawPlane();

	//	// Draw GameObjects
	//	for (uint i = 0; i < App->scene->gameobjects.size(); i++)
	//	{
	//		App->scene->gameobjects[i]->Draw();
	//	}

	//	App->scene->gameBuff->UnBind("Game");

	//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	//glLoadIdentity();
	//}

	ImGui::Render();

	SDL_GL_SwapWindow(App->window->window);

	postUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::UpdateConfig(float dt)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));
	if (ImGui::Checkbox("Depth Test", &depth_test))
	{
		(depth_test) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	}
	if (ImGui::Checkbox("Cull Face", &cull_face))
	{
		(cull_face) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}
	if (ImGui::Checkbox("Lighting", &lighting))
	{
		(lighting) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
	}
	if (ImGui::Checkbox("Color Material", &color_material))
	{
		(color_material) ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);
	}
	if (ImGui::Checkbox("Texture 2D", &texture_2d))
	{
		(texture_2d) ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
	}

	ImGui::Checkbox("Wireframe", &wireframe);

	ImGui::Checkbox("Normals", &normals);

	if (ImGui::Checkbox("Smooth", &smooth))
	{
		(smooth) ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);
	}
	if (ImGui::CollapsingHeader("Fog"))
	{
		if (ImGui::Checkbox("Active", &fog_active))
		{
			if (fog_active)
			{
				glEnable(GL_FOG);
				glFogfv(GL_FOG_DENSITY, &fog_density);
			}
			else
			{
				glDisable(GL_FOG);
			}
		}

		if (fog_active)
		{
			if (ImGui::SliderFloat("Density", &fog_density, 0.0f, 1.0f, "%.3f"))
			{
				glFogfv(GL_FOG_DENSITY, &fog_density);
			}
		}
	}

	ImGui::PopStyleVar();
	return UPDATE_CONTINUE;
}

bool ModuleRenderer3D::SaveConfig(JSON_Object * node)
{
	//Save render config info -------
	json_object_set_boolean(node, "Depth Test", depth_test);
	json_object_set_boolean(node, "Cull Face", cull_face);
	json_object_set_boolean(node, "Lighting", lighting);
	json_object_set_boolean(node, "Color Material", color_material);
	json_object_set_boolean(node, "Texture 2D", texture_2d);
	json_object_set_boolean(node, "Wireframe", wireframe);
	json_object_set_boolean(node, "Normals", normals);
	json_object_set_boolean(node, "Smooth", smooth);

	node = json_object_get_object(node, "Fog");
	json_object_set_boolean(node, "Active", fog_active);
	json_object_set_number(node, "Density", fog_density);
	return true;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::SetActiveCamera(CompCamera* cam)
{
	active_camera = cam;
}

void ModuleRenderer3D::SetSceneCamera(CompCamera* cam)
{
	scene_camera = cam;
}

void ModuleRenderer3D::SetGameCamera(CompCamera* cam)
{
	game_camera = cam;
}

void ModuleRenderer3D::UpdateProjection(CompCamera* cam)
{
	if (cam != nullptr)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glLoadMatrixf(cam->GetProjectionMatrix());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	float ratio = (float)width / (float)height;
	active_camera->SetRatio(ratio);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glLoadMatrixf(active_camera->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
