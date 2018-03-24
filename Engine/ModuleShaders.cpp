#include "ModuleShaders.h"
#include "ShadersLib.h"
#include "ModuleFS.h"
#include "Application.h"
#include "JSONSerialization.h"
#include "ModuleEventSystem.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "ModuleLightning.h"
#include "CompLight.h"
#include "GameObject.h"
#include "CompTransform.h"
#include "ModuleResourceManager.h"
#include "ModuleImporter.h"
#include "ImportMaterial.h"

ModuleShaders::ModuleShaders()
{
}

ModuleShaders::~ModuleShaders()
{
	for (auto item = materials.begin(); item != materials.end(); item++) {
		RELEASE(*item);
	}
	for (auto item = shaders.begin(); item != shaders.end(); item++) {
		RELEASE(*item);

	}
	for (auto item = programs.begin(); item != programs.end(); item++) {
		RELEASE(*item);
	}

}

bool ModuleShaders::Init(JSON_Object * node)
{
	Shader_Directory_fs = "Assets/Shaders";
	Material_Directory_fs = "Assets/Shaders/Materials";

	return true;
}

bool ModuleShaders::Start()
{

	//Import all the asset files related to shaders

	ImportShaderObjects();
	//ImportShaderMaterials();	

	return true;
}

update_status ModuleShaders::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleShaders::Update(float dt)
{
	std::vector<Material*>::iterator item = materials.begin();
	static float time_dt = 0;
	time_dt += dt * App->game_time.time_scale;
	while (item != materials.end())
	{
		if ((*item)->active_num == 0) {
			item++;
			continue;
		}

		(*item)->Bind();

		//TIME		
		GLint timeLoc = glGetUniformLocation((*item)->GetProgramID(), "_time");
		glUniform1f(timeLoc, time_dt);

		//CAMERA POSITION
		float3 cam_pos = App->camera->GetPos();
		GLint cameraLoc = glGetUniformLocation((*item)->GetProgramID(), "_cameraPosition");
		glUniform3fv(cameraLoc, 1, &cam_pos[0]);

		//ALPHA
		float alpha = (*item)->alpha;
		GLint alphaLoc = glGetUniformLocation((*item)->GetProgramID(), "_alpha");
		glUniform1f(alphaLoc, alpha);
		
		//LIGHTS
		GLint lightsizeLoc = glGetUniformLocation((*item)->GetProgramID(), "_numLights");
		std::vector<CompLight*> lights = App->module_lightning->GetSceneLights();
		glUniform1i(lightsizeLoc, lights.size());
		
		if(lightsizeLoc >= 0)
			for (size_t i = 0; i < lights.size(); ++i) {

				if(lights[i]->type == Light_type::DIRECTIONAL_LIGHT)
					SetLightUniform((*item)->GetProgramID(), "position", i,lights[i]->GetParent()->GetComponentTransform()->GetEulerToDirection());
				if (lights[i]->type == Light_type::POINT_LIGHT)
					SetLightUniform((*item)->GetProgramID(), "position", i, lights[i]->GetParent()->GetComponentTransform()->GetPosGlobal());

				SetLightUniform((*item)->GetProgramID(), "type", i, (int)lights[i]->type);
				SetLightUniform((*item)->GetProgramID(), "l_color", i, lights[i]->color);			
				SetLightUniform((*item)->GetProgramID(), "ambientCoefficient", i, lights[i]->ambientCoefficient);

				SetLightUniform((*item)->GetProgramID(), "properties", i, lights[i]->properties);

			}
		(*item)->Unbind();
		item++;
	}

	Enable_Text_Editor();
	
	if (App->gui->shader_program_creation) {
		SendEventWithAllShaders();
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleShaders::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleShaders::UpdateConfig(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleShaders::SaveConfig(JSON_Object * node)
{
	return true;
}

bool ModuleShaders::CleanUp()
{

	for (auto item = materials.begin(); item != materials.end(); item++) {
		(*item)->Save();
		
	}
	return true;
}

ShaderProgram* ModuleShaders::CreateShader(const char* name)
{

	ShaderProgram* newProgram = new ShaderProgram();

	newProgram->name = name;

	programs.push_back(newProgram);
	return newProgram;
}

char * ModuleShaders::GetShaderText(std::string path)
{

	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;
	std::string string_buffer;
	fopen_s(&pFile, path.c_str(), "rb");
	if (pFile == NULL) { fputs("File error", stderr); exit(1); }

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = new char[lSize + 1];// (char*)malloc(sizeof(char)*lSize);
	memset(buffer, 0, lSize + 1);
	if (buffer == NULL) { fputs("Memory error", stderr); exit(2); }

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);

	fclose(pFile);


	return buffer;
}

ShaderProgram * ModuleShaders::CreateDefaultShader(const char* name_text, const GLchar* const* fragment_text, const GLchar* const* vertex_text, const GLchar* const* geometry_text, bool push_in_list)
{
	ShaderProgram* defaultShader = new ShaderProgram();
	defaultShader->name = name_text;
	//Success flag
	GLint programSuccess = GL_TRUE;

	//Generate program
	defaultShader->programID = glCreateProgram();

	//Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Set vertex source
	glShaderSource(vertexShader, 1, vertex_text, NULL);

	//Compile vertex source
	glCompileShader(vertexShader);

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE)
	{
		GLchar infoLog[1024];
		glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
		LOG("Vertex shader compile error [%s]: %s", name_text, infoLog);
		return nullptr;
	}

	//Attach vertex shader to program
	glAttachShader(defaultShader->programID, vertexShader);

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);


	//Set fragment source
	glShaderSource(fragmentShader, 1, fragment_text, NULL);

	//Compile fragment source
	glCompileShader(fragmentShader);

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{
		GLchar infoLog[1024];
		glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
		LOG("Fragment shader compile error [%s]: %s", name_text, infoLog);
		return nullptr;
	}

	//Attach fragment shader to program
	glAttachShader(defaultShader->programID, fragmentShader);

	if(geometry_text != nullptr)
	{
		// Create geometry shader
		GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		// Set geometry source
		glShaderSource(geometryShader, 1, geometry_text, NULL);

		// Compile geometry
		glCompileShader(geometryShader);

		// Check errors on geometry
		GLint gShaderCompiled = GL_FALSE;
		
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &gShaderCompiled);
		if (gShaderCompiled != GL_TRUE)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(geometryShader, 1024, NULL, infoLog);
			LOG("Geometry shader compile error [%s]: %s", name_text, infoLog);
			return nullptr;
		}

		//Attach geometry shader to program
		glAttachShader(defaultShader->programID, geometryShader);
	}

	//Link program
	glLinkProgram(defaultShader->programID);

	//Check for errors
	glGetProgramiv(defaultShader->programID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		//ProgramLog(default_shader.mProgramID);
		return nullptr;
	}

	Shader* newFragment = new Shader();
	newFragment->shaderID = fragmentShader;
	newFragment->shaderText = *fragment_text;
	newFragment->shaderType = ShaderType::fragment;
	newFragment->name =  name_text;
	newFragment->name.append("_frag");
	newFragment->shaderPath = "";

	defaultShader->AddFragment(newFragment);

	Shader* newVertex = new Shader();
	newVertex->shaderID = vertexShader;
	newVertex->shaderText = *vertex_text;
	newVertex->shaderType = ShaderType::vertex;
	newVertex->name = name_text;
	newVertex->name.append("_vert");
	newVertex->shaderPath = "";

	defaultShader->AddVertex(newVertex);


	

	if (push_in_list) {
		programs.push_back(defaultShader);
		shaders.push_back(newFragment);
		shaders.push_back(newVertex);

	}
	
	return defaultShader;

}

