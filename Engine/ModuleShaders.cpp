#include "ModuleShaders.h"
#include "ShadersLib.h"
#include "ModuleFS.h"
#include "Application.h"
#include "JSONSerialization.h"
#include "ModuleEventSystemV2.h"
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
#include "CubeMap_Texture.h"

ModuleShaders::ModuleShaders()
{
	name = "Shaders";
	Start_enabled = true;
	Update_enabled = true;
}

ModuleShaders::~ModuleShaders()
{
	for (auto item = App->module_shaders->materials.begin(); item != App->module_shaders->materials.end(); item++) {
		RELEASE((*item).second);
	}
	materials.clear();
	for (auto item = shaders.begin(); item != shaders.end(); item++) {
		RELEASE(*item);

	}
	shaders.clear();
	for (auto item = programs.begin(); item != programs.end(); item++) {
		RELEASE(*item);
	}
	programs.clear();
	editor_shaders.Delete();
}

bool ModuleShaders::Init(JSON_Object * node)
{
	Shader_Directory_fs = "Assets/Shaders";
	Material_Directory_fs = "Assets/Shaders/Materials";

	return true;
}

bool ModuleShaders::Start()
{
	perf_timer.Start();
	//Import all the asset files related to shaders
	if (App->build_mode == false)
	{
		ImportShaderObjects();
	}

	//ImportShaderMaterials();	
	Start_t = perf_timer.ReadMs();
	return true;
}

update_status ModuleShaders::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleShaders::Update(float dt)
{
	BROFILER_CATEGORY("Update: ModuleShaders", Profiler::Color::Blue);
	perf_timer.Start();

	Enable_Text_Editor();

	SetGlobalVariables(dt);

	if (App->gui->shader_program_creation) {
		SendEventWithAllShaders();
	}
	Update_t = perf_timer.ReadMs();
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
		(*item).second->Save();

	}
	RELEASE(App->renderer3D->default_texture);
	RELEASE(App->renderer3D->default_material);
	return true;
}

