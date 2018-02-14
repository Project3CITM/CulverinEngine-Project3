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
	material_shader = *App->renderer3D->default_shader;

}

CompMaterial::CompMaterial(const CompMaterial& copy, GameObject* parent) : Component(Comp_Type::C_MATERIAL, parent)
{
	uid = App->random->Int();
	color = copy.color;
	material_shader = copy.material_shader;
	//resource_material = copy.resource_material;
	for (int i = 0; i < material_shader.textures.size(); i++) {
		if (material_shader.textures[i].res_material != nullptr)
		{
			material_shader.textures[i].res_material->num_game_objects_use_me++;
		}
	}
	name_component = "Material";
	material_shader = *App->renderer3D->default_shader;
}

CompMaterial::~CompMaterial()
{

	for (int i = 0; i < material_shader.textures.size(); i++) {
		if (material_shader.textures[i].res_material != nullptr)
		{
			if(material_shader.textures[i].res_material->num_game_objects_use_me > 0 )
				material_shader.textures[i].res_material->num_game_objects_use_me--;
		}
		material_shader.textures[i].res_material = nullptr;
	}
	/*
	if (resource_material != nullptr)
	{
		if (resource_material->num_game_objects_use_me > 0)
		{
			resource_material->num_game_objects_use_me--;
		}
	}
	resource_material = nullptr;*/
}

void CompMaterial::PreUpdate(float dt)
{
	for (int i = 0; i < material_shader.textures.size(); i++)
	{
		if (material_shader.textures[i].res_material == nullptr)
		{
			if (material_shader.textures[i].uuid_to_reimport != 0)
			{
				material_shader.textures[i].res_material = (ResourceMaterial*)App->resource_manager->GetResource(material_shader.textures[i].uuid_to_reimport);
				if (material_shader.textures[i].res_material != nullptr)
				{
					material_shader.textures[i].res_material->num_game_objects_use_me++;

					// Check if loaded
					if (material_shader.textures[i].res_material->IsLoadedToMemory() == Resource::State::UNLOADED)
					{
						App->importer->iMaterial->LoadResource(std::to_string(material_shader.textures[i].res_material->GetUUID()).c_str(), material_shader.textures[i].res_material);
					}
					material_shader.textures[i].uuid_to_reimport = 0;

				}
			}
		}
	}
	for (int i = 0; i < material_shader.textures.size(); i++)
	{
		if (material_shader.textures[i].res_material != nullptr)
		{
			if (material_shader.textures[i].res_material->GetState() == Resource::State::WANTDELETE)
			{
				material_shader.textures[i].res_material = nullptr;
			}
			else if (material_shader.textures[i].res_material->GetState() == Resource::State::REIMPORTED)
			{
				material_shader.textures[i].uuid_to_reimport = material_shader.textures[i].res_material->GetUUID();
				material_shader.textures[i].res_material = nullptr;
			}
		}
	}
}

	
















/*





	// Manage Resource -------------------------------------------------
	// Before delete Resource, Set this pointer to nullptr
	if (resource_material != nullptr)
	{
		if (resource_material->GetState() == Resource::State::WANTDELETE)
		{
			resource_material = nullptr;
		}
		else if (resource_material->GetState() == Resource::State::REIMPORTED)
		{
			uuid_resource_reimported = resource_material->GetUUID();
			resource_material = nullptr;
		}
	}
	else
	{

		//TODO change this
		if (uuid_resource_reimported != 0)
		{
			resource_material = (ResourceMaterial*)App->resource_manager->GetResource(uuid_resource_reimported);
			if (resource_material != nullptr)
			{
				resource_material->num_game_objects_use_me++;

				// Check if loaded
				if (resource_material->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(resource_material->GetUUID()).c_str(), resource_material);
				}
				uuid_resource_reimported = 0;
			}
		}
	}
	// -------------------------------------------------
}*/

void CompMaterial::Clear()
{
	for (int i = 0; i < material_shader.textures.size(); i++) {
		material_shader.textures[i].res_material = nullptr;
		
	}
	
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
	/*
	if (resource_material != nullptr)
	{
		return resource_material->GetTextureID();
	}
	*/

	return 0;
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
		for (int i = 0; i < material_shader.textures.size(); i++) {
			TextureVar* texture_var = &material_shader.textures[i];
			if (texture_var->res_material != nullptr)
			{
				if (texture_var->res_material->num_game_objects_use_me > 0)
				{
					texture_var->res_material->num_game_objects_use_me--;
				}
			}
			texture_var->res_material = nullptr;
			
		}
		/*
		if (resource_material != nullptr)
		{
			if (resource_material->num_game_objects_use_me > 0)
			{
				resource_material->num_game_objects_use_me--;
			}
		}*/
		for (int i = 0; i < material_shader.textures.size(); i++) {
			if (material_shader.textures[i].res_material != nullptr)
			{
				if (material_shader.textures[i].res_material->num_game_objects_use_me > 0)
					material_shader.textures[i].res_material->num_game_objects_use_me--;
			}
			material_shader.textures[i].res_material = nullptr;
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
	std::string shaders_names;
	for (int i = 0; i < App->module_shaders->programs.size(); i++) {
		shaders_names += App->module_shaders->programs[i]->name;
		shaders_names += '\0';
	}
	if (ImGui::Combo("Inputs Mode", &shader_pos, shaders_names.c_str())) {
		material_shader = *App->module_shaders->programs[shader_pos];

	}

	if (material_shader.fragment != nullptr) {
		if (ImGui::Button("Edit Fragment"))
		{
			material_shader.edit_fragment = true;
			material_shader.edit_vertex = false;
			material_shader.edit_geometry = false;
			material_shader.get_shader_text = true;
		}
		
	}

	if (material_shader.vertex != nullptr) {
		if (ImGui::Button("Edit Vertex"))
		{
			material_shader.edit_vertex = true;
			material_shader.edit_fragment = false;
			material_shader.edit_geometry = false;
			material_shader.get_shader_text = true;
		}
	}

	if (material_shader.geometry != nullptr) {
		if (ImGui::Button("Edit Geometry"))
		{
			material_shader.edit_geometry = true;
			material_shader.edit_fragment = false;
			material_shader.edit_vertex = false;
			material_shader.get_shader_text = true;
		}
	}

	ShowShadersEditors();

	std::vector<TextureVar>::iterator item = material_shader.textures.begin();
	int i = 0;
	while (item != material_shader.textures.end()) {
		ShowTextureVariable(i);
		i++;
		item++;

	}
	
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

	json_object_dotset_string_with_std(object, name + "ShaderName:", material_shader.name.c_str());

	json_object_dotset_number_with_std(object, name + "Num Textures:", material_shader.textures.size());

	for (int i = 0; i < material_shader.textures.size(); i++)
	{
		char mat_name[128] = { 0 };

		char* num = new char[4];
		itoa(i, num, 10);
		strcat(mat_name, "Resource Material UUID ");
		strcat(mat_name, num);
		RELEASE_ARRAY(num);

		if (material_shader.textures[i].res_material != nullptr && material_shader.textures[i].res_material->GetTextureID() != App->renderer3D->id_checkImage)
		{
			json_object_dotset_number_with_std(object, name + mat_name, material_shader.textures[i].res_material->GetUUID());
		}
		else
		{			
			json_object_dotset_number_with_std(object, name + mat_name, 0);
		}		

	}

	//Save Shaders and shaders values
	//TODO
	//
	/*if (resource_material != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Resource Material UUID", resource_material->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Resource Material UUID", 0);
	}*/
}

void CompMaterial::Load(const JSON_Object* object, std::string name)
{
	float4 tempColor = App->fs->json_array_dotget_float4_string(object, name + "Color");
	color.Set(tempColor.x, tempColor.y, tempColor.z, tempColor.w);
	uid = json_object_dotget_number_with_std(object, name + "UUID");

	
	//uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Material UUID");

	std::string shader_name = json_object_dotget_string_with_std(object, name + "ShaderName:");
	ShaderProgram* temp_shader = nullptr;
	for (int i = 0; i < App->module_shaders->programs.size(); i++)
	{
		if (strcmp(App->module_shaders->programs[i]->name.c_str(), shader_name.c_str() )==0) 
		{
			temp_shader = App->module_shaders->programs[i];
			break;
		}

	}
	if (temp_shader != nullptr)
	{
		material_shader = *temp_shader;
		uint num_textures = json_object_dotget_number_with_std(object, name + "Num Textures:");
		for (int i = 0; i < num_textures; i++) 
		{
			char mat_name[128] = { 0 };

			char* num = new char[4];
			itoa(i, num, 10);
			strcat(mat_name, "Resource Material UUID ");
			strcat(mat_name, num);
			RELEASE_ARRAY(num);

			uint uuid = json_object_dotget_number_with_std(object, name + mat_name);

			material_shader.textures[i].res_material = (ResourceMaterial*)App->resource_manager->GetResource(uuid);
			if (material_shader.textures[i].res_material != nullptr)
			{
				material_shader.textures[i].res_material->num_game_objects_use_me++;

				// LOAD MATERIAL -------------------------
				if (material_shader.textures[i].res_material->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(material_shader.textures[i].res_material->GetUUID()).c_str(), material_shader.textures[i].res_material);
				}
			}
			else {
				material_shader.textures[i].res_material = App->renderer3D->default_mat;
			}
		}
	}
	else
		material_shader = *App->renderer3D->default_shader;

	//Load Shaders and shaders values
	//TODO
	//
	/*
	if (resourceID > 0)
	{
		resource_material = (ResourceMaterial*)App->resource_manager->GetResource(resourceID);
		if (resource_material != nullptr)
		{
			resource_material->num_game_objects_use_me++;

			// LOAD MATERIAL -------------------------
			if (resource_material->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMaterial->LoadResource(std::to_string(resource_material->GetUUID()).c_str(), resource_material);
			}
		}
	}*/
	Enable();
}



void CompMaterial::ShowTextureVariable(int index)
{
	ImGui::PushID(index);

	TextureVar* texture_var = &material_shader.textures[index];

	/* Name of the material */
	ImGui::Text("Name:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", texture_var->var_name.c_str());

	if (texture_var->res_material != nullptr)
	{
		/* Image of the texture */
		ImGui::Image((ImTextureID*)texture_var->res_material->GetTextureID(), ImVec2(64, 64), ImVec2(-1, 1), ImVec2(0, 0));
	}

	else{

		if (ImGui::Button("Select Material..."))
		{
			texture_var->selected = true;
		}
	}
	if (texture_var->selected)
	{
		ResourceMaterial* temp = (ResourceMaterial*)App->resource_manager->ShowResources(texture_var->selected, Resource::Type::MATERIAL);
		if (temp != nullptr)
		{
			if (texture_var->res_material != nullptr)
			{
				if (texture_var->res_material->num_game_objects_use_me > 0)
				{
					texture_var->res_material->num_game_objects_use_me--;
				}
			}
			texture_var->res_material = temp;
			texture_var->res_material->num_game_objects_use_me++;
			if (texture_var->res_material->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMaterial->LoadResource(std::to_string(texture_var->res_material->GetUUID()).c_str(), texture_var->res_material);
			}
			Enable();
		}
	}
	ImGui::PopID();
}

void CompMaterial::ShowShadersEditors()
{
	Shader* shader = nullptr;
	bool* edit_shader = nullptr;
	ShaderType shader_type;

	if (material_shader.edit_fragment) {
		shader = material_shader.fragment;
		edit_shader = &material_shader.edit_fragment;
		shader_type = ShaderType::fragment;
	}
	else if (material_shader.edit_vertex) {
		shader = material_shader.vertex;
		edit_shader = &material_shader.edit_vertex;
		shader_type = ShaderType::vertex;
	}
	else if (material_shader.edit_vertex) {
		shader = material_shader.vertex;
		edit_shader = &material_shader.edit_geometry;
		shader_type = ShaderType::geometry;
	}

	if (shader == nullptr) return;


		if (ImGui::Begin("Edit sHADER", edit_shader)) {

			if (ImGui::Button("Save") && material_shader.programID != App->renderer3D->default_shader->programID) {

				uint buffer_size = strlen(material_shader.shader_editor.GetText().c_str());

				FILE* pFile = fopen(shader->shaderPath.c_str(), "wb");
				fwrite(material_shader.shader_editor.GetText().c_str(), sizeof(char), buffer_size, pFile);
				fclose(pFile);

				uint last_shader_id = shader->shaderID;

				Shader* new_shader = App->module_shaders->CompileShader(shader->shaderPath, shader->name,shader_type);

				switch(shader_type){
				case ShaderType::fragment:
							material_shader.UpdateShaderProgram(0, new_shader->shaderID, 0);
							break;
				case ShaderType::vertex:
					material_shader.UpdateShaderProgram(new_shader->shaderID,0,0);
					break;

				case ShaderType::geometry:
					material_shader.UpdateShaderProgram(0,0,new_shader->shaderID);
					break;
					
				}

				glDeleteShader(last_shader_id);
				
			}

			if (material_shader.get_shader_text)
			{
				if (strcmp(shader->shaderPath.c_str(), "") != 0) {
					char* buffer;
					buffer = shader->GetShaderText();

					material_shader.shader_editor.SetText(buffer);
					
				}
				else {
					material_shader.shader_editor.SetText("YOU CAN'T MODIFY THE DEFAULT SHADER");
				}
				material_shader.get_shader_text = false;
		}

			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[1]);
			material_shader.shader_editor.Render("Editor");
			ImGui::PopFont();

			ImGui::End();
		}


	
}