Shader* ModuleShaders::CompileShader(std::string path, std::string name, ShaderType type)
{
	uint id = 0;
	switch (type) {
	case fragment:
		id = glCreateShader(GL_FRAGMENT_SHADER);
		break;

	case vertex:
		id = glCreateShader(GL_VERTEX_SHADER);
		break;

	case geometry:
		id = glCreateShader(GL_GEOMETRY_SHADER);
		break;
	}


	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;
	std::string string_buffer;
	fopen_s(&pFile, path.c_str(), "rb");
	if (pFile == NULL) { fputs("File error", stderr); exit(1); }

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = new char[lSize + 1];// (char*)malloc(sizeof(char)*lSize);
	memset(buffer, 0, lSize + 1);
	if (buffer == NULL) { fputs("Memory error", stderr); exit(2); }

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);

	fclose(pFile);

	//Set fragment source
	glShaderSource(id, 1, (const GLchar* const *)&buffer, NULL);

	//Compile fragment source
	glCompileShader(id);

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{
		last_shader_error = GetShaderError(id);
		return nullptr;
	}

	Shader* newShader = new Shader();
	newShader->shaderType = type;
	newShader->shaderID = id;
	newShader->name = name;
	newShader->shaderPath = path;
	newShader->shaderText = buffer;

	AddShaderList(newShader);
	delete[] buffer;
	return newShader;

}