ShaderProgram* ModuleShaders::CreateShader(const char* name)
{

	ShaderProgram* newProgram = new ShaderProgram();

	newProgram->name = name;

	if (strcmp(name,"DefaultShader") != 0)
	{
		programs.push_back(newProgram);
	}
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

	if (geometry_text != nullptr)
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
	newFragment->name = name_text;
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
	buffer = new char[lSize + 1];
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
		RELEASE_ARRAY(buffer);
		return nullptr;
	}

	Shader* newShader = new Shader();
	newShader->shaderType = type;
	newShader->shaderID = id;
	newShader->name = name;
	newShader->shaderPath = path;
	newShader->shaderText = buffer;

	AddShaderList(newShader);
	RELEASE_ARRAY(buffer);
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
		std::string extension_path = App->fs->GetExtension(str_path);

		if (extension_path == "vert" || extension_path == "frag" || extension_path == "geom" || extension_path == "mat")
		{

			//Extracting name of the file
			size_t size_name_front = str_path.rfind("\\") + 1;
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
					Shader* shader_temp = CompileShader(str_path, name, ShaderType::vertex);
				}

			}

			//If the shader object is fragment
			else if (extension_path == "frag")
			{

				if (buffer != nullptr)
				{
					//Compile shader
					Shader* shader_temp = CompileShader(str_path, name, ShaderType::fragment);
				}

			}

			//If the shader object is geometry
			else if (extension_path == "geom")
			{

				if (buffer != nullptr)
				{
					//Compile shader
					Shader* shader_temp = CompileShader(str_path, name, ShaderType::geometry);
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
					App->module_shaders->programs[0] = mat_shader;
					App->renderer3D->default_shader = mat_shader;
					App->renderer3D->default_material->material_shader = mat_shader;
				}

				json_object_clear(obj_proj);
				json_value_free(file_proj);
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
	delete[] buffer;
	buffer = nullptr;

	JSON_Object* object;
	JSON_Value* file_proj;
	bool insert = false;
	//Point JSON_Object and JSON_Value to the path we want
	App->json_seria->Create_Json_Doc(&file_proj, &object, str_path.c_str());
	std::string name = App->fs->GetOnlyName(str_path);
	std::string program_name = json_object_dotget_string_with_std(object, name + "ShaderName:");
	Material* material = nullptr;
	ShaderProgram* program = nullptr;
	for (int i = 0; i < App->module_shaders->programs.size(); i++) {

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
		for (auto item = App->module_shaders->materials.begin(); item != App->module_shaders->materials.end(); item++)
		{
			if (strcmp((*item).second->name.c_str(), material_name.c_str()) == 0)
			{
				material = (*item).second;

				break;
			}

		}

		if (material == nullptr) {
			material = new Material();			
			material->name = material_name;
			insert = true;
		}
			material->material_shader = program;
			material->GetProgramVariables();
			uint num_textures = json_object_dotget_number_with_std(object, name + "Num Textures:");
			material->glow = json_object_dotget_boolean_with_std(object, name + "Glow:");
			material->cast_shadows = json_object_dotget_boolean_with_std(object, name + "CastShadow:");
			material->alpha = json_object_dotget_number_with_std(object, name + "Alpha:");
			material->m_source_type = json_object_dotget_number_with_std(object, name + "Source Blend:");
			material->m_destiny_type = json_object_dotget_number_with_std(object, name + "Destiny Blend:");

			material->SetDestinyBlendMode();
			material->SetSourceBlendMode();

			if (load_vars) {
				for (int i = 0; i < num_textures; i++)
				{
					if (i >= material->textures.size()) break;
					char mat_name[128] = { 0 };
					char mat_var[128] = { 0 };

					char* num = new char[4];
					itoa(i, num, 10);

					strcat(mat_name, "Resource Material UUID ");
					strcat(mat_name, num);

					strcat(mat_var, "Resource Material Var Name ");
					strcat(mat_var, num);

					RELEASE_ARRAY(num);

					uint uuid = json_object_dotget_number_with_std(object, name + mat_name);
					std::string var_name = json_object_dotget_string_with_std(object, name + mat_var);

					if (strcmp(var_name.c_str(), material->textures[i].var_name.c_str()) == 0) {
						material->textures[i].value = (ResourceMaterial*)App->resource_manager->GetResource(uuid);
					}
					else {
						for (int n = 0; n < num_textures; n++) {
							char temp_name[128] = { 0 };
							char temp_var[128] = { 0 };

							char* temp_num = new char[4];
							itoa(n, temp_num, 10);

							strcat(temp_name, "Resource Material UUID ");
							strcat(temp_name, temp_num);

							strcat(temp_var, "Resource Material Var Name ");
							strcat(temp_var, temp_num);
							RELEASE_ARRAY(temp_num);

							uint temp_uuid = json_object_dotget_number_with_std(object, name + temp_name);
							std::string var_name_temp = json_object_dotget_string_with_std(object, name + temp_var);

							if (strcmp(var_name_temp.c_str(), material->textures[i].var_name.c_str()) == 0) {
								material->textures[i].value = (ResourceMaterial*)App->resource_manager->GetResource(temp_uuid);
								break;
							}
						}
					}
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

					std::ostringstream ss2;
					ss2 << name << "Bool Name:" << i;
					std::string json_var = ss2.str();

					if (i >= material->bool_variables.size()) break;
					std::string var_name = json_object_dotget_string_with_std(object, json_var);

					if (strcmp(var_name.c_str(), material->bool_variables[i].var_name.c_str()) == 0) {
						material->bool_variables[i].value = json_object_dotget_boolean_with_std(object, json_name);
					}
					else {
						for (int n = 0; n < num_bools; n++) {
							std::ostringstream temp_ss;
							temp_ss << name << "Bool:" << n;
							std::string temp_name = temp_ss.str();

							std::ostringstream temp_ss2;
							temp_ss2 << name << "Bool Name:" << n;
							std::string temp_var = temp_ss2.str();

							std::string temp_var_name = json_object_dotget_string_with_std(object, temp_var);

							if (strcmp(temp_var_name.c_str(), material->bool_variables[i].var_name.c_str()) == 0) {
								material->bool_variables[i].value = json_object_dotget_boolean_with_std(object, temp_name);
								break;
							}

						}
					}
				}
				uint num_ints = json_object_dotget_number_with_std(object, name + "Num Ints:");
				for (int i = 0; i < num_ints; i++)
				{
					std::ostringstream ss;
					ss << name << "Int:" << i;
					std::string json_name = ss.str();

					std::ostringstream ss2;
					ss2 << name << "Int Name:" << i;
					std::string json_var = ss2.str();

					if (i >= material->int_variables.size()) break;
					std::string var_name = json_object_dotget_string_with_std(object, json_var);

					if (strcmp(var_name.c_str(), material->int_variables[i].var_name.c_str()) == 0) {
						material->int_variables[i].value = json_object_dotget_number_with_std(object, json_name);
					}
					else {
						for (int n = 0; n < num_ints; n++) {
							std::ostringstream temp_ss;
							temp_ss << name << "Int:" << n;
							std::string temp_name = temp_ss.str();

							std::ostringstream temp_ss2;
							temp_ss2 << name << "Int Name:" << n;
							std::string temp_var = temp_ss2.str();

							std::string temp_var_name = json_object_dotget_string_with_std(object, temp_var);

							if (strcmp(temp_var_name.c_str(), material->int_variables[i].var_name.c_str()) == 0) {
								material->int_variables[i].value = json_object_dotget_number_with_std(object, temp_name);
								break;
							}

						}
					}
				}

				uint num_floats = json_object_dotget_number_with_std(object, name + "Num Floats:");
				for (int i = 0; i < num_floats; i++)
				{
					std::ostringstream ss;
					ss << name << "Float:" << i;
					std::string json_name = ss.str();

					std::ostringstream ss2;
					ss2 << name << "Float Name:" << i;
					std::string json_var = ss2.str();

					if (i >= material->float_variables.size()) break;
					std::string var_name = json_object_dotget_string_with_std(object, json_var);

					if (strcmp(var_name.c_str(), material->float_variables[i].var_name.c_str()) == 0) {
						material->float_variables[i].value = json_object_dotget_number_with_std(object, json_name);
					}
					else {
						for (int n = 0; n < num_floats; n++) {
							std::ostringstream temp_ss;
							temp_ss << name << "Float:" << n;
							std::string temp_name = temp_ss.str();

							std::ostringstream temp_ss2;
							temp_ss2 << name << "Float Name:" << n;
							std::string temp_var = temp_ss2.str();

							std::string temp_var_name = json_object_dotget_string_with_std(object, temp_var);

							if (strcmp(temp_var_name.c_str(), material->float_variables[i].var_name.c_str()) == 0) {
								material->float_variables[i].value = json_object_dotget_number_with_std(object, temp_name);
								break;
							}

						}
					}
				}
				uint num_float3 = json_object_dotget_number_with_std(object, name + "Num Float3:");
				for (int i = 0; i < num_float3; i++)
				{
					std::ostringstream ss;
					ss << name << "Float3:" << i;
					std::string json_name = ss.str();

					std::ostringstream ss2;
					ss2 << name << "Float3 Name:" << i;
					std::string json_var = ss2.str();

					if (i >= material->float3_variables.size()) break;
					std::string var_name = json_object_dotget_string_with_std(object, json_var);

					if (strcmp(var_name.c_str(), material->float3_variables[i].var_name.c_str()) == 0) {
						material->float3_variables[i].value = App->fs->json_array_dotget_float3_string(object, json_name);
					}
					else {
						for (int n = 0; n < num_float3; n++) {
							std::ostringstream temp_ss;
							temp_ss << name << "Float3:" << n;
							std::string temp_name = temp_ss.str();

							std::ostringstream temp_ss2;
							temp_ss2 << name << "Float3 Name:" << n;
							std::string temp_var = temp_ss2.str();

							std::string temp_var_name = json_object_dotget_string_with_std(object, temp_var);

							if (strcmp(temp_var_name.c_str(), material->float3_variables[i].var_name.c_str()) == 0) {
								material->float3_variables[i].value = App->fs->json_array_dotget_float3_string(object, temp_name);
								break;
							}

						}
					}
				}
				uint num_colors = json_object_dotget_number_with_std(object, name + "Num Colors:");
				for (int i = 0; i < num_colors; i++)
				{
					std::ostringstream ss;
					ss << name << "Color:" << i;
					std::string json_name = ss.str();

					std::ostringstream ss2;
					ss2 << name << "Color Name:" << i;
					std::string json_var = ss2.str();

					if (i >= material->color_variables.size()) break;
					std::string var_name = json_object_dotget_string_with_std(object, json_var);

					if (strcmp(var_name.c_str(), material->color_variables[i].var_name.c_str()) == 0) {
						material->color_variables[i].value = App->fs->json_array_dotget_float4_string(object, json_name);
					}
					else {
						for (int n = 0; n < num_colors; n++) {
							std::ostringstream temp_ss;
							temp_ss << name << "Color:" << n;
							std::string temp_name = temp_ss.str();

							std::ostringstream temp_ss2;
							temp_ss2 << name << "Color Name:" << n;
							std::string temp_var = temp_ss2.str();

							std::string temp_var_name = json_object_dotget_string_with_std(object, temp_var);

							if (strcmp(temp_var_name.c_str(), material->color_variables[i].var_name.c_str()) == 0) {
								material->color_variables[i].value = App->fs->json_array_dotget_float4_string(object, temp_name);
								break;
							}

						}
					}
				}
				uint num_cubes = json_object_dotget_number_with_std(object, name + "Num Cubes:");
				for (int i = 0; i < num_cubes; i++)
				{
					std::ostringstream ss;
					ss << name << "Cube:" << i;
					std::string json_name = ss.str();

					std::ostringstream ss2;
					ss2 << name << "Cube Name:" << i;
					std::string json_var = ss2.str();

					if (i >= material->cube_maps.size()) break;
					std::string var_name = json_object_dotget_string_with_std(object, json_var);

					if (strcmp(var_name.c_str(), material->cube_maps[i].var_name.c_str()) == 0) {
						material->cube_maps[i].cube_name = json_object_dotget_string_with_std(object, json_name);
					}
					else {
						for (int n = 0; n < num_cubes; n++) {
							std::ostringstream temp_ss;
							temp_ss << name << "Cube:" << n;
							std::string temp_name = temp_ss.str();

							std::ostringstream temp_ss2;
							temp_ss2 << name << "Cube Name:" << n;
							std::string temp_var = temp_ss2.str();

							std::string temp_var_name = json_object_dotget_string_with_std(object, temp_var);

							if (strcmp(temp_var_name.c_str(), material->cube_maps[i].var_name.c_str()) == 0) {

								material->cube_maps[i].cube_name = json_object_dotget_string_with_std(object, temp_name);

								break;
							}

						}
					}
				}
			}
			if(insert) 
				App->module_shaders->materials.insert(std::pair<uint, Material*>(material->GetProgramID(), material));
		
	}
	json_value_free(file_proj);

	return nullptr;
}

