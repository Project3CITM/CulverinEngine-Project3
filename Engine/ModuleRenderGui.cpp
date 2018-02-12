#include "Application.h"
#include "ModuleRenderGui.h"
#include "EventDef.h"
#include "CompCanvas.h"
#include "ModuleEventSystem.h"
#include "ModuleWindow.h"
#include "SDL/include/SDL_opengl.h"
#include "GL3W/include/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>


ModuleRenderGui::ModuleRenderGui(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;
	Update_enabled = true;
	postUpdate_enabled = true;
	name = "Render Gui";

}

ModuleRenderGui::~ModuleRenderGui()
{
}

bool ModuleRenderGui::Init(JSON_Object * node)
{
	window_width=App->window->GetWidth();
	window_height=App->window->GetHeight();
	return true;
}

bool ModuleRenderGui::Start()
{
	return true;
}

update_status ModuleRenderGui::PreUpdate(float dt)
{
	perf_timer.Start();


	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleRenderGui::Update(float dt)
{
	perf_timer.Start();


	Update_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleRenderGui::PostUpdate(float dt)
{
	perf_timer.Start();
	
	//if (!world_space_canvas.empty())
	//	WorldSpaceDraw();
	if (!screen_space_canvas.empty())
		ScreenSpaceDraw();
	
	postUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

bool ModuleRenderGui::SetEventListenrs()
{
	AddListener(EventType::EVENT_DRAW, this);
	return true;
}

void ModuleRenderGui::OnEvent(Event & this_event)
{
	//this_event.draw.ToDraw->Draw();
}

void ModuleRenderGui::WorldSpaceDraw()
{
}

void ModuleRenderGui::ScreenSpaceDraw()
{

	int total_width = (int)(window_width * window_scale);
	int total_height = (int)(window_height * window_scale);
	if (total_width == 0 || total_height == 0)
		return;
/*
	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers, polygon fill.
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_CLAMP);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glViewport(0, 0, (GLsizei)total_width, (GLsizei)total_height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f, window_width, window_height, 0.0f, -1.0f, +1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	*/
	//Draw

	for (int i = 0; i < screen_space_canvas.size(); i++)
	{
		screen_space_canvas[i]->DrawCanvasRender();
	}

	/*
	//End Draw
	// Restore modified state
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glPolygonMode(GL_FRONT, last_polygon_mode[0]); glPolygonMode(GL_BACK, last_polygon_mode[1]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);


	*/
	screen_space_canvas.clear();
}


bool ModuleRenderGui::CleanUp()
{
	return true;
}

void ModuleRenderGui::OnResize(int width, int height)
{
}