std::string ModuleShaders::GetShaderError(uint ID)
{
	std::string ret = "";
	
	int infoLogLength = 0;
	int maxLength = infoLogLength;

	glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &maxLength);

	char* infoLog = new char[maxLength];
	glGetShaderInfoLog(ID, maxLength, &infoLogLength, infoLog);

	if (infoLogLength > 0) {

		//LOG("%s\n", infoLog);
		ret = infoLog;
	}

	delete[] infoLog;

	return ret;
}

void ModuleShaders::AddShaderList(Shader* newShader)
{
	shaders.push_back(newShader);
}

void ModuleShaders::ImportShaderObjects()
{
	namespace stdfs = std::experimental::filesystem;
	char* buffer = nullptr;
	//Iterating all files
	for (stdfs::directory_iterator::value_type item : stdfs::directory_iterator(Shader_Directory_fs))
	{
		std::string str_path = item.path().string().c_str();

		//Extracting the extension file
		std::string extension_path=	App->fs->GetExtension(str_path);

		if (extension_path == "vert" || extension_path == "frag" || extension_path == "geom" || extension_path == "mat") 
		{

			//Extracting name of the file
			size_t size_name_front = str_path.rfind("\\")+1;
			size_t size_name_end = str_path.rfind(".");
			std::string name = str_path.substr(size_name_front, size_name_end - size_name_front);

			//Loading the file to extract the file buffer information
			
			App->fs->LoadFile(str_path.c_str(), &buffer, DIRECTORY_IMPORT::IMPORT_DEFAULT);

			//If the shader object is vertex
			if (extension_path == "vert") 
			{
				
				if (buffer != nullptr) 
				{
					//Compile shader
					Shader* shader_temp = CompileShader(str_path, name,ShaderType::vertex);
				}

			}

			//If the shader object is fragment
			else if (extension_path == "frag") 
			{

				if (buffer != nullptr) 
				{
					//Compile shader
					Shader* shader_temp = CompileShader(str_path, name,ShaderType::fragment);
				}

			}

			//If the shader object is geometry
			else if (extension_path == "geom") 
			{

				if (buffer != nullptr) 
				{
					//Compile shader
					Shader* shader_temp = CompileShader(str_path, name,ShaderType::geometry);
				}
			}

			//maybe delete buffer?
			RELEASE_ARRAY(buffer);
		}
	}

	

}

