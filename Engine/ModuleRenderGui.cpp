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
#include"ModuleShaders.h"
#include "Scene.h"
#include "ModuleFramebuffers.h"
#include"ShadersLib.h"
#include "CompInteractive.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
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

	default_ui_shader = new ShaderProgram();
	default_ui_shader->name = "Default Ui Shader";
	//Success flag
	GLint programSuccess = GL_TRUE;

	//Generate program
	default_ui_shader->programID = glCreateProgram();

	//Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Get vertex source
	const GLchar* vertexShaderSource[] =
	{
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"uniform mat4 model;\n"
		"uniform vec4 Color_UI_ME;\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec2 texCoord;\n"
		"out vec2 Frag_UV;\n"

		"void main()\n"
		"{\n"
		"	Frag_UV = texCoord;\n"
		"	gl_Position = ProjMtx * model * vec4(position.xy,0,1);\n"
		"}\n"
	};

	//Set vertex source
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

	//Compile vertex source
	glCompileShader(vertexShader);

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE)
	{
		//ShaderLog(vertexShader);
		//return nullptr;
	}

	//Attach vertex shader to program
	glAttachShader(default_ui_shader->programID, vertexShader);

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Get fragment source
	const GLchar* fragmentShaderSource[] =
	{
		"#version 330\n"
		"uniform sampler2D Texture;\n"
		"uniform vec4 Color_UI_ME;\n"

		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Color_UI_ME*texture(Texture,vec2(Frag_UV.s, 1.0 - Frag_UV.t).st);\n"
		"}\n"
	};

	//Set fragment source
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

	//Compile fragment source
	glCompileShader(fragmentShader);

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{

		//ShaderLog(fragmentShader);
		//return nullptr;
	}

	//Attach fragment shader to program
	glAttachShader(default_ui_shader->programID, fragmentShader);

	//Link program
	glLinkProgram(default_ui_shader->programID);

	//Check for errors
	glGetProgramiv(default_ui_shader->programID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		//ProgramLog(default_shader.mProgramID);
		//return nullptr;
	}

	Shader* newFragment = new Shader();
	newFragment->shaderID = fragmentShader;
	newFragment->shaderText = *fragmentShaderSource;
	newFragment->shaderType = ShaderType::fragment;
	newFragment->name = "default_ui_shader_frag";
	newFragment->shaderPath = "";

	default_ui_shader->AddFragment(newFragment);

	Shader* newVertex = new Shader();
	newVertex->shaderID = vertexShader;
	newVertex->shaderText = *vertexShaderSource;
	newVertex->shaderType = ShaderType::vertex;
	newFragment->name = "default_ui_shader_vert";
	newVertex->shaderPath = "";

	default_ui_shader->AddVertex(newVertex);

	uint var_size = default_ui_shader->GetVariablesSize();
	for (int i = 0; i < var_size; i++) {
		UniformVar temp = default_ui_shader->GetVariableInfo(i);

		//Textures
		if (temp.type == 35678) {
			TextureVar texture_var;
			texture_var.var_name = temp.name;
			default_ui_shader->textures.push_back(texture_var);
		}

	}

	return true;
}

update_status ModuleRenderGui::PreUpdate(float dt)
{
	perf_timer.Start();
	if (last_size_dock.x != GetSizeDock("Scene").x || last_size_dock.y != GetSizeDock("Scene").y)
	{
		App->scene->scene_buff->WantRefreshRatio();
	}

	last_size_dock = GetSizeDock("Scene");
	iteractive_vector.clear();
	
	
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
	//if (!screen_space_canvas.empty())
	//	ScreenSpaceDraw();
	
	postUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

bool ModuleRenderGui::SetEventListenrs()
{
	AddListener(EventType::EVENT_DRAW, this);
	AddListener(EventType::EVENT_BUTTON_DOWN, this);
	AddListener(EventType::EVENT_MOUSE_MOTION, this);
	AddListener(EventType::EVENT_BUTTON_UP, this);
	AddListener(EventType::EVENT_PASS_COMPONENT, this);

	return true;
}

void ModuleRenderGui::OnEvent(Event & this_event)
{
	switch (this_event.type)
	{
	case EventType::EVENT_PASS_COMPONENT:
		iteractive_vector.push_back((CompInteractive*)this_event.pass_component.component);
		break;
	case EventType::EVENT_BUTTON_DOWN:
	case EventType::EVENT_BUTTON_UP:
	case EventType::EVENT_MOUSE_MOTION:
		if (focus != nullptr)
		{
			if (focus->PointerInside(this_event.pointer.position))
			{
				if (this_event.type == EventType::EVENT_BUTTON_DOWN)
				{
					focus->OnPointDown(this_event);
					if (focus->GetNavigationMode() != NavigationMode::NAVIGATION_NONE)
					{
						focus->OnInteractiveSelected(this_event);
					}

				}
				if (this_event.type == EventType::EVENT_BUTTON_UP)
				{
					focus->OnPointUP(this_event);

				}
				if (this_event.type == EventType::EVENT_MOUSE_MOTION)
				{
					focus->OnPointEnter(this_event);
				}
			}
			else
			{
				if (this_event.type == EventType::EVENT_MOUSE_MOTION)
				{
					focus->OnPointExit(this_event);

				}
			}
		}
		else
		{
			bool positive_colision = false;
			std::vector<CompInteractive*>::reverse_iterator it = iteractive_vector.rbegin();
			for (; it != iteractive_vector.rend(); it++)
			{
				if ((*it)->PointerInside(this_event.pointer.position))
				{
					if (positive_colision)
					{
						(*it)->ForceClear(this_event);
					}
					if (this_event.type == EventType::EVENT_BUTTON_DOWN)
					{
						(*it)->OnPointDown(this_event);
						if ((*it)->GetNavigationMode() != NavigationMode::NAVIGATION_NONE)
						{
							(*it)->OnInteractiveSelected(this_event);
						}

					}
					if (this_event.type == EventType::EVENT_BUTTON_UP)
					{
						(*it)->OnPointUP(this_event);

					}
					if (this_event.type == EventType::EVENT_MOUSE_MOTION)
					{
						(*it)->OnPointEnter(this_event);
					}

					positive_colision = true;

				}
				else
				{
					if (this_event.type == EventType::EVENT_MOUSE_MOTION)
					{
						(*it)->OnPointExit(this_event);
					}
				}
			}
		}
		break;
	default:
		break;
	}
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

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers, polygon fill.
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_texture_2D = glIsEnabled(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ImGuiIO& io = ImGui::GetIO();

	glViewport(0, 0, (GLsizei)total_width, (GLsizei)total_height);


	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x,	0.0f,						 0.0f, 0.0f },
		{ 0.0f,						2.0f / io.DisplaySize.y,	 0.0f, 0.0f },
		{ 0.0f,						0.0f,						-1.0f, 0.0f },
		{ -1.0f,						-1.0f,						 0.0f, 1.0f },
	};
	//Draw
	default_ui_shader->Bind();
	GLint g_AttribLocationProjMtx = glGetUniformLocation(default_ui_shader->programID, "ProjMtx");
	glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);


	//Frustum camFrust = App->renderer3D->active_camera->frustum;// App->camera->GetFrustum();
	//GLint viewLoc = glGetUniformLocation(default_ui_shader->programID, "ProjMtx");
	//glUniformMatrix4fv(viewLoc, 1, GL_TRUE, camFrust.ViewProjMatrix().ptr());

	for (int i = 0; i < screen_space_canvas.size(); i++)
	{
		screen_space_canvas[i]->DrawCanvasRender();
	}

	
	//End Draw
	// Restore modified state
	default_ui_shader->Unbind();
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_texture_2D) glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);

	glPolygonMode(GL_FRONT, last_polygon_mode[0]); glPolygonMode(GL_BACK, last_polygon_mode[1]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);

	
	
	screen_space_canvas.clear();
}


bool ModuleRenderGui::CleanUp()
{
	return true;
}

void ModuleRenderGui::OnResize(int width, int height)
{
}
