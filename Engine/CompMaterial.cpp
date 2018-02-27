#include "CompMaterial.h"
#include "Color.h"
#include "GAmeObject.h"
#include "parson.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ImportMaterial.h"
#include "ResourceMaterial.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "ModuleRenderer3D.h"
#include "Scene.h"


CompMaterial::CompMaterial(Comp_Type t, GameObject* parent): Component(t, parent)
{
	uid = App->random->Int();
	color = White;
	name_component = "Material";
	material = App->renderer3D->default_material;

}

CompMaterial::CompMaterial(const CompMaterial& copy, GameObject* parent) : Component(Comp_Type::C_MATERIAL, parent)
{
	uid = App->random->Int();
	color = copy.color;
	material = copy.material;
	//resource_material = copy.resource_material;
	for (int i = 0; i < material->textures.size(); i++) {
		if (material->textures[i].value != nullptr)
		{
			material->textures[i].value->num_game_objects_use_me++;
		}
	}
	name_component = "Material";
	if(material == nullptr)
	material = App->renderer3D->default_material;
}

CompMaterial::~CompMaterial()
{
	
}

void CompMaterial::PreUpdate(float dt)
{
	for (int i = 0; i < material->textures.size(); i++)
	{
		if (material->textures[i].value == nullptr)
		{
			if (material->textures[i].uuid_to_reimport != 0)
			{
				material->textures[i].value = (ResourceMaterial*)App->resource_manager->GetResource(material->textures[i].uuid_to_reimport);
				if (material->textures[i].value != nullptr)
				{
					material->textures[i].value->num_game_objects_use_me++;

					// Check if loaded
					if (material->textures[i].value->IsLoadedToMemory() == Resource::State::UNLOADED)
					{
						App->importer->iMaterial->LoadResource(std::to_string(material->textures[i].value->GetUUID()).c_str(), material->textures[i].value);
					}
					material->textures[i].uuid_to_reimport = 0;

				}
			}
		}
	}
	for (int i = 0; i < material->textures.size(); i++)
	{
		if (material->textures[i].value != nullptr)
		{
			if (material->textures[i].value->GetState() == Resource::State::WANTDELETE)
			{
				material->textures[i].value = nullptr;
			}
			else if (material->textures[i].value->GetState() == Resource::State::REIMPORTED)
			{
				material->textures[i].uuid_to_reimport = material->textures[i].value->GetUUID();
				material->textures[i].value = nullptr;
			}
		}
	}
}

void CompMaterial::Clear()
{	

	
}

void CompMaterial::SetColor(float r, float g, float b, float a)
{
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}

Color CompMaterial::GetColor() const
{
	return color;
}

uint CompMaterial::GetTextureID() const
{
	
	return 0;
}

ShaderProgram * CompMaterial::GetShaderProgram()const
{
	return material->material_shader;
}

void CompMaterial::SetUUIDMesh(uint uuid)
{
	uuid_material = uuid;
}

void CompMaterial::ShowOptions()
{
	if (ImGui::MenuItem("Reset", NULL, false, false))
	{
		// Not implmented yet.
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component"))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_CAMERA)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompMaterial*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Reset Material"))
	{
		for (int i = 0; i < material->textures.size(); i++) {
			TextureVar* texture_var = &material->textures[i];
			if (texture_var->value != nullptr)
			{
				if (texture_var->value->num_game_objects_use_me > 0)
				{
					texture_var->value->num_game_objects_use_me--;
				}
			}
			texture_var->value = nullptr;
			
		}
		
		for (int i = 0; i < material->textures.size(); i++) {
			if (material->textures[i].value != nullptr)
			{
				if (material->textures[i].value->num_game_objects_use_me > 0)
					material->textures[i].value->num_game_objects_use_me--;
			}
			material->textures[i].value = nullptr;
		}
		
		ImGui::CloseCurrentPopup();
	}
	/* Select Material */
	if (ImGui::MenuItem("Select Material..."))
	{
		select_material = true;
		ImGui::CloseCurrentPopup();
	}
}

void CompMaterial::ShowInspectorInfo()
{
	// Reset Values Button -------------------------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsMaterial");
	}

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsMaterial"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
	ImGui::ColorEdit3("", (float*)&color);
	

	int shader_pos = 0;
	int program_pos = 0;
	std::string shaders_names;
	std::string program_names;
	for (int i = 0; i < App->module_shaders->materials.size(); i++) {
		shaders_names += App->module_shaders->materials[i]->name;
		shaders_names += '\0';
		if (material->name.compare(App->module_shaders->materials[i]->name) == 0)
			shader_pos = i;
	}
	for (int i = 0; i < App->module_shaders->programs.size(); i++) {
		program_names += App->module_shaders->programs[i]->name;
		program_names += '\0';
		if (material->material_shader->name.compare(App->module_shaders->programs[i]->name) == 0)
			program_pos = i;
	}
	if (ImGui::Combo("Inputs Mode", &shader_pos, shaders_names.c_str())) {

		
		ResourceMaterial* resource_mat = (ResourceMaterial*)App->resource_manager->GetResource(App->module_shaders->materials[shader_pos]->path.c_str());
		if (resource_mat != nullptr)
		{
			resource_mat->num_game_objects_use_me++;
			if (resource_mat->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				std::string temp = std::to_string(resource_mat->GetUUID());
				App->importer->iMaterial->LoadResource(temp.c_str(), resource_mat);
			}
		}
		material = App->module_shaders->materials[shader_pos];
	}

	if (ImGui::Combo("Programs", &program_pos, program_names.c_str())) {
		
		material->material_shader = App->module_shaders->programs[program_pos];
		material->GetProgramVariables();

	}

	if (GetShaderProgram()->fragment != nullptr) {
		if (ImGui::Button("Edit Fragment"))
		{
			GetShaderProgram()->edit_fragment = true;
			GetShaderProgram()->edit_vertex = false;
			GetShaderProgram()->edit_geometry = false;
			GetShaderProgram()->get_shader_text = true;
		}
		
	}

	if (GetShaderProgram()->vertex != nullptr) {
		if (ImGui::Button("Edit Vertex"))
		{
			GetShaderProgram()->edit_vertex = true;
			GetShaderProgram()->edit_fragment = false;
			GetShaderProgram()->edit_geometry = false;
			GetShaderProgram()->get_shader_text = true;
		}
	}

	if (GetShaderProgram()->geometry != nullptr) {
		if (ImGui::Button("Edit Geometry"))
		{
			GetShaderProgram()->edit_geometry = true;
			GetShaderProgram()->edit_fragment = false;
			GetShaderProgram()->edit_vertex = false;
			GetShaderProgram()->get_shader_text = true;
		}
	}

	ShowShadersEditors();

	material->GetProgramVariables();
	material->RestartIterators();
	

	uint var_size = material->GetVariablesSize();
	for (int i = 0; i < var_size; i++)
	{
		UniformVar temp = material->GetVariableInfo(i);

		// Variables started with '_' reserved for global variables
		if (temp.name != nullptr && temp.name[0] == '_') continue;

		//Textures
		if (temp.type == GL_SAMPLER_2D &&material->textures.size() != 0)
		{
			ShowTextureVariable(i, &(*material->it_textures));
			material->it_textures++;			
		}

		//Vec3
		if (temp.type == GL_FLOAT_VEC3 && material->int_variables.size() != 0)
		{
			ShowVec3Variable(i, &(*material->it_float3_variables));
			material->it_float3_variables++;
		}

		//Int
		if (temp.type == GL_INT && material->int_variables.size() != 0)
		{
			ShowIntVariable(i, &(*material->it_int_variables));
			material->it_int_variables++;
		}

		//Float
		if (temp.type == GL_FLOAT && material->float_variables.size() != 0)
		{
			ShowFloatVariable(i, &(*material->it_float_variables));
			material->it_float_variables++;
		}

		//Bool
		if (temp.type == GL_BOOL && material->bool_variables.size() != 0)
		{
			ShowBoolVariable(i, &(*material->it_bool_variables));
			material->it_bool_variables++;
		}


		//Color
		if (temp.type == GL_FLOAT_VEC4 && material->color_variables.size() != 0)
		{
			ShowColorVariable(i, &(*material->it_color_variables));
			material->it_color_variables++;
		}

	}

	material->RestartIterators();


	ImGui::TreePop();
}

void CompMaterial::CopyValues(const CompMaterial* component)
{
	//more...
}


void CompMaterial::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", C_MATERIAL);
	float4 tempColor = { color.r, color.g, color.b, color.a };
	App->fs->json_array_dotset_float4(object, name + "Color", tempColor);
	json_object_dotset_number_with_std(object, name + "UUID", uid);

	json_object_dotset_string_with_std(object, name + "MaterialName:", material->name.c_str());


	
}

void CompMaterial::Load(const JSON_Object* object, std::string name)
{
	float4 tempColor = App->fs->json_array_dotget_float4_string(object, name + "Color");
	color.Set(tempColor.x, tempColor.y, tempColor.z, tempColor.w);
	uid = json_object_dotget_number_with_std(object, name + "UUID");

	
	//uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Material UUID");

	std::string material_name = json_object_dotget_string_with_std(object, name + "MaterialName:");
	
	for (int i = 0; i < App->module_shaders->materials.size(); i++)
	{
		if (strcmp(App->module_shaders->materials[i]->name.c_str(), material_name.c_str() )==0)
		{
			material = App->module_shaders->materials[i];
			break;
		}

	}
	if (material == nullptr)
	{
		material = App->renderer3D->default_material;

	}
	else {
		for (int i = 0; i < material->textures.size(); i++)
		{
			if (material->textures[i].value != nullptr)
			{
				material->textures[i].value->num_game_objects_use_me++;

				// LOAD MATERIAL -------------------------
				if (material->textures[i].value->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(material->textures[i].value->GetUUID()).c_str(), material->textures[i].value);
				}
			}
		}
	}

	Enable();
}



void CompMaterial::ShowTextureVariable(int index, TextureVar* texture)
{
	ImGui::PushID(index);

	
	static bool open = false;
	/* Name of the material */
	ImGui::Text("Name:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", texture->var_name.c_str());

	if (texture->value != nullptr)
	{
		/* Image of the texture */
		if (ImGui::ImageButton((ImTextureID*)texture->value->GetTextureID(), ImVec2(64, 64), ImVec2(-1, 1), ImVec2(0, 0))) {
			texture->selected = true;

		}
	}

	else{

		if (ImGui::Button("Select Material..."))
		{
			texture->selected = true;
		}
	}
	if (texture->selected)
	{
		ResourceMaterial* temp = (ResourceMaterial*)App->resource_manager->ShowResources(texture->selected, Resource::Type::MATERIAL);
		if (temp != nullptr)
		{
			if (texture->value != nullptr)
			{
				if (texture->value->num_game_objects_use_me > 0)
				{
					texture->value->num_game_objects_use_me--;
				}
			}
			texture->value = temp;
			texture->value->num_game_objects_use_me++;
			if (texture->value->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMaterial->LoadResource(std::to_string(texture->value->GetUUID()).c_str(), texture->value);
			}
			Enable();
		}
	}
	ImGui::PopID();
}

void CompMaterial::ShowIntVariable(int index, intVar * var)
{
	ImGui::PushID(index);
	if(ImGui::InputInt(var->var_name.c_str(),&var->value))
		material->Save();
	ImGui::PopID();

}

void CompMaterial::ShowFloatVariable(int index, floatVar * var)
{
	ImGui::PushID(index);	
	if(ImGui::InputFloat(var->var_name.c_str(), &var->value, 0.01f,0.1f,3))
		material->Save();
	ImGui::PopID();

}

void CompMaterial::ShowVec3Variable(int index, float3Var * var)
{
	ImGui::PushID(index);
	if (ImGui::InputFloat3(var->var_name.c_str(), &var->value[0]))
		material->Save();
	ImGui::PopID();

}

void CompMaterial::ShowBoolVariable(int index, boolVar * var)
{
	ImGui::PushID(index);
	if(ImGui::Checkbox(var->var_name.c_str(), &var->value))
		material->Save();
	ImGui::PopID();

}

void CompMaterial::ShowColorVariable(int index, ColorVar *var)
{
	ImGui::PushID(index);
	if(ImGui::ColorPicker4(var->var_name.c_str(), &var->value[0]))
		material->Save();
	ImGui::PopID();

}

void CompMaterial::ShowShadersEditors()
{
	Shader* shader = nullptr;
	bool* edit_shader = nullptr;
	ShaderType shader_type;

	if (GetShaderProgram()->edit_fragment) {
		shader = GetShaderProgram()->fragment;
		edit_shader = &GetShaderProgram()->edit_fragment;
		shader_type = ShaderType::fragment;
	}
	else if (GetShaderProgram()->edit_vertex) {
		shader = GetShaderProgram()->vertex;
		edit_shader = &GetShaderProgram()->edit_vertex;
		shader_type = ShaderType::vertex;
	}
	else if (GetShaderProgram()->edit_vertex) {
		shader = GetShaderProgram()->vertex;
		edit_shader = &GetShaderProgram()->edit_geometry;
		shader_type = ShaderType::geometry;
	}

	if (shader == nullptr) return;


		if (ImGui::Begin("Edit shader", edit_shader)) {
			Shader* new_shader = nullptr;
			
			if (ImGui::Button("Save") && material->GetProgramID()) {


				char* last_text = App->module_shaders->GetShaderText(shader->shaderPath);

				uint buffer_size = strlen(GetShaderProgram()->shader_editor.GetText().c_str());

				FILE* pFile = fopen(shader->shaderPath.c_str(), "wb");				
				fwrite(GetShaderProgram()->shader_editor.GetText().c_str(), sizeof(char), buffer_size, pFile);
				fclose(pFile);

				uint last_shader_id = shader->shaderID;

				new_shader = App->module_shaders->CompileShader(shader->shaderPath, shader->name,shader_type);

				if (new_shader != nullptr) {
					
					switch (shader_type) {
					case ShaderType::fragment:
						cant_comiple_shader = !GetShaderProgram()->UpdateShaderProgram(0, new_shader->shaderID, 0);
						break;
					case ShaderType::vertex:
						cant_comiple_shader = !GetShaderProgram()->UpdateShaderProgram(new_shader->shaderID, 0, 0);
						break;

					case ShaderType::geometry:
						cant_comiple_shader = !GetShaderProgram()->UpdateShaderProgram(0, 0, new_shader->shaderID);
						break;

					}

					glDeleteShader(last_shader_id);
				}
				else {
					cant_comiple_shader = true;

					uint last_buffer_size = strlen(last_text);
					FILE* last_pFile = fopen(shader->shaderPath.c_str(), "wb");
					fwrite(last_text, sizeof(char), last_buffer_size, last_pFile);
					fclose(pFile);

					//material_shader.shader_editor.SetText(last_text);

					delete[] last_text;
				}
				
			}

			if (cant_comiple_shader) {
				if (ImGui::Begin("ERROR", &cant_comiple_shader)) {

					std::string program_error = GetShaderProgram()->LogProgramLastError();
					ImGui::Text("ERROR COMPILING SHADER:\n");
					if (App->module_shaders->last_shader_error != "") {
						ImGui::Text("SHADER ERROR: %s\n", App->module_shaders->last_shader_error.c_str());
					}
					if (program_error != "") {
						ImGui::Text("PROGRAM ERROR: %s\n",program_error.c_str());
					}
					
					ImGui::End();
					
				}
				
			}

			if (GetShaderProgram()->get_shader_text)
			{
				if (strcmp(shader->shaderPath.c_str(), "") != 0) {
					char* buffer;
					buffer = shader->GetShaderText();

					GetShaderProgram()->shader_editor.SetText(buffer);
					delete[] buffer;
					
				}
				else {
					GetShaderProgram()->shader_editor.SetText("YOU CAN'T MODIFY THE DEFAULT SHADER");
				}
				GetShaderProgram()->get_shader_text = false;
		}

			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[1]);
			GetShaderProgram()->shader_editor.Render("Editor");
			ImGui::PopFont();

			ImGui::End();
		}


	
}