void ModuleShaders::ImportShaderMaterials()
{

	namespace stdfs = std::experimental::filesystem;
	ShaderProgram* mat_shader = nullptr;
	char* buffer = nullptr;
	//Iterating all files
	for (stdfs::directory_iterator::value_type item : stdfs::directory_iterator(Shader_Directory_fs))
	{
		std::string str_path = item.path().string().c_str();

		//Extracting the extension file
		std::string extension_path = App->fs->GetExtension(str_path);

			if (extension_path == "mat")
			{
				//Extracting name of the file
				size_t size_name_front = str_path.rfind("\\") + 1;
				size_t size_name_end = str_path.rfind(".");
				std::string name = str_path.substr(size_name_front, size_name_end - size_name_front);

				//Loading the file to extract the file buffer information
			
				App->fs->LoadFile(str_path.c_str(), &buffer, DIRECTORY_IMPORT::IMPORT_DEFAULT);

				if (buffer != nullptr)
				{
					JSON_Object* obj_proj;
					JSON_Value* file_proj;
					//Point JSON_Object and JSON_Value to the path we want
					App->json_seria->Create_Json_Doc(&file_proj, &obj_proj, str_path.c_str());

					//Creating the shader program
					mat_shader = CreateShader(name.c_str());

					//If the program has a fragment shader
					if (json_object_has_value(obj_proj, "Fragment Shader") > 0)
					{
						std::string frag_name = json_object_get_string(obj_proj, "Fragment Shader");

						//Find the shader object by name
						mat_shader->fragment = GetShaderByName(frag_name.c_str(), ShaderType::fragment);
					}

					//If the program has a vertex shader
					if (json_object_has_value(obj_proj, "Vertex Shader") > 0)
					{
						std::string vertex_name = json_object_get_string(obj_proj, "Vertex Shader");

						//Find the shader object by name
						mat_shader->vertex = GetShaderByName(vertex_name.c_str(), ShaderType::vertex);
					}

					//If the program has a geometry shader
					if (json_object_has_value(obj_proj, "Geometry Shader"))
					{
						std::string geometry_name = json_object_get_string(obj_proj, "Geometry Shader");

						//Find the shader object by name
						mat_shader->geometry = GetShaderByName(geometry_name.c_str(), ShaderType::geometry);
					}

					//Store the program
					mat_shader->LoadProgram();
					
					mat_shader->CreateMaterialFile();

					//IF default material found, change it
					if (App->fs->GetOnlyName(str_path).compare("DefaultShader") == 0)
					{
						App->renderer3D->default_shader = mat_shader;
						App->renderer3D->default_material->material_shader = mat_shader;
					}
					RELEASE_ARRAY(buffer);
				}

			}
		
	}
	

	for (stdfs::directory_iterator::value_type item : stdfs::directory_iterator(Material_Directory_fs))
	{
		std::string str_path = item.path().string().c_str();

		//Extracting the extension file
		std::string extension_path = App->fs->GetExtension(str_path);

		if (extension_path == "material")
		{
			LoadMaterial(str_path);

		}
	}
}

void ModuleShaders::LoadShaderMaterials()
{
	namespace stdfs = std::experimental::filesystem;

	for (stdfs::directory_iterator::value_type item : stdfs::directory_iterator(Material_Directory_fs))
	{
		std::string str_path = item.path().string().c_str();

		//Extracting the extension file
		std::string extension_path = App->fs->GetExtension(str_path);

		if (extension_path == "material")
		{
			LoadMaterial(str_path, false);

		}
	}
}

Material * ModuleShaders::LoadMaterial(std::string str_path, bool load_vars)
{

	
	char* buffer = nullptr;
	App->fs->LoadFile(str_path.c_str(), &buffer, DIRECTORY_IMPORT::IMPORT_DEFAULT);

	JSON_Object* object;
	JSON_Value* file_proj;
	//Point JSON_Object and JSON_Value to the path we want
	App->json_seria->Create_Json_Doc(&file_proj, &object, str_path.c_str());
	std::string name = App->fs->GetOnlyName(str_path);
	std::string program_name = json_object_dotget_string_with_std(object, name + "ShaderName:");
	Material* material = nullptr;
	ShaderProgram* program = nullptr;
	for (int i = 0; i < App->module_shaders->programs.size(); i++)	{

		if (strcmp(App->module_shaders->programs[i]->name.c_str(), program_name.c_str()) == 0)
		{
			program = App->module_shaders->programs[i];
			
			break;
		}

	}
	if (program != nullptr)
	{
		std::string material_name = App->fs->GetOnlyName(str_path);
		Material* material = nullptr;
		for (int i = 0; i < App->module_shaders->materials.size(); i++)
		{
			if (strcmp(App->module_shaders->materials[i]->name.c_str(), material_name.c_str()) == 0)
			{
				material = App->module_shaders->materials[i];
				
				break;
			}

		}

		if (material == nullptr) {
			material = new Material();
			material->material_shader = program;
			material->name = material_name;
			material->GetProgramVariables();
			uint num_textures = json_object_dotget_number_with_std(object, name + "Num Textures:");
			if (load_vars) {
				for (int i = 0; i < num_textures; i++)
				{
					if (i >= material->textures.size()) break;
					char mat_name[128] = { 0 };

					char* num = new char[4];
					itoa(i, num, 10);
					strcat(mat_name, "Resource Material UUID ");
					strcat(mat_name, num);
					RELEASE_ARRAY(num);

					uint uuid = json_object_dotget_number_with_std(object, name + mat_name);

					material->textures[i].value = (ResourceMaterial*)App->resource_manager->GetResource(uuid);
					if (material->textures[i].value != nullptr)
					{
						material->textures[i].value->num_game_objects_use_me++;

						// LOAD MATERIAL -------------------------
						if (material->textures[i].value->IsLoadedToMemory() == Resource::State::UNLOADED)
						{
							App->importer->iMaterial->LoadResource(std::to_string(material->textures[i].value->GetUUID()).c_str(), material->textures[i].value);
						}
					}
					else {
						material->textures[i].value = App->renderer3D->default_texture;
					}
				}
				uint num_bools = json_object_dotget_number_with_std(object, name + "Num Bools:");
				for (int i = 0; i < num_bools; i++)
				{
					std::ostringstream ss;
					ss << name << "Bool:" << i;
					std::string json_name = ss.str();

					if (i >= material->bool_variables.size()) break;
					material->bool_variables[i].value = json_object_dotget_boolean_with_std(object, json_name);
				}
				uint num_ints = json_object_dotget_number_with_std(object, name + "Num Int:");
				for (int i = 0; i < num_ints; i++)
				{
					std::ostringstream ss;
					ss << name << "Int:" << i;
					std::string json_name = ss.str();

					if (i >= material->bool_variables.size()) break;
					material->int_variables[i].value = json_object_dotget_number_with_std(object, json_name);
				}

				uint num_floats = json_object_dotget_number_with_std(object, name + "Num Floats:");
				for (int i = 0; i < num_floats; i++)
				{
					std::ostringstream ss;
					ss << name << "Float:" << i;
					std::string json_name = ss.str();

					if (i >= material->float_variables.size()) break;
					material->float_variables[i].value = json_object_dotget_number_with_std(object, json_name);
				}
				uint num_float3 = json_object_dotget_number_with_std(object, name + "Num Float3:");
				for (int i = 0; i < num_float3; i++)
				{
					std::ostringstream ss;
					ss << name << "Float3:" << i;
					std::string json_name = ss.str();

					if (i >= material->float3_variables.size()) break;
					material->float3_variables[i].value = App->fs->json_array_dotget_float3_string(object, json_name);
				}
				uint num_colors = json_object_dotget_number_with_std(object, name + "Num Colors:");
				for (int i = 0; i < num_colors; i++)
				{
					std::ostringstream ss;
					ss << name << "Color:" << i;
					std::string json_name = ss.str();

					if (i >= material->color_variables.size()) break;
					material->color_variables[i].value = App->fs->json_array_dotget_float4_string(object, json_name);
				}
			}
			App->module_shaders->materials.push_back(material);
		}
	}
	

	return nullptr;
}

Shader * ModuleShaders::GetShaderByName(const char * name, ShaderType type)
{

	for (std::vector<Shader*>::const_iterator item = shaders.cbegin(); item != shaders.cend(); item++) 
	{
		std::string str_path = (*item)->shaderPath.c_str();
		std::string extension_path = App->fs->GetExtension(str_path);
		if ((*item)->name == name) 
		{

			//This is to led all the shader objects to have the same name and the engine will
			//find them searching by name and extension too.

			switch (type) 
			{
			case ShaderType::fragment:
				if (extension_path=="frag") return *item;break;

			case ShaderType::vertex:
				if (extension_path == "vert") return *item;break;

			case ShaderType::geometry:
				if (extension_path == "geom") return *item;break;
			}
		}
	}
	return nullptr;
}


bool ModuleShaders::SetEventListenrs()
{
	AddListener(EventType::EVENT_OPEN_SHADER_EDITOR, this);
	AddListener(EventType::EVENT_CREATE_SHADER_PROGRAM, this);
	return true;
}

void ModuleShaders::OnEvent(Event & event)
{
	std::string path;
	switch (event.type)
	{
	case EventType::EVENT_OPEN_SHADER_EDITOR:

		path = Shader_Directory_fs+ "/" + event.shader_editor.name;

		switch (event.shader_editor.shader_type)
		{
		case ShaderType::fragment:
			path += ".frag";
			break;
		case ShaderType::vertex:
			path += ".vert";
			break;
		case ShaderType::geometry:
			path += ".geom";
			break;
		}
		shader_text_active.name = event.shader_editor.name;
		shader_text_active.shaderPath = path;
		shader_text_active.shaderType = event.shader_editor.shader_type;
		enable_editor = event.shader_editor.open_editor;
		break;
	case EventType::EVENT_CREATE_SHADER_PROGRAM:
		path = Shader_Directory_fs + "/" + event.shader_program.name + ".mat";
		ShaderProgram * program=CreateShader(event.shader_program.name);
		program->path = path;
		switch (event.shader_program.Shader1->shaderType) {
		case ShaderType::vertex:
			program->AddVertex(event.shader_program.Shader1);
			break;
		case ShaderType::fragment:
			program->AddFragment(event.shader_program.Shader2);
			break;
		case ShaderType::geometry:
			program->AddGeometry(event.shader_program.Shader2);
			break;
		}

		switch (event.shader_program.Shader2->shaderType) {
		case ShaderType::vertex:
			program->AddVertex(event.shader_program.Shader2);
			break;
		case ShaderType::fragment:
			program->AddFragment(event.shader_program.Shader2);
			break;
		case ShaderType::geometry:
			program->AddGeometry(event.shader_program.Shader2);
			break;
		}
		program->LoadProgram();
		program->CreateMaterialFile();
		break;

	}
}

void ModuleShaders::Enable_Text_Editor()
{


	if (enable_editor) {
	
			std::string temp_name= "Shader editor";
			ImGui::Begin(temp_name.c_str(), &enable_editor);

			if (editor_shaders.GetText().size()==0) {
				editor_shaders.InsertText("#version 330 core\n void main(){\n }");
			}

			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[1]);
			editor_shaders.Render(temp_name.c_str());
			ImGui::PopFont();
			if (ImGui::Button("Save"))
			{
				FILE* pFile = fopen(shader_text_active.shaderPath.c_str(), "wb");
				uint buffer_size = strlen(editor_shaders.GetText().c_str());
				fwrite(editor_shaders.GetText().c_str(), sizeof(char), buffer_size, pFile);
				fclose(pFile);
				CompileShader(shader_text_active.shaderPath, shader_text_active.name, shader_text_active.shaderType);
				enable_editor = false;
			}
			ImGui::End();
	}
}

void ModuleShaders::SetUniform(uint ID, const GLchar * uniformName, float3 & v)
{
	GLint var_loc = glGetUniformLocation(ID, uniformName);
	glUniform3fv(var_loc, 1, &v[0]);

}
void ModuleShaders::SetUniform(uint ID, const GLchar * uniformName, float4 & v)
{
	GLint var_loc = glGetUniformLocation(ID, uniformName);
	glUniform4fv(var_loc, 1, &v[0]);

}

void ModuleShaders::SetUniform(uint ID, const GLchar * uniformName, float & v)
{
	GLint var_loc = glGetUniformLocation(ID, uniformName);
	glUniform1f(var_loc, v);

}

void ModuleShaders::SetUniform(uint ID, const GLchar * uniformName, int & v)
{
	GLint var_loc = glGetUniformLocation(ID, uniformName);
	glUniform1i(var_loc, v);

}

void ModuleShaders::SendEventWithAllShaders() 
{
	Event shader_event;
	shader_event.send_shader_object.type = EventType::EVENT_SEND_ALL_SHADER_OBJECTS;
	shader_event.send_shader_object.shaders = &shaders;
	PushEvent(shader_event);
	App->gui->shader_program_creation = false;
}