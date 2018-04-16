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
#include "ModuleRenderGui.h"
#include "SDL/include/SDL_opengl.h"
#include "GL3W/include/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include"Devil\include\ilut.h"
#include "DefaultShaders.h"
#include "Materials.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "gif-h\gif.h"

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma comment (lib, "Devil/libx86/ILU.lib")
#pragma comment (lib, "Devil/libx86/ILUT.lib")

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;
	postUpdate_enabled = true;

	have_config = true;

	name = "Renderer";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{

}

// Called before render is available
bool ModuleRenderer3D::Init(JSON_Object* node)
{
	perf_timer.Start();

	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (App->GetVSYNC() && SDL_GL_SetSwapInterval(1) < 0)
			LOG("[error]Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("[error]Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("[error]Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);



		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("[error]Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);



		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		error = glewInit();
		if (error != GL_NO_ERROR)
		{
			LOG("[error]Error initializing GL3W! %s\n", gluErrorString(error));
			ret = false;
		}



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

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 32; j++) {
			//int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)255;
			checkImage[i][j][1] = (GLubyte)255;
			checkImage[i][j][2] = (GLubyte)255;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &id_checkImage);
	glBindTexture(GL_TEXTURE_2D, id_checkImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	Texture default_text;
	default_text.name = "default_texture";
	default_text.id = id_checkImage;

	default_texture = new ResourceMaterial(App->random->Int());
	default_texture->Init(default_text);


	default_shader = App->module_shaders->CreateDefaultShader("Default Shader", fragmentShaderSource, vertexShaderSource, nullptr, true);
	lights_billboard_shader = App->module_shaders->CreateDefaultShader("Billboard Lights Shader", DefaultFrag, DefaultVert, nullptr,true);

	particles_shader = App->module_shaders->CreateDefaultShader("Particles Shader", DefaultFrag, DefaultVert, nullptr, true);
	non_glow_shader = App->module_shaders->CreateDefaultShader("Non Glow Shader", NonGlowFrag, DefaultVert, nullptr, true);
	blur_shader_tex = App->module_shaders->CreateDefaultShader("Texture Shader", BlurFrag, TextureVert, nullptr, true);
	final_shader_tex = App->module_shaders->CreateDefaultShader("Texture Shader", FinalFrag, TextureVert, nullptr, true);

	non_glow_material = new Material();
	non_glow_material->name = "Non Glow Material";
	non_glow_material->material_shader = non_glow_shader;
	non_glow_material->GetProgramVariables();
	App->module_shaders->materials.push_back(non_glow_material);
	non_glow_material->active_num = 1;

	final_tex_material = new Material();
	final_tex_material->name = "Final Tex Material";
	final_tex_material->material_shader = final_shader_tex;
	final_tex_material->GetProgramVariables();
	App->module_shaders->materials.push_back(final_tex_material);
	final_tex_material->active_num = 1;
	dmg_texture_id = App->textures->LoadTexture("Assets/Damage2.png");

	default_material = new Material();
	default_material->name = "Default Material";
	default_material->material_shader = default_shader;
	default_material->GetProgramVariables();

	if (default_material->textures.size()>0)
		default_material->textures[0].value = default_texture;


	Awake_t = perf_timer.ReadMs();
	return ret;
}

bool ModuleRenderer3D::Start()
{
	perf_timer.Start();

	blur_amount = 15;
	blur_scale = 1.0f;
	blur_strength = 0.3f;
	GLfloat cube_vertices[] = {
		// front

		-1, -1,  0.0f,
		1, -1,  0.0f,
		1,  1,  0.0f,
		-1,  1, 0.0f,
	};

	uint cube_elements[] = {
		// front
		0, 1, 2, 3

	};

	static const GLfloat g_UV_buffer_data[] = {
		0.0f, 0.0f,
		1.0f,  0.0f,
		1.0f,  1.0f,
		0.0f, 1.0f,
	};


	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), cube_vertices, GL_STATIC_DRAW);

glGenBuffers(1, &UVbuffer);
glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
glBufferData(GL_ARRAY_BUFFER, sizeof(g_UV_buffer_data), g_UV_buffer_data, GL_STATIC_DRAW);

glGenBuffers(1, &ibo_cube_elements);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(uint), cube_elements, GL_STATIC_DRAW);


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

	App->scene->scene_buff->Init("Scene");
	App->scene->final_buff->Init("Scene");
	App->scene->glow_buff->Init("Scene");
	App->scene->horizontal_blur_buff->Init("Scene");
	App->scene->vertical_blur_buff->Init("Scene");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}


// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	perf_timer.Start();




	App->scene->horizontal_blur_buff->Bind("Scene");
	BlurShaderVars(0);
	RenderSceneWiewport();
	App->scene->horizontal_blur_buff->UnBind("Scene");

	App->scene->vertical_blur_buff->Bind("Scene");
	BlurShaderVars(1);
	RenderSceneWiewport();
	App->scene->vertical_blur_buff->UnBind("Scene");

	if (!App->mode_game)
		App->scene->final_buff->Bind("Scene");
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
	GlowShaderVars();
	RenderSceneWiewport();

	App->render_gui->ScreenSpaceDraw();
	App->scene->final_buff->UnBind("Scene");



	/*ImGui::Begin("Test");
	ImGui::Image((ImTextureID*)App->scene->scene_buff->GetTexture(), ImVec2(256, 256));
	ImGui::SliderFloat("Strength", &blur_strength, 0.0f, 50.0f);
	ImGui::SliderInt("Amount", &blur_amount, 0.0f, 30.0f);
	ImGui::SliderFloat("Scale", &blur_scale, 0.0f, 50.0f);
	ImGui::End();*/

	ImGui::Render();

	static uint ScreenshotsNum = 0; //Used to enable multiple screenshots in the same second

	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
	{
		if (FirstPoint)
		{
			ImGuiIO GuiIO = ImGui::GetIO();
			Point1.x = GuiIO.MousePos.x;
			Point1.y = GuiIO.DisplaySize.y - GuiIO.MousePos.y;
			FirstPoint = false;
		}
		else
		{
			ImGuiIO GuiIO = ImGui::GetIO();
			Point2.x = GuiIO.MousePos.x;
			Point2.y = GuiIO.DisplaySize.y - GuiIO.MousePos.y;

			float2 MinPoint = float2((Point1.x <= Point2.x) ? Point1.x : Point2.x, (Point1.y <= Point2.y) ? Point1.y : Point2.y);
			float2 MaxPoint = float2((Point1.x >= Point2.x) ? Point1.x : Point2.x, (Point1.y >= Point2.y) ? Point1.y : Point2.y);

			uint width = abs(MaxPoint.x - MinPoint.x);
			uint height = abs(MaxPoint.y - MinPoint.y);
			unsigned char* s_pixels = new unsigned char[width * height * sizeof(unsigned char) * 3];
			glReadPixels(MinPoint.x, MinPoint.y, width, height, GL_RGB, GL_UNSIGNED_BYTE, s_pixels);
			ILuint imageID = ilGenImage();
			ilBindImage(imageID);
			ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, s_pixels);
			ilEnable(IL_FILE_OVERWRITE);

			/*
			static char tmp_string[1024];
			static std::string tmp_string2;
			sprintf_s(tmp_string, 1024, "Screenshots/ScreenPortion/ScreenPortion_%s_%s.png", __DATE__, __TIME__, tmp_string);
			tmp_string2 = tmp_string;
			for (uint i = 0; i < tmp_string2.length(); i++) if ((tmp_string2[i] == ' ') || (tmp_string2[i] == ':')) tmp_string2[i] = '_';
			*/
			
			time_t now = time(0);
			tm ltm;
			localtime_s(&ltm, &now);
			static char tmp_string[1024];
			sprintf_s(tmp_string, 1024, "Screenshots/ScreenPortion/ScreenPortion_%i_%i_%i_%i_%i_%i__%i.png", ltm.tm_mday, 1 + ltm.tm_mon, 1900 + ltm.tm_year, ltm.tm_hour, ltm.tm_min, ltm.tm_sec, ScreenshotsNum);

			ilSave(IL_PNG, tmp_string);
			ilDisable(IL_FILE_OVERWRITE);
			ilDeleteImage(imageID);
			RELEASE_ARRAY(s_pixels);

			FirstPoint = true;
			ScreenshotsNum++;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	{
		ILuint imageID = ilGenImage();
		ilBindImage(imageID);
		ilutGLScreen();
		ilEnable(IL_FILE_OVERWRITE);

		/*
		static char tmp_string[1024];
		static std::string tmp_string2;
		sprintf_s(tmp_string, 1024, "Screenshots/ScreenFull/ScreenFull_%s_%s.png", __DATE__, __TIME__, tmp_string);
		tmp_string2 = tmp_string;
		for (uint i = 0; i < tmp_string2.length(); i++) if ((tmp_string2[i] == ' ') || (tmp_string2[i] == ':')) tmp_string2[i] = '_';
		*/
		
		time_t now = time(0);
		tm ltm;
		localtime_s(&ltm, &now);
		static char tmp_string[1024];
		sprintf_s(tmp_string, 1024, "Screenshots/ScreenFull/ScreenFull_%i_%i_%i_%i_%i_%i__%i.png", ltm.tm_mday, 1 + ltm.tm_mon, 1900 + ltm.tm_year, ltm.tm_hour, ltm.tm_min, ltm.tm_sec, ScreenshotsNum);

		ilSave(IL_PNG, tmp_string);
		ilDisable(IL_FILE_OVERWRITE);
		ilDeleteImage(imageID);
		ScreenshotsNum++;
	}

	static bool gif_in_progress = false;
	static GifWriter gif_writer;
	static unsigned char* pixels = nullptr;

	if ((App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) && gif_in_progress)
	{
		GifEnd(&gif_writer);
		RELEASE_ARRAY(pixels);
		gif_in_progress = false;
		ScreenshotsNum++;
	}

	if ((App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) && !gif_in_progress)
	{
		time_t now = time(0);
		tm ltm;
		localtime_s(&ltm, &now);
		static char tmp_string[1024];
		sprintf_s(tmp_string, 1024, "Screenshots/GifFull/GifFull_%i_%i_%i_%i_%i_%i__%i.gif", ltm.tm_mday, 1 + ltm.tm_mon, 1900 + ltm.tm_year, ltm.tm_hour, ltm.tm_min, ltm.tm_sec, ScreenshotsNum);

		ImGuiIO GuiIO = ImGui::GetIO();
		gif_in_progress = GifBegin(&gif_writer, tmp_string, GuiIO.DisplaySize.x, GuiIO.DisplaySize.y, (uint32_t)(dt * 100.0f), 8, false);
		if (gif_in_progress)
			pixels = new unsigned char[GuiIO.DisplaySize.x *  GuiIO.DisplaySize.y * sizeof(unsigned char) * 4];
	}

	if (gif_in_progress)
	{
		/*
		ILuint imageID = ilGenImage();
		ilBindImage(imageID);
		ilutGLScreen();
		ILubyte* pixels = ilGetData();
		GifWriteFrame(&gif_writer, pixels, App->window->GetWidth(), App->window->GetHeight(), dt, 8, false);
		ilDeleteImage(imageID);
		*/
		ImGuiIO GuiIO = ImGui::GetIO();
		glReadPixels(0, 0, GuiIO.DisplaySize.x, GuiIO.DisplaySize.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		GifWriteFrame(&gif_writer, pixels, GuiIO.DisplaySize.x, GuiIO.DisplaySize.y, (uint32_t)(dt * 100.0f), 8, false);
	}

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

	RELEASE(default_texture);
	RELEASE(default_material);

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

CompCamera * ModuleRenderer3D::GetActiveCamera()
{
	return active_camera;
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
	App->window->SetWindowSize(width, height);

	//Reset the FOV
	active_camera->SetFov(active_camera->GetFOV());
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glLoadMatrixf(active_camera->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

float2 ModuleRenderer3D::LoadImage_devil(const char * theFileName, GLuint *buff)
{
	float2 texture_w_h;
	//Texture loading success
	bool textureLoaded = false;

	//Generate and set current image ID
	uint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);

	//Load image
	ILboolean success = ilLoadImage(theFileName);

	//Image loaded successfully
	if (success == IL_TRUE)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		//Convert image to RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (success == IL_TRUE)
		{
			textureLoaded = loadTextureFromPixels32((GLuint*)ilGetData(), (GLuint)ilGetInteger(IL_IMAGE_WIDTH), (GLuint)ilGetInteger(IL_IMAGE_HEIGHT), buff);
			texture_w_h.x = (uint)ilGetInteger(IL_IMAGE_WIDTH); texture_w_h.y = (uint)ilGetInteger(IL_IMAGE_HEIGHT);
			//Create texture from file pixels
			textureLoaded = true;
		}

		//Delete file from memory
		ilDeleteImages(1, &imgID);
	}

	return texture_w_h;

}

bool ModuleRenderer3D::loadTextureFromPixels32(GLuint * id_pixels, GLuint width_img, GLuint height_img, GLuint *buff)
{


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, buff);
	glBindTexture(GL_TEXTURE_2D, *buff);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_img, height_img,
		0, GL_RGBA, GL_UNSIGNED_BYTE, id_pixels);
	glBindTexture(GL_TEXTURE_2D, NULL);
	//Check for error
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error loading texture from %p pixels! %s\n", id_pixels, gluErrorString(error));
		return false;
	}

	return true;
}

void ModuleRenderer3D::RenderSceneWiewport()
{

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	//DRAW QUAD
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glUseProgram(0);
}

void ModuleRenderer3D::BlurShaderVars(int i)
{
	blur_shader_tex->Bind();

	glActiveTexture(GL_TEXTURE0);
	GLint texLoc = glGetUniformLocation(blur_shader_tex->programID, "albedo");

	if (i == 0)
		glBindTexture(GL_TEXTURE_2D, App->scene->glow_buff->GetTexture());
	else
		glBindTexture(GL_TEXTURE_2D, App->scene->horizontal_blur_buff->GetTexture());
	glUniform1i(texLoc, 0);


	GLint orientLoc = glGetUniformLocation(blur_shader_tex->programID, "_orientation");
	glUniform1i(orientLoc, i);

	GLint amountLoc = glGetUniformLocation(blur_shader_tex->programID, "BlurAmount");
	glUniform1i(amountLoc, blur_amount);

	GLint scaleLoc = glGetUniformLocation(blur_shader_tex->programID, "BlurScale");
	glUniform1f(scaleLoc, blur_scale);

	GLint strengthLoc = glGetUniformLocation(blur_shader_tex->programID, "BlurStrength");
	glUniform1f(strengthLoc, blur_strength);


}

void ModuleRenderer3D::GlowShaderVars()
{
	final_shader_tex->Bind();

	glActiveTexture(GL_TEXTURE0);
	GLint texLoc = glGetUniformLocation(final_shader_tex->programID, "_albedo");
	glBindTexture(GL_TEXTURE_2D, App->scene->scene_buff->GetTexture());
	glUniform1i(texLoc, 0);

	glActiveTexture(GL_TEXTURE1);
	texLoc = glGetUniformLocation(final_shader_tex->programID, "_glow_tex");
	glBindTexture(GL_TEXTURE_2D, App->scene->vertical_blur_buff->GetTexture());
	glUniform1i(texLoc, 1);

	glActiveTexture(GL_TEXTURE2);
	texLoc = glGetUniformLocation(final_shader_tex->programID, "_dmg_tex");
	glBindTexture(GL_TEXTURE_2D, dmg_texture_id);
	glUniform1i(texLoc, 2);

}