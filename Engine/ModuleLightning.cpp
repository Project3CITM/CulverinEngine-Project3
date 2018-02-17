#include "ModuleLightning.h"
#include "GL3W/include/glew.h"
#include"MathGeoLib.h"
#include"Application.h"
#include"ShadersLib.h"
#include"Application.h"
#include"Scene.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ModuleRenderer3D.h"
#include "ModuleResourceManager.h"
using namespace glm;


DepthFrameBuffer::DepthFrameBuffer()
{
}

DepthFrameBuffer::DepthFrameBuffer(int width, int height)
{
	Create(width, height);
}

DepthFrameBuffer::~DepthFrameBuffer()
{
	Destroy();
}

void DepthFrameBuffer::Create(int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &frame_id);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
				0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, frame_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	glDrawBuffer(GL_NONE); // Since we dont need the color buffer we must tell OpenGl explicitly
	glReadBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthFrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &frame_id);
	glDeleteTextures(1, &texture);
}

void DepthFrameBuffer::Resize(int width, int height)
{
	if(this->width != width || this->height != height)
	{
		Destroy();
		Create(width, height);
	}
}

uint DepthFrameBuffer::GetTexture() const
{
	return texture;
}

//---------------------------------------------------------

ModuleLightning::ModuleLightning(bool start_enabled) : Module(start_enabled)
{
	name = "Lightning";
	have_config = true;
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;
}


ModuleLightning::~ModuleLightning()
{
}

bool ModuleLightning::Init(JSON_Object* node)
{
	perf_timer.Start();

	// TODO: Read ammount of shadow cast point from config. Will use default for now for testing purposes
	shadow_cast_points_count = 1;

	Awake_t = perf_timer.ReadMs();
	return true;
}

bool ModuleLightning::Start()
{
	perf_timer.Start();

	AddShadowMapCastViews(shadow_cast_points_count);

	Start_t = perf_timer.ReadMs();

	text.Create(shadow_maps_res_w, shadow_maps_res_h);

	//------------------------------------


	shadow_Shader = new ShaderProgram();
	shadow_Shader->name = "Default Shader";
	//Success flag
	GLint programSuccess = GL_TRUE;

	//Generate program
	shadow_Shader->programID = glCreateProgram();

	//Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Get vertex source
	const GLchar* vertexShaderSource[] =
	{
		"#version 330 core\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec2 texCoord;\n"
		"layout(location = 2) in vec3 normal;\n"
		"layout(location = 3) in vec4 color;\n"
		
		"uniform mat4 depthMVP;\n"
		"uniform mat4 model;\n"

		"void main()\n"
		"{\n"
		" gl_Position =  depthMVP* model * vec4(position,1);\n"
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
	glAttachShader(shadow_Shader->programID, vertexShader);

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Get fragment source
	const GLchar* fragmentShaderSource[] =
	{
		"#version 330 core\n"
		"layout(location = 0) out vec4 fragmentdepth;\n"

		"void main()\n"
		"{\n"
		"fragmentdepth = vec4(vec3(gl_FragCoord.z),1);\n"
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
	glAttachShader(shadow_Shader->programID, fragmentShader);

	//Link program
	glLinkProgram(shadow_Shader->programID);

	//Check for errors
	glGetProgramiv(shadow_Shader->programID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		//ProgramLog(default_shader.mProgramID);
		//return nullptr;
	}

	Shader* newFragment = new Shader();
	newFragment->shaderID = fragmentShader;
	newFragment->shaderText = *fragmentShaderSource;
	newFragment->shaderType = ShaderType::fragment;
	newFragment->name = "default_shader_frag";
	newFragment->shaderPath = "";

	shadow_Shader->AddFragment(newFragment);

	Shader* newVertex = new Shader();
	newVertex->shaderID = vertexShader;
	newVertex->shaderText = *vertexShaderSource;
	newVertex->shaderType = ShaderType::vertex;
	newVertex->name = "default_shader_vert";
	newVertex->shaderPath = "";

	shadow_Shader->AddVertex(newVertex);


	//-------------------------------------
	App->renderer3D->LoadImage_devil("Assets/Bulb_Texture.png", &texture_bulb);

	light_UI_plane = (ResourceMesh*)App->resource_manager->GetResource(4);

	light_UI_plane->vertices[0].texCoords = float2(0, 0);
	light_UI_plane->vertices[1].texCoords = float2(1, 0);
	light_UI_plane->vertices[2].texCoords = float2(0, 1);
	light_UI_plane->vertices[3].texCoords = float2(1, 1);


	char* total_buffer_mesh = nullptr;
	int total_size_buffer = 0;

	if (light_UI_plane->vertices.size()>0)
	{
		total_size_buffer += light_UI_plane->num_vertices * 3;
		total_size_buffer += light_UI_plane->num_vertices * 2;
	}

	total_buffer_mesh = new char[total_size_buffer * sizeof(float)];
	char* cursor = total_buffer_mesh;

	for (int i = 0; i <light_UI_plane->num_vertices; i++) {
		memcpy(cursor, &light_UI_plane->vertices[i].pos, 3 * sizeof(float));
		cursor += 3 * sizeof(float);

		memcpy(cursor, &light_UI_plane->vertices[i].texCoords, 2 * sizeof(float));
		cursor += 2 * sizeof(float);
	}
	light_UI_plane->num_game_objects_use_me++;
	light_UI_plane->LoadToMemory();


	return true;
}

update_status ModuleLightning::PreUpdate(float dt)
{
	// TODO: Calc here the closest lights used, need the main/active camera pos
	perf_timer.Start();

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleLightning::Update(float dt)
{


	shadow_Shader->Bind();
	text.Bind("peter");

	glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);

	// Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

	


	uint depthMatrixID= glGetUniformLocation(shadow_Shader->programID, "depthMVP");
	glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

	App->scene->scene_buff->Bind("Scene");
	//ImGui::Image((ImTextureID*)text.frame_id, ImVec2(shadow_maps_res_w, shadow_maps_res_h));
	shadow_Shader->Unbind();
	//text.UnBind("peter");




	return UPDATE_CONTINUE;
}

bool ModuleLightning::SaveConfig(JSON_Object* node)
{
	return true;
}

bool ModuleLightning::CleanUp()
{
	for (std::vector<DepthFrameBuffer>::iterator it = shadow_maps.begin(); it != shadow_maps.end(); ++it)
	{
		(*it).Destroy();
	}

	return true;
}

void ModuleLightning::CalShadowMaps()
{
	//TODO: Actually calc the shadow maps
	//TODO: Need to get the main/render camera somehow

	// Start rendering z buffer into the frame buffer

	// Should render from lights perspectives. For now will use hardcoded positions in order to test.

	//float3 lisght_pos(4.f, 10.f, 0.f);
	//float3 look_at(0.f, 0.f, 0.f);

	//for(uint i = 0; i < shadow_maps.size(); ++i)
	//{
	//	DepthFrameBuffer* frame = &shadow_maps[i];
	//
	//
	//}
}

void ModuleLightning::SetShadowCastPoints(uint points)
{
	shadow_cast_points_count = points;
	if(shadow_cast_points_count > shadow_maps.size())
	{
		// Then create new frame buffers
		// TODO: Probably would be better to add a max ammount of shadow maps

		AddShadowMapCastViews(shadow_cast_points_count - shadow_maps.size());
	}
}

void ModuleLightning::ResizeShadowMaps(uint w_res, uint h_res)
{
	shadow_maps_res_w = w_res;
	shadow_maps_res_h = h_res;

	for(std::vector<DepthFrameBuffer>::iterator it = shadow_maps.begin(); it != shadow_maps.end(); ++it)
	{
		(*it).Resize(shadow_maps_res_w, shadow_maps_res_h);
	}
}

std::vector<DepthFrameBuffer>* ModuleLightning::GetShadowMaps()
{
	return &shadow_maps;
}

void ModuleLightning::GetShadowMapsResolution(uint& w_res, uint& h_res)
{
	w_res = shadow_maps_res_w;
	h_res = shadow_maps_res_h;
}

void ModuleLightning::OnLightCreated(CompLight* l)
{
	//TODO: Link this with component creation

	scene_lights.push_back(l);
}

void ModuleLightning::OnLightDestroyed(CompLight* l)
{
	//TODO: Link this with component destruction

	for(std::vector<CompLight*>::iterator it = scene_lights.begin(); it != scene_lights.end(); ++it)
	{
		if((*it) == l)
		{
			scene_lights.erase(it);

			for(std::vector<CompLight*>::iterator it2 = frame_used_lights.begin(); it2 != frame_used_lights.end(); ++it2)
			{
				frame_used_lights.erase(it2);
				return;
			}

			return;
		}
	}
}

std::vector<CompLight*> ModuleLightning::GetSceneLights() const
{
	return scene_lights;
}

void ModuleLightning::PushLight(CompLight * light)
{
	scene_lights.push_back(light);
}

void ModuleLightning::DeleteLight(CompLight * light)
{
	for (auto item = scene_lights.begin(); item != scene_lights.end(); item++) {
		if ((*item) == light)
			scene_lights.erase(item);
	}

}

void ModuleLightning::AddShadowMapCastViews(uint ammount)
{
	for(int i = 0; i < ammount; ++i)
	{
		DepthFrameBuffer buff;
		buff.Create(shadow_maps_res_w, shadow_maps_res_h);
		shadow_maps.push_back(buff);
	}
}

// ------------- UI -----------------------------------------

update_status ModuleLightning::UpdateConfig(float dt)
{
	//TODO: Add possibility to change this. For now just info display
	ImGui::Text("Shadow cast points used:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", shadow_cast_points_count);

	ImGui::Text("Shadow maps resolution:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i x %i", shadow_maps_res_w, shadow_maps_res_h);

	ImGui::Text("Lights in the scene:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", scene_lights.size());

	ImGui::Text("Lights used on frame:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", frame_used_lights.size());

	if(ImGui::TreeNodeEx("Shadow maps"))
	{
		for(uint i = 0; i < shadow_maps.size(); ++i)
		{
			if(i == shadow_cast_points_count)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "This shadow maps are not used currently!");
				ImGui::Separator();
			}

			std::string shadowmap_name = "Shadow map: " + std::to_string(i);
			if(ImGui::TreeNodeEx(shadowmap_name.c_str()))
			{
				DepthFrameBuffer* frame = &shadow_maps[i];
				ImGui::Image((ImTextureID*)frame->GetTexture(), ImVec2(256, 256));

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	return UPDATE_CONTINUE;
}