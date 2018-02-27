#include "CompParticleSystem.h"
#include "Application.h"
#include "Scene.h"
#include "ParticleSystem.h"
#include "ResourceMaterial.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "GameObject.h"
#include "ModuleFS.h"
#include "ModuleResourceManager.h"
#include "ImportMaterial.h"
#include "CompTransform.h"

CompParticleSystem::CompParticleSystem(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	name_component = "Particle_System";
	part_system = new ParticleSystem();
	uid = App->random->Int();
}

CompParticleSystem::CompParticleSystem(const CompParticleSystem& copy, GameObject* parent) : Component(copy.GetType(), parent)
{
}

CompParticleSystem::~CompParticleSystem()
{

}


void CompParticleSystem::PreUpdate(float dt)
{
	const CompCamera* camera = App->renderer3D->GetActiveCamera();
	if (camera != nullptr)
		part_system->SetCameraPosToFollow(camera->frustum.pos);

	part_system->PreUpdate(dt);
}

void CompParticleSystem::Update(float dt)
{
	if (pop_up_load_open) ImGuiLoadPopUp();
	if (pop_up_save_open) ImGuiSavePopUp();

	if (!child_loaded && part_system->IsEmitterDead() && !child_particle.empty() && !child_emitter.empty())
	{
		//Load Child Particle
		ParticleState InitialState;
		ParticleState FinalState;

		size_t dot_pos = child_particle.rfind(".");
		child_particle_name = child_particle.substr(0, dot_pos);

		LoadParticleStates(child_particle_name.c_str(),this, InitialState, FinalState);
	

		part_system->SetInitialStateResource(InitialState);
		part_system->SetFinalStateResource(FinalState);

		//Load Child Emitter
		ParticleEmitter Emitter;

		dot_pos = child_emitter.rfind(".");
		child_emitter_name = child_emitter.substr(0, dot_pos);

		LoadParticleEmitter(child_emitter_name.c_str(), this, Emitter);
	

		part_system->SetEmitterResource(Emitter);

		child_loaded = false;
	}

	part_system->SetEmitterTransform(parent->GetComponentTransform()->GetGlobalTransform().Transposed());
	part_system->Update(dt);
	if (part_system->EditorWindowOpen)
		part_system->DrawImGuiEditorWindow();
	
	part_system->PostUpdate(dt);
}

void CompParticleSystem::Clear()
{
	part_system->CleanUp();
	RELEASE(part_system);
}

void CompParticleSystem::SetTextureResource(uint uuid, int columns, int rows, int numberOfFrames, uint AnimationOrder)
{
	if (texture_resource != nullptr && texture_resource->num_game_objects_use_me > 0)
	{
		texture_resource->num_game_objects_use_me--;
	}		
	
	texture_resource = (ResourceMaterial*)App->resource_manager->GetResource(uuid);//(texture_resource*)App->resources->Get(uuid);
	if (texture_resource != nullptr)
		part_system->SetTextureResource(texture_resource->GetTextureID(), texture_resource->GetTextureWidth(), texture_resource->GetTextureHeight(), columns, rows, numberOfFrames, AnimationOrder);
}

void CompParticleSystem::SetTextureResource(const char * Path, int columns, int rows, int numberOfFrames, uint AnimationOrder)
{
	file_to_load = Path;
	size_t bar_pos = file_to_load.rfind("\\") + 1;
	size_t dot_pos = file_to_load.rfind(".");
	file_to_load_name = file_to_load.substr(bar_pos, dot_pos - bar_pos);
//	uint Texuuid = App->resources->LoadResource((*App->importer->Get_Library_material_path() + "\\" + FileToLoadName + ".dds").c_str(), FileToLoad.c_str());

	uint text_id = App->importer->iMaterial->LoadResource(Path, texture_resource);
	
	SetTextureResource(texture_resource->GetTextureID(), columns, rows, numberOfFrames, AnimationOrder);
}

const std::string* CompParticleSystem::GetChildParticle() const
{
	if (!child_particle.empty()) return &child_particle;
	return nullptr;
}

const std::string* CompParticleSystem::GetChildEmitter() const
{
	if (!child_emitter.empty()) return &child_emitter;
	return nullptr;
}


// LOAD / SAVE ---------------------------------------------------------

void CompParticleSystem::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{

}

void CompParticleSystem::Load(const JSON_Object* object, std::string name)
{

}

bool CompParticleSystem::SaveParticleStates(const char* file_name, ResourceMaterial* TextureResource, const ParticleTextureData* TexData, const ParticleState* stateI, const ParticleState* stateF) const
{
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	std::string file_path = App->fs->GetFullPath("Assets\\ParticleSystem\\Particles");
	file_path += "\\";
	file_path += file_name;

	root_value = json_parse_file(file_path.c_str());
	if (root_value == NULL)
		root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);

	JSON_Object* file_conf = nullptr;

	//If this entry does not exist, create it
	if (json_object_get_object(root_object, "particlestate") == NULL)
		json_object_set_value(root_object, "particlestate", json_value_init_object());
	//Save
	file_conf = json_object_get_object(root_object, "particlestate");

	const ParticleState* state = nullptr;
	JSON_Object* conf = nullptr;

	if (TextureResource != nullptr)
	{
		json_object_set_value(file_conf, "texture", json_value_init_object());
		conf = json_object_get_object(file_conf, "texture");
		SetString(conf, "texture_path", TextureResource->GetTextureName());
		SetInt(conf, "columns", TexData->columns);
		SetInt(conf, "rows", TexData->rows);
		SetInt(conf, "numberOfFrames", TexData->numberOfFrames);
		SetUInt(conf, "AnimationOrder", TexData->AnimationOrder);
	}

	for (uint i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			state = stateI;
			json_object_set_value(file_conf, "initial_state", json_value_init_object());
			conf = json_object_get_object(file_conf, "initial_state");
		}
		else
		{
			state = stateF;
			json_object_set_value(file_conf, "final_state", json_value_init_object());
			conf = json_object_get_object(file_conf, "final_state");
		}

		SetFloat3(conf, "force", state->force);
		SetFloat3(conf, "forceVariation", state->forceVariation);
		SetFloat(conf, "Size", state->Size);
		SetFloat(conf, "SizeVariation", state->SizeVariation);
		SetFloat4(conf, "RGBATint", state->RGBATint);
		SetFloat4(conf, "RGBATintVariation", state->RGBATintVariation);
		SetBool(conf, "alpha_preview", state->alpha_preview);
		SetBool(conf, "alpha_half_preview", state->alpha_half_preview);
		SetBool(conf, "options_menu", state->options_menu);
		SetBool(conf, "alpha", state->alpha);
		SetBool(conf, "alpha_bar", state->alpha_bar);
		SetBool(conf, "side_preview", state->side_preview);
		SetUInt(conf, "inputs_mode", state->inputs_mode);
		SetUInt(conf, "picker_mode", state->picker_mode);
	}

	json_serialize_to_file(root_value, file_path.c_str());
	json_value_free(root_value);
	return true;
}

bool CompParticleSystem::LoadParticleStates(const char* file_name, CompParticleSystem* system, ParticleState& stateI, ParticleState& stateF) const
{
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	std::string file_path = App->fs->GetFullPath("Assets\\ParticleSystem\\Particles");
	file_path += "\\";
	file_path += file_name;
	root_value = json_parse_file(file_path.c_str());
	if (root_value == NULL)
		root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);

	JSON_Object* file_conf = nullptr;
	file_conf = json_object_get_object(root_object, "particlestate");

	ParticleState* State = nullptr;
	JSON_Object* conf = nullptr;

	conf = json_object_get_object(file_conf, "texture");
	if (conf != nullptr)
	{
		const char* Path = GetString(conf, "texture_path");
		int columns = GetInt(conf, "columns");
		int rows = GetInt(conf, "rows");
		int numberOfFrames = GetInt(conf, "numberOfFrames");
		uint AnimationOrder = GetUInt(conf, "AnimationOrder");
		system->SetTextureResource(Path, columns, rows, numberOfFrames, AnimationOrder);
	}

	for (uint i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			State = &stateI;
			conf = json_object_get_object(file_conf, "initial_state");
		}
		else
		{
			State = &stateF;
			conf = json_object_get_object(file_conf, "final_state");
		}

		State->force = GetFloat3(conf, "force");
		State->forceVariation = GetFloat3(conf, "forceVariation");
		State->Size = GetFloat(conf, "Size");
		State->SizeVariation = GetFloat(conf, "SizeVariation");
		State->RGBATint = GetFloat4(conf, "RGBATint");
		State->RGBATintVariation = GetFloat4(conf, "RGBATintVariation");
		State->alpha_preview = GetBool(conf, "alpha_preview");
		State->alpha_half_preview = GetBool(conf, "alpha_half_preview");
		State->options_menu = GetBool(conf, "options_menu");
		State->alpha = GetBool(conf, "alpha");
		State->alpha_bar = GetBool(conf, "alpha_bar");
		State->side_preview = GetBool(conf, "side_preview");
		State->inputs_mode = GetUInt(conf, "inputs_mode");
		State->picker_mode = GetUInt(conf, "picker_mode");
	}

	json_value_free(root_value);
	return true;
}

bool CompParticleSystem::SaveParticleEmitter(const char* file_name, CompParticleSystem* system, const ParticleEmitter* emitter) const
{
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	std::string file_path = App->fs->GetFullPath("Assets\\ParticleSystem\\Emitters");
	file_path += "\\";
	file_path += file_name;

	root_value = json_parse_file(file_path.c_str());
	if (root_value == NULL)
		root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
	JSON_Object* conf = nullptr;

	//If this entry does not exist, create it
	if (json_object_get_object(root_object, "emitter") == NULL)
		json_object_set_value(root_object, "emitter", json_value_init_object());
	//Save
	conf = json_object_get_object(root_object, "emitter");

	bool ShowEmitterBoundBox = false;
	bool ShowEmitter = false;
	system->GetDebugOptions(ShowEmitterBoundBox, ShowEmitter);
	SetBool(conf, "ShowEmitterBoundBox", ShowEmitterBoundBox);
	SetBool(conf, "ShowEmitter", ShowEmitter);

	if (system->GetChildParticle() != nullptr)
		SetString(conf, "ChildParticle", system->GetChildParticle()->c_str());
	if (system->GetChildEmitter() != nullptr)
		SetString(conf, "ChildEmitter", system->GetChildEmitter()->c_str());

	SetFloat(conf, "EmitterLifeMax", emitter->EmitterLifeMax);
	SetFloat4x4(conf, "Transform", emitter->Transform);
	SetUInt(conf, "SpawnRate", emitter->SpawnRate);
	SetFloat(conf, "Lifetime", emitter->Lifetime);
	SetFloat(conf, "LifetimeVariation", emitter->LifetimeVariation);
	SetFloat(conf, "EmissionDuration", emitter->EmissionDuration);
	SetBool(conf, "Loop", emitter->Loop);
	SetFloat(conf, "Speed", emitter->Speed);
	SetFloat(conf, "SpeedVariation", emitter->SpeedVariation);
	SetFloat3(conf, "BoundingBox_min", emitter->BoundingBox.minPoint);
	SetFloat3(conf, "BoundingBox_max", emitter->BoundingBox.maxPoint);
	//SetUInt(conf, "EmissionType", emitter->EmissionType);
	SetUInt(conf, "Type", emitter->Type);
	SetUInt(conf, "ParticleFacingOptions", emitter->ParticleFacingOptions);

	switch (emitter->Type)
	{
	case 0: //EmitterType_Sphere
	case 1: //EmitterType_SemiSphere
		SetFloat(conf, "Radius", emitter->EmitterShape.Sphere_Shape.r);
		break;
	case 2: //EmitterType_Cone
		SetFloat(conf, "URadius", emitter->EmitterShape.ConeTrunk_Shape.Upper_Circle.r);
		SetFloat(conf, "BRadius", emitter->EmitterShape.ConeTrunk_Shape.Bottom_Circle.r);
		SetFloat(conf, "heigth", emitter->EmitterShape.ConeTrunk_Shape.heigth);
		break;
	case 3: //EmitterType_Box
		SetFloat3(conf, "EmitterAABB_min", emitter->EmitterShape.Box_Shape.minPoint);
		SetFloat3(conf, "EmitterAABB_max", emitter->EmitterShape.Box_Shape.maxPoint);
		break;
	case 4: //EmitterType_Circle
		SetFloat(conf, "Radius", emitter->EmitterShape.Circle_Shape.r);
		break;
	}

	json_serialize_to_file(root_value, file_name);
	json_value_free(root_value);
	return true;
}

bool CompParticleSystem::LoadParticleEmitter(const char* file_name, CompParticleSystem* system, ParticleEmitter& emitter) const
{
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	std::string file_path = App->fs->GetFullPath("Assets\\ParticleSystem\\Emitters");
	file_path += "\\";
	file_path += file_name;

	root_value = json_parse_file(file_path.c_str());
	if (root_value == NULL)
		root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);

	JSON_Object* conf = nullptr;
	conf = json_object_get_object(root_object, "emitter");

	bool ShowEmitterBoundBox = GetBool(conf, "ShowEmitterBoundBox");
	bool ShowEmitter = GetBool(conf, "ShowEmitter");
	system->SetDebugOptions(ShowEmitterBoundBox, ShowEmitter);

	const char* ChildParticle = GetString(conf, "ChildParticle");
	const char* ChildEmitter = GetString(conf, "ChildEmitter");
	if ((ChildParticle != nullptr) && (ChildEmitter != nullptr))
		system->SetChild(ChildParticle, ChildEmitter);

	emitter.EmitterLifeMax = GetFloat(conf, "EmitterLifeMax");
	emitter.Transform = GetFloat4x4(conf, "Transform");
	emitter.SpawnRate = GetUInt(conf, "SpawnRate");
	emitter.Lifetime = GetFloat(conf, "Lifetime");
	emitter.LifetimeVariation = GetFloat(conf, "LifetimeVariation");
	emitter.EmissionDuration = GetFloat(conf, "EmissionDuration");
	emitter.Loop = GetBool(conf, "Loop");
	emitter.Speed = GetFloat(conf, "Speed");
	emitter.SpeedVariation = GetFloat(conf, "SpeedVariation");
	emitter.BoundingBox.minPoint = GetFloat3(conf, "BoundingBox_min");
	emitter.BoundingBox.maxPoint = GetFloat3(conf, "BoundingBox_max");
	//emitter.EmissionType = (ParticleEmitter::TypeEmission)GetUInt(conf, "EmissionType");
	emitter.Type = (ParticleEmitter::TypeEmitter)GetUInt(conf, "Type");
	emitter.ParticleFacingOptions = (ParticleEmitter::TypeBillboard)GetUInt(conf, "ParticleFacingOptions");

	switch (emitter.Type)
	{
	case 0: //EmitterType_Sphere
	case 1: //EmitterType_SemiSphere
		emitter.EmitterShape.Sphere_Shape.r = GetFloat(conf, "Radius");
		break;
	case 2: //EmitterType_Cone
		emitter.EmitterShape.ConeTrunk_Shape.Upper_Circle.r = GetFloat(conf, "URadius");
		emitter.EmitterShape.ConeTrunk_Shape.Bottom_Circle.r = GetFloat(conf, "BRadius");
		emitter.EmitterShape.ConeTrunk_Shape.heigth = GetFloat(conf, "heigth");
		break;
	case 3: //EmitterType_Box
		emitter.EmitterShape.Box_Shape.minPoint = GetFloat3(conf, "EmitterAABB_min");
		emitter.EmitterShape.Box_Shape.maxPoint = GetFloat3(conf, "EmitterAABB_max");
		break;
	case 4: //EmitterType_Circle
		emitter.EmitterShape.Circle_Shape.r = GetFloat(conf, "Radius");
		break;
	}

	json_value_free(root_value);
	return true;
}

void CompParticleSystem::SetChild(const char* Particle, const char* Emitter)
{
	child_particle = Particle;
	child_emitter = Emitter;
}
//-----------------------------------------------------------
void CompParticleSystem::ShowOptions()
{
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Reset"))
	{
		ImGui::CloseCurrentPopup();
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
}

void CompParticleSystem::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsParticleSystem");
	}
	ImGui::PopStyleVar();

	ImGui::Checkbox("Show Particle Editor", &part_system->EditorWindowOpen);
	if (ImGui::Button("Save Particles Resource", ImVec2(170, 30)))
	{
		file_type = Particle_Resource;
		pop_up_save_open = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Particles Resource", ImVec2(170, 30)))
	{
		file_type = Particle_Resource;
		pop_up_load_open = true;
	}
	if (ImGui::Button("Save Emitter Resource", ImVec2(170, 30)))
	{
		file_type = Emitter_Resource;
		pop_up_save_open = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Emitter Resource", ImVec2(170, 30)))
	{
		file_type = Emitter_Resource;
		pop_up_load_open = true;
	}
	if (ImGui::Button("Load Child Particles Res", ImVec2(170, 30)))
	{
		file_type = Child_Particle_Resource;
		pop_up_load_open = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Child Emitter Res", ImVec2(170, 30)))
	{
		file_type = Child_Emitter_Resource;
		pop_up_load_open = true;
	}
	if (ImGui::Button("Unload children", ImVec2(120, 30)))
		child_particle = child_emitter = "";
	char title[1000] = "";
	if (child_particle.length() <= 950)
	{
		sprintf_s(title, 1000, "Loaded Child Particle: %s", child_particle.c_str());
		ImGui::Text(title);
	}
	if (child_emitter.length() <= 950)
	{
		sprintf_s(title, 1000, "Loaded Child Emitter: %s", child_emitter.c_str());
		ImGui::Text(title);
	}
	/*
	if (ImGui::Button("Load Mesh", ImVec2(120, 30)))
	{
	FileType = MeshResource;
	PopUpLoadOpen = true;
	}
	*/
	if (ImGui::Button("Load Texture", ImVec2(120, 30)))
	{
		file_type = Texture_Resource;
		pop_up_load_open = true;
	}




	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsParticleSystem"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	ImGui::TreePop();
}



// PopUps ---------------------------------------------------
void CompParticleSystem::ImGuiLoadPopUp()
{
	/*const char* Str = "Wrong Type";
	switch (file_type)
	{
	case Texture_Resource: Str = "Load Texture"; break;
	case Particle_Resource: Str = "Load Particle"; break;
	case Emitter_Resource: Str = "Load Emitter"; break;
	case Child_Particle_Resource: Str = "Load Child Particle"; break;
	case Child_Emitter_Resource: Str = "Load Child Emitter"; break;
	case MeshResource: Str = "Load Mesh"; break;
	}

	ImGui::OpenPopup(Str);
	if (ImGui::BeginPopupModal(Str, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (file_type == MeshResource)
		{
			ImGui::Text("You are in mesh folder of library. Here you\ncan search for files containing only one mesh.\nThis meshes are stored with the name of the\nnode containing them in the original 3d model\nfile you store in assets folder.");
			if (ImGui::Button("Load Plane", ImVec2(50, 20)))
				part_system->SetMeshResourcePlane();
		}
		else ImGui::Text("Here are only shown files that are accepted\nextention files.");

		ImGui::BeginChild("File Browser##1", ImVec2(300, 300), true);
		switch (file_type)
		{
		case Texture_Resource: DrawDirectory(App->importer->Get_Assets_path()->c_str()); break;
		case Particle_Resource: case Child_Particle_Resource: DrawDirectory(App->importer->Get_ParticleSystem_Particles_path()->c_str()); break;
		case Emitter_Resource: case Child_Emitter_Resource: DrawDirectory(App->importer->Get_ParticleSystem_Emitter_path()->c_str()); break;
		case MeshResource: DrawDirectory(App->importer->Get_Library_mesh_path()->c_str()); break;
		}
		ImGui::EndChild();
		char file_path[1000] = "";
		sprintf_s(file_path, 1000, "%s", file_to_load.c_str());
		ImGui::InputText("##input text 1", file_path, 1000, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("Ok##1", ImVec2(50, 20)))
		{
			if (!file_to_load.empty())
			{
				switch (file_type)
				{
				case Texture_Resource: ImGuiLoadTexturePopUp(); break;
				case Particle_Resource: ImGuiLoadParticlePopUp(); break;
				case Emitter_Resource: ImGuiLoadEmitterPopUp(); break;
				case Child_Particle_Resource: child_particle = file_to_load; break;
				case Child_Emitter_Resource: child_emitter = file_to_load; break;
				case MeshResource: ImGuiLoadMeshPopUp(); break;
				}
			}
			pop_up_load_open = false;
			file_to_load.clear();
			file_to_load_name.clear();
			directory_temporal_str.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##1", ImVec2(50, 20)))
		{
			pop_up_load_open = false;
			file_to_load.clear();
			file_to_load_name.clear();
			directory_temporal_str.clear();
		}
		ImGui::EndPopup();
	}*/
}

void CompParticleSystem::ImGuiLoadTexturePopUp()
{
	/*size_t bar_pos = file_to_load.rfind("\\") + 1;
	size_t dot_pos = file_to_load.rfind(".");
	file_to_load_name = file_to_load.substr(bar_pos, dot_pos - bar_pos);
	uint Texuuid = App->resources->LoadResource((*App->importer->Get_Library_material_path() + "\\" + file_to_loadName + ".dds").c_str(), file_to_load.c_str());
	SetTextureResource(Texuuid);*/
}

void CompParticleSystem::ImGuiLoadParticlePopUp()
{
	/*ParticleState InitialState;
	ParticleState FinalState;

	size_t dot_pos = file_to_load.rfind(".");
	file_to_load_name = file_to_load.substr(0, dot_pos);

	ParsonJSON* parsonjson = new ParsonJSON(file_to_loadName.c_str(), true, false, false);
	bool Loaded = parsonjson->Init();
	if (Loaded) parsonjson->LoadParticleStates(this, InitialState, FinalState);
	RELEASE(parsonjson);

	part_system->SetInitialStateResource(InitialState);
	part_system->SetFinalStateResource(FinalState);*/
}

void CompParticleSystem::ImGuiLoadEmitterPopUp()
{
	/*ParticleEmitter Emitter;

	size_t dot_pos = file_to_load.rfind(".");
	file_to_loadName = file_to_load.substr(0, dot_pos);

	ParsonJSON* parsonjson = new ParsonJSON(file_to_loadName.c_str(), true, false, false);
	bool Meta = parsonjson->Init();
	if (Meta) parsonjson->LoadParticleEmitter(this, Emitter);
	RELEASE(parsonjson);

	part_system->SetEmitterResource(Emitter);*/
}

void CompParticleSystem::ImGuiLoadMeshPopUp()
{
	//uint Meshuuid = App->resources->LoadResource(file_to_load.c_str(), file_to_load.c_str());
	//SetMeshResource(Meshuuid);
}

void CompParticleSystem::ImGuiSavePopUp()
{
	/*ImGui::OpenPopup("Save File##1");
	if (ImGui::BeginPopupModal("Save File##1", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
	{
		const char* Str = "Wrong Type";
		switch (file_type)
		{
		case Particle_Resource: Str = "Save Particle"; break;
		case Emitter_Resource: Str = "Save Emitter"; break;
		}
		static char file_name[500] = "";
		ImGui::InputText(Str, file_name, 500);
		if (ImGui::Button("Ok", ImVec2(50, 20)))
		{
			if (strcmp(file_name, ""))
			{
				FileToSave = file_name;
				switch (file_type)
				{
				case Particle_Resource: ImGuiSaveParticlePopUp(); break;
				case Emitter_Resource: ImGuiSaveEmitterPopUp(); break;
				}
			}
			PopUpSaveOpen = false;
			file_to_load.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			PopUpSaveOpen = false;
		}
		ImGui::EndPopup();
	}*/
}

void CompParticleSystem::ImGuiSaveParticlePopUp()
{
	/*ParticleState InitialState;
	part_system->GetInitialState(InitialState);
	ParticleState FinalState;
	part_system->GetFinalState(FinalState);

	FileToSaveName = *App->importer->Get_ParticleSystem_Particles_path() + "\\" + FileToSave;

	ParsonJSON* parsonjson = new ParsonJSON(FileToSaveName.c_str(), true, false, false);
	bool Meta = parsonjson->Init();
	if (Meta) parsonjson->SaveParticleStates(TextureResource, part_system->GetTextureResource(), &InitialState, &FinalState);

	RELEASE(parsonjson);*/
}

void CompParticleSystem::ImGuiSaveEmitterPopUp()
{
	/*ParticleEmitter Emitter;
	part_system->GetEmitter(Emitter);

	FileToSaveName = *App->importer->Get_ParticleSystem_Emitter_path() + "\\" + FileToSave;

	ParsonJSON* parsonjson = new ParsonJSON(FileToSaveName.c_str(), true, false, false);
	bool Meta = parsonjson->Init();
	if (Meta) parsonjson->SaveParticleEmitter(this, &Emitter);
	RELEASE(parsonjson);*/
}


/*DEBUG FUNCS*/

void CompParticleSystem::SetDebugOptions(bool ShowEmitterBoundBox, bool ShowEmitter)
{
	part_system->ShowEmitterBoundBox = ShowEmitterBoundBox;
	part_system->ShowEmitter = ShowEmitter;
}

void CompParticleSystem::GetDebugOptions(bool& ShowEmitterBoundBox, bool& ShowEmitter)
{
	ShowEmitterBoundBox = part_system->ShowEmitterBoundBox;
	ShowEmitter = part_system->ShowEmitter;
}



//JSON FUNC XAVI WTF ---------------------------------------
bool CompParticleSystem::SetInt(JSON_Object* conf, const char * field, int value) const
{
	return json_object_set_number(conf, field, (double)value) == JSONSuccess;
}

bool CompParticleSystem::SetUInt(JSON_Object* conf, const char * field, uint value) const
{
	return json_object_set_number(conf, field, (double)value) == JSONSuccess;
}

bool CompParticleSystem::SetFloat(JSON_Object* conf, const char * field, float value) const
{
	return json_object_set_number(conf, field, (double)value) == JSONSuccess;
}

bool CompParticleSystem::SetDouble(JSON_Object* conf, const char * field, double value) const
{
	return json_object_set_number(conf, field, (double)value) == JSONSuccess;
}

bool CompParticleSystem::SetBool(JSON_Object* conf, const char * field, bool value) const
{
	uint boolean = 0;
	if (value) boolean = 1;
	return json_object_set_boolean(conf, field, boolean) == JSONSuccess;
}

bool CompParticleSystem::SetString(JSON_Object* conf, const char * field, const char* value) const
{
	if (value != nullptr) return json_object_set_string(conf, field, value) == JSONSuccess;
	return false;
}

bool CompParticleSystem::SetFloat2(JSON_Object* conf, const char* field, float2 value) const
{
	bool ret = true;
	JSON_Object* float2_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	float2_iterate = json_object_get_object(conf, field);

	ret = SetFloat(float2_iterate, "x", value.x);
	if (!ret) return false;
	ret = SetFloat(float2_iterate, "y", value.y);
	return ret;
}

bool CompParticleSystem::SetFloat3(JSON_Object* conf, const char* field, float3 value) const
{
	bool ret = true;
	JSON_Object* float3_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	float3_iterate = json_object_get_object(conf, field);

	ret = SetFloat(float3_iterate, "x", value.x);
	if (!ret) return false;
	ret = SetFloat(float3_iterate, "y", value.y);
	if (!ret) return false;
	ret = SetFloat(float3_iterate, "z", value.z);
	return ret;
}

bool CompParticleSystem::SetFloat4(JSON_Object * conf, const char * field, float4 value) const
{
	bool ret = true;
	JSON_Object* float4_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	float4_iterate = json_object_get_object(conf, field);

	ret = SetFloat(float4_iterate, "x", value.x);
	if (!ret) return false;
	ret = SetFloat(float4_iterate, "y", value.y);
	if (!ret) return false;
	ret = SetFloat(float4_iterate, "z", value.z);
	if (!ret) return false;
	ret = SetFloat(float4_iterate, "w", value.w);
	return ret;
}

bool CompParticleSystem::SetFloat4x4(JSON_Object* conf, const char* field, float4x4 value) const
{
	bool ret = true;
	JSON_Object* float4x4_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	float4x4_iterate = json_object_get_object(conf, field);

	ret = SetFloat(float4x4_iterate, "00", value[0][0]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "01", value[0][1]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "02", value[0][2]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "03", value[0][3]);

	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "10", value[1][0]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "11", value[1][1]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "12", value[1][2]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "13", value[1][3]);

	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "20", value[2][0]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "21", value[2][1]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "22", value[2][2]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "23", value[2][3]);

	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "30", value[3][0]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "31", value[3][1]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "32", value[3][2]);
	if (!ret) return false;
	ret = SetFloat(float4x4_iterate, "33", value[3][3]);
	return ret;
}

bool CompParticleSystem::SetQuat(JSON_Object* conf, const char* field, Quat value) const
{
	bool ret = true;
	JSON_Object* quat_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	quat_iterate = json_object_get_object(conf, field);

	ret = SetFloat(quat_iterate, "x", value.x);
	if (!ret) return false;
	ret = SetFloat(quat_iterate, "y", value.y);
	if (!ret) return false;
	ret = SetFloat(quat_iterate, "z", value.z);
	if (!ret) return false;
	ret = SetFloat(quat_iterate, "w", value.w);
	return ret;
}

bool CompParticleSystem::SetColor(JSON_Object* conf, const char* field, Color color) const
{
	bool ret = true;
	JSON_Object* color_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	color_iterate = json_object_get_object(conf, field);

	ret = SetFloat(color_iterate, "r", color.r);
	if (!ret) return false;
	ret = SetFloat(color_iterate, "g", color.g);
	if (!ret) return false;
	ret = SetFloat(color_iterate, "b", color.b);
	if (!ret) return false;
	ret = SetFloat(color_iterate, "a", color.a);
	return ret;
}

int CompParticleSystem::GetInt(JSON_Object* conf, const char* field, int default) const
{
	JSON_Value* value = json_object_get_value(conf, field);
	if (value && (json_value_get_type(value) == JSONNumber))
		return (int)json_value_get_number(value);
	return default;
}

uint CompParticleSystem::GetUInt(JSON_Object* conf, const char* field, uint default) const
{
	JSON_Value* value = json_object_get_value(conf, field);
	if (value && (json_value_get_type(value) == JSONNumber))
		return (uint)json_value_get_number(value);
	return default;
}

float CompParticleSystem::GetFloat(JSON_Object* conf, const char* field, float default) const
{
	JSON_Value* value = json_object_get_value(conf, field);
	if (value && (json_value_get_type(value) == JSONNumber))
		return (float)json_value_get_number(value);
	return default;
}

double CompParticleSystem::GetDouble(JSON_Object* conf, const char* field, double default) const
{
	JSON_Value* value = json_object_get_value(conf, field);
	if (value && (json_value_get_type(value) == JSONNumber))
		return json_value_get_number(value);
	return default;
}

bool CompParticleSystem::GetBool(JSON_Object* conf, const char* field, bool default) const
{
	JSON_Value* value = json_object_get_value(conf, field);
	if (value && (json_value_get_type(value) == JSONBoolean))
		return json_value_get_boolean(value);
	return default;
}

const char* CompParticleSystem::GetString(JSON_Object* conf, const char* field, const char* default) const
{
	JSON_Value* value = json_object_get_value(conf, field);
	if (value && (json_value_get_type(value) == JSONString))
		return json_value_get_string(value);
	return default;
}

float2 CompParticleSystem::GetFloat2(JSON_Object* conf, const char* field, float2 default) const
{
	float2 ret = float2::zero;
	JSON_Object* float2_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	float2_iterate = json_object_get_object(conf, field);

	ret.x = GetFloat(float2_iterate, "x", default.x);
	ret.y = GetFloat(float2_iterate, "y", default.y);
	return ret;
}

float3 CompParticleSystem::GetFloat3(JSON_Object* conf, const char* field, float3 default) const
{
	float3 ret = float3::zero;
	JSON_Object* float3_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	float3_iterate = json_object_get_object(conf, field);

	ret.x = GetFloat(float3_iterate, "x", default.x);
	ret.y = GetFloat(float3_iterate, "y", default.y);
	ret.z = GetFloat(float3_iterate, "z", default.z);
	return ret;
}

float4 CompParticleSystem::GetFloat4(JSON_Object * conf, const char * field, float4 default) const
{
	float4 ret = float4::zero;
	JSON_Object* float4_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	float4_iterate = json_object_get_object(conf, field);

	ret.x = GetFloat(float4_iterate, "x", default.x);
	ret.y = GetFloat(float4_iterate, "y", default.y);
	ret.z = GetFloat(float4_iterate, "z", default.z);
	ret.w = GetFloat(float4_iterate, "w", default.w);
	return ret;
}

float4x4 CompParticleSystem::GetFloat4x4(JSON_Object* conf, const char* field, float4x4 default) const
{
	float4x4 ret = float4x4::identity;
	JSON_Object* float4x4_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	float4x4_iterate = json_object_get_object(conf, field);

	ret[0][0] = GetFloat(float4x4_iterate, "00", default[0][0]);
	ret[0][1] = GetFloat(float4x4_iterate, "01", default[0][1]);
	ret[0][2] = GetFloat(float4x4_iterate, "02", default[0][2]);
	ret[0][3] = GetFloat(float4x4_iterate, "03", default[0][3]);

	ret[1][0] = GetFloat(float4x4_iterate, "10", default[1][0]);
	ret[1][1] = GetFloat(float4x4_iterate, "11", default[1][1]);
	ret[1][2] = GetFloat(float4x4_iterate, "12", default[1][2]);
	ret[1][3] = GetFloat(float4x4_iterate, "13", default[1][3]);

	ret[2][0] = GetFloat(float4x4_iterate, "20", default[2][0]);
	ret[2][1] = GetFloat(float4x4_iterate, "21", default[2][1]);
	ret[2][2] = GetFloat(float4x4_iterate, "22", default[2][2]);
	ret[2][3] = GetFloat(float4x4_iterate, "23", default[2][3]);

	ret[3][0] = GetFloat(float4x4_iterate, "30", default[3][0]);
	ret[3][1] = GetFloat(float4x4_iterate, "31", default[3][1]);
	ret[3][2] = GetFloat(float4x4_iterate, "32", default[3][2]);
	ret[3][3] = GetFloat(float4x4_iterate, "33", default[3][3]);

	return ret;
}

Quat CompParticleSystem::GetQuat(JSON_Object* conf, const char* field, Quat default) const
{
	Quat ret = Quat::identity;
	JSON_Object* quat_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	quat_iterate = json_object_get_object(conf, field);

	ret.x = GetFloat(quat_iterate, "x", default.x);
	ret.y = GetFloat(quat_iterate, "y", default.y);
	ret.z = GetFloat(quat_iterate, "z", default.z);
	ret.w = GetFloat(quat_iterate, "w", default.w);
	return ret;
}

Color CompParticleSystem::GetColor(JSON_Object* conf, const char* field, Color default) const
{
	Color ret = default;
	JSON_Object* color_iterate = nullptr;
	//If this entry does not exist, create it
	if (json_object_get_object(conf, field) == NULL)
		json_object_set_value(conf, field, json_value_init_object());
	color_iterate = json_object_get_object(conf, field);

	ret.r = GetFloat(color_iterate, "r", default.r);
	ret.g = GetFloat(color_iterate, "g", default.g);
	ret.b = GetFloat(color_iterate, "b", default.b);
	ret.a = GetFloat(color_iterate, "a", default.a);
	return ret;
}