Material * ModuleShaders::GetMaterialByName(const char * name)
{
	for (auto item = App->module_shaders->materials.begin(); item != App->module_shaders->materials.end(); item++)
	{
		std::string str_path = (*item).second->name.c_str();

		if (strcmp(str_path.c_str(), name) == 0) {
			return (*item).second;
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
				if (extension_path == "frag") return *item; break;

			case ShaderType::vertex:
				if (extension_path == "vert") return *item; break;

			case ShaderType::geometry:
				if (extension_path == "geom") return *item; break;
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
	BROFILER_CATEGORY("OnEvent: ModuleShaders", Profiler::Color::Blue);
	std::string path;
	switch (event.Get_event_data_type())
	{
	case EventType::EVENT_OPEN_SHADER_EDITOR:

		path = Shader_Directory_fs + "/" + event.shader_editor.name;

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
		ShaderProgram * program = CreateShader(event.shader_program.name);
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

		std::string temp_name = "Shader editor";
		ImGui::Begin(temp_name.c_str(), &enable_editor);

		if (editor_shaders.GetText().size() == 0) {
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


void ModuleShaders::SetUniformVariables(Material * material)
{
	material->RestartIterators();
	//BOOL
	if (material->bool_variables.size() != 0)
		while (material->it_bool_variables != material->bool_variables.end()) {

			GLint bool_loc = glGetUniformLocation(material->GetProgramID(), (*material->it_bool_variables).var_name.c_str());
			glUniform1i(bool_loc, (*material->it_bool_variables).value);
			material->it_bool_variables++;
		}
	//INT
	if (material->int_variables.size() != 0)
		while (material->it_int_variables != material->int_variables.end()) {
			GLint int_loc = glGetUniformLocation(material->GetProgramID(), (*material->it_int_variables).var_name.c_str());
			glUniform1i(int_loc, (*material->it_int_variables).value);
			material->it_int_variables++;
		}
	if (material->float_variables.size() != 0)
		while (material->it_float_variables != material->float_variables.end()) {
			GLint float_loc = glGetUniformLocation(material->GetProgramID(), (*material->it_float_variables).var_name.c_str());
			glUniform1f(float_loc, (*material->it_float_variables).value);
			material->it_float_variables++;
		}

	if (material->float3_variables.size() != 0)
		while (material->it_float3_variables != material->float3_variables.end()) {
			GLint float3_loc = glGetUniformLocation(material->GetProgramID(), (*material->it_float3_variables).var_name.c_str());
			glUniform3fv(float3_loc, 1, &(*material->it_float3_variables).value[0]);
			material->it_float3_variables++;
		}

	if (material->color_variables.size() != 0)
		while (material->it_color_variables != material->color_variables.end()) {
			GLint color_loc = glGetUniformLocation(material->GetProgramID(), (*material->it_color_variables).var_name.c_str());
			glUniform4fv(color_loc, 1, &(*material->it_color_variables).value[0]);
			material->it_color_variables++;
		}

	//Textures
	if (material->textures.size() != 0)
	{
		for (uint i = 0; i < material->textures.size(); i++)
		{
			uint texLoc = glGetUniformLocation(material->GetProgramID(), material->textures[i].var_name.c_str());
			glUniform1i(texLoc, i);
			glActiveTexture(GL_TEXTURE0 + i);
			if (material->textures[i].value == nullptr) glBindTexture(GL_TEXTURE_2D, App->renderer3D->id_checkImage);
			else glBindTexture(GL_TEXTURE_2D, material->textures[i].value->GetTextureID());
		}

	}
	if (material->cube_maps.size() != 0)
	{
		for (uint i = 0; i < material->cube_maps.size(); i++)
		{
			if (material->cube_maps[i].cube_map == nullptr) continue;
			uint texLoc = glGetUniformLocation(material->GetProgramID(), material->cube_maps[i].var_name.c_str());
			glUniform1i(texLoc, i + material->textures.size());
			glActiveTexture(GL_TEXTURE0 + i + material->textures.size());
			if (material->cube_maps[i].cube_map->GetTextureId() <= 0) glBindTexture(GL_TEXTURE_CUBE_MAP, App->renderer3D->id_checkImage);
			else glBindTexture(GL_TEXTURE_CUBE_MAP, material->cube_maps[i].cube_map->GetTextureId());

		}
	}

	material->RestartIterators();
}

void ModuleShaders::SetGlobalVariables(float dt, bool all_lights)
{
	BROFILER_CATEGORY("Update: ModuleShaders SetGlobal Variables", Profiler::Color::Blue);
	std::multimap<uint, Material*>::iterator item = materials.begin();
	static float time_dt = 0;
	time_dt += dt * App->game_time.time_scale;
	uint LastID = 0;

	while (item != materials.end())
	{
		BROFILER_CATEGORY("Update: ModuleShaders Vars", Profiler::Color::Blue);
		uint ID = (*item).second->GetProgramID();
		if ((*item).second->active_num == 0 || ID == LastID) {
			item++;
			continue;
		}

		LastID = ID;
		(*item).second->Bind();

		//TIME		
		GLint timeLoc = glGetUniformLocation(ID, "_time");
		if (timeLoc != -1) glUniform1f(timeLoc, time_dt);

		//CAMERA POSITION
		if (App->renderer3D->active_camera != nullptr) {
			float3 cam_pos = App->renderer3D->active_camera->frustum.pos;
			GLint cameraLoc = glGetUniformLocation(ID, "_cameraPosition");
			if (cameraLoc != -1) glUniform3fv(cameraLoc, 1, &cam_pos[0]);

			float far_plane = App->renderer3D->active_camera->frustum.farPlaneDistance;
			GLint farLoc = glGetUniformLocation(ID, "_farPlane");
			if (farLoc != -1) glUniform1f(farLoc, far_plane);
		}

		//ALPHA
		float alpha = (*item).second->alpha;
		GLint alphaLoc = glGetUniformLocation(ID, "_alpha");
		if (alphaLoc != -1) glUniform1f(alphaLoc, alpha);

		//VIEWPROJ MATRIX
		if (App->renderer3D->active_camera != nullptr) {

			Frustum camFrust = App->renderer3D->active_camera->frustum;
			GLint viewLoc = glGetUniformLocation(ID, "viewproj");
			if (viewLoc != -1)glUniformMatrix4fv(viewLoc, 1, GL_TRUE, camFrust.ViewProjMatrix().ptr());
		}

		//LIGHTS
		std::vector<CompLight*> lights_vec;
		if (!all_lights)
			lights_vec = *App->module_lightning->GetActiveLights();
		else
			lights_vec = App->module_lightning->GetSceneLights();

		GLint lightsizeLoc = glGetUniformLocation(ID, "_numLights");
		if (lightsizeLoc != -1) glUniform1i(lightsizeLoc, lights_vec.size());


		if (lightsizeLoc != -1) {
			BROFILER_CATEGORY("Update: ModuleShaders SetLights", Profiler::Color::Blue);
			for (size_t i = 0; i < lights_vec.size(); ++i) {
				if (lights_vec[i]->to_delete) continue;
				if (lights_vec[i]->type == Light_type::DIRECTIONAL_LIGHT) {
					int depthBiasID = glGetUniformLocation(ID, "depthBias");
					if (depthBiasID != -1) glUniformMatrix4fv(depthBiasID, 1, GL_FALSE, &lights_vec[i]->depthBiasMat[0][0]);
					SetLightUniform(ID, "position", i, lights_vec[i]->GetParent()->GetComponentTransform()->GetEulerToDirection());
				}
					
				if (lights_vec[i]->type == Light_type::POINT_LIGHT)
					SetLightUniform((*item).second->GetProgramID(), "position", i, lights_vec[i]->GetParent()->GetComponentTransform()->GetPosGlobal());

				SetLightUniform(ID, "type", i, (int)lights_vec[i]->type);
				SetLightUniform(ID, "l_color", i, lights_vec[i]->color);
				SetLightUniform(ID, "ambientCoefficient", i, lights_vec[i]->ambientCoefficient);

				SetLightUniform(ID, "properties", i, lights_vec[i]->properties);

				// Bias matrix. TODO: Might be better to set other place

				
				// End Bias matrix.
			}
		}
		item++;
	}
	glUseProgram(0);
}


void ModuleShaders::SendEventWithAllShaders()
{
	Event shader_event;
	shader_event.Set_event_data(EventType::EVENT_SEND_ALL_SHADER_OBJECTS);
	shader_event.send_shader_object.shaders = &shaders;
	PushEvent(shader_event);
	App->gui->shader_program_creation = false;
}