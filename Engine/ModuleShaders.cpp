#include "ModuleShaders.h"
#include "ShadersLib.h"
#include "ModuleFS.h"
#include "Application.h"
#include "JSONSerialization.h"
#include "ModuleEventSystem.h"

ModuleShaders::ModuleShaders()
{
}

ModuleShaders::~ModuleShaders()
{
}

bool ModuleShaders::Init(JSON_Object * node)
{
	Shader_Directory_fs = "Assets/Shaders";

	return true;
}

bool ModuleShaders::Start()
{

	//Import all the asset files related to shaders

	ImportShaderObjects();

	ImportShaderMaterials();

	/*ShaderProgram* test = new ShaderProgram();
	test->AddFragment(shaders[0]);
	test->AddVertex(shaders[1]);
	test->LoadProgram();
	test->name = "Test_Material";
	test->path = Shader_Directory_fs+"/Test_Material.mat";
	test->CreateMaterialFile();*/

	Light new_light;
	new_light.position = float3(1, 1, 0);
	lights.push_back(new_light);

	return true;
}

update_status ModuleShaders::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleShaders::Update(float dt)
{
	std::vector<ShaderProgram*>::iterator item = programs.begin();
	static float time_dt = 0;
	time_dt += dt * App->game_time.time_scale;
	while (item != programs.end())
	{
		(*item)->Bind();

		//TIME		
		GLint timeLoc = glGetUniformLocation((*item)->programID, "_time");
		glUniform1f(timeLoc, time_dt);

		//LIGHTS
		GLint lightsizeLoc = glGetUniformLocation((*item)->programID, "_numLights");
		glUniform1i(lightsizeLoc, lights.size());
		for (size_t i = 0; i < lights.size(); ++i) {
			SetLightUniform((*item)->programID, "position", i, lights[i].position);
			SetLightUniform((*item)->programID, "intensities", i, lights[i].color);
			SetLightUniform((*item)->programID, "attenuation", i, lights[i].attenuation);
			SetLightUniform((*item)->programID, "ambientCoefficient", i, lights[i].ambientCoefficient);
	
		}
		(*item)->Unbind();
		item++;
	}

	Enable_Text_Editor();
	
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

ShaderProgram * ModuleShaders::CreateDefaultShader()
{
	ShaderProgram* defaultShader = new ShaderProgram();
	defaultShader->name = "Default Shader";
	//Success flag
	GLint programSuccess = GL_TRUE;

	//Generate program
	defaultShader->programID = glCreateProgram();

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
		"out float ourTime;\n"
		"out vec4 ourColor;\n"
		"out vec3 ourNormal;\n"
		"out vec2 TexCoord;\n"
		"uniform float _time;\n"
		"uniform vec4 _color;\n"
		"uniform mat4 model;\n"
		"uniform mat4 viewproj;\n"
		"uniform mat4 view;\n"
		"void main()\n"
		"{\n"
		"gl_Position = viewproj *  model * vec4(position.x,position.y,position.z, 1.0f);\n"
		"ourColor = _color;\n"
		"TexCoord = texCoord;\n"
		"ourTime = _time;\n"
		"ourNormal = mat3(model) * normal;"
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
		return nullptr;
	}

	//Attach vertex shader to program
	glAttachShader(defaultShader->programID, vertexShader);

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Get fragment source
	const GLchar* fragmentShaderSource[] =
	{
		"#version 330 core\n"
		"in vec4 ourColor;\n"
		"in float ourTime;\n"
		"in vec2 TexCoord;\n"
		"in vec3 ourNormal;\n"
		"in vec4 gl_FragCoord;\n"
		"out vec4 color;\n"
		"uniform sampler2D albedo;\n"
		
		"void main()\n"
		"{\n"
		"vec3 lightDir = vec3(1);\n"
		"float angle = dot(lightDir, ourNormal);\n"
		
		//Z-Buffer Line Shader
		"color= vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1) *texture(albedo, TexCoord);\n"
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
		return nullptr;
	}

	//Attach fragment shader to program
	glAttachShader(defaultShader->programID, fragmentShader);

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
	newFragment->shaderText = *fragmentShaderSource;
	newFragment->shaderType = ShaderType::fragment;
	newFragment->name = "default_shader_frag";
	newFragment->shaderPath = "";

	defaultShader->AddFragment(newFragment);

	Shader* newVertex = new Shader();
	newVertex->shaderID = vertexShader;
	newVertex->shaderText = *vertexShaderSource;
	newVertex->shaderType = ShaderType::vertex;
	newFragment->name = "default_shader_vert";
	newVertex->shaderPath = "";

	defaultShader->AddVertex(newVertex);

	uint var_size = defaultShader->GetVariablesSize();
	for (int i = 0; i < var_size; i++) {
		UniformVar temp = defaultShader->GetVariableInfo(i);

		//Textures
		if (temp.type == 35678) {
			TextureVar texture_var;
			texture_var.var_name = temp.name;
			defaultShader->textures.push_back(texture_var);
		}

	}

	programs.push_back(defaultShader);
	shaders.push_back(newFragment);
	shaders.push_back(newVertex);

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
			char* buffer;
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

		}
	}
}

void ModuleShaders::ImportShaderMaterials()
{

	namespace stdfs = std::experimental::filesystem;
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
				char* buffer;
				App->fs->LoadFile(str_path.c_str(), &buffer, DIRECTORY_IMPORT::IMPORT_DEFAULT);

				if (buffer != nullptr)
				{
					JSON_Object* obj_proj;
					JSON_Value* file_proj;
					//Point JSON_Object and JSON_Value to the path we want
					App->json_seria->Create_Json_Doc(&file_proj, &obj_proj, str_path.c_str());

					//Creating the shader program
					ShaderProgram* mat_shader = CreateShader(name.c_str());

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
				}
			}
	}
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
	return true;
}

void ModuleShaders::OnEvent(Event & event)
{
	switch (event.type)
	{
	case EventType::EVENT_OPEN_SHADER_EDITOR:
		JSON_Object* obj_proj;
		JSON_Value* file_proj;
		std::string path= Shader_Directory_fs+ "/" + event.shadereditor.name;

		switch (event.shadereditor.shader_type)
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
		shader_text_active.name = event.shadereditor.name;
		shader_text_active.shaderPath = path;
		shader_text_active.shaderType = event.shadereditor.shader_type;
		enable_editor = event.shadereditor.open_editor;
		
		
		
		
		//----------------------
		/*App->json_seria->Create_Json_Doc(&file_proj, &obj_proj, path.c_str());

		char* serialized_string = "#version 330 core\n void main(){\n }";
		json_serialize_to_file(file_proj, path.c_str());
		Shader* new_shader = CompileShader(path, event.shader.name, event.shader.shader_type);
		*/
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
			editor_shaders.Render(temp_name.c_str());
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