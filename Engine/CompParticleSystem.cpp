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
#include "ModuleEventSystem.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"

CompParticleSystem::CompParticleSystem(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	name_component = "CompParticleSystem";

	emitter_resource_name.reserve(100);
	particle_resource_name.reserve(100);

	emitter_resource_name = parent->GetName();
	emitter_resource_name += "_emitter";
	particle_resource_name = parent->GetName();
	particle_resource_name += "_particle";

	part_system = new ParticleSystem();
	uid = App->random->Int();
}

CompParticleSystem::CompParticleSystem(const CompParticleSystem& copy, GameObject* parent) : Component(copy.GetType(), parent)
{
	uid = App->random->Int();
	name_component = "CompParticleSystem";
	emitter_resource_name.reserve(100);
	particle_resource_name.reserve(100);

	emitter_resource_name = copy.emitter_resource_name;
	particle_resource_name = copy.particle_resource_name;

	part_system = new ParticleSystem();

	discard_distance = copy.discard_distance;


	file_to_load = particle_resource_name;
	ImGuiLoadParticlePopUp();

	file_to_load = emitter_resource_name;
	ImGuiLoadEmitterPopUp();

	if (copy.active)
		part_system->ActivateEmitter();
	else
		part_system->DeactivateEmitter();
}

CompParticleSystem::~CompParticleSystem()
{
	
}


void CompParticleSystem::PreUpdate(float dt)
{
	const CompCamera* camera = App->renderer3D->GetActiveCamera();
	if (camera != nullptr)
		part_system->SetCameraPosToFollow(camera->frustum.pos);
		
	distance_to_camera = camera->frustum.pos.Distance(parent->GetComponentTransform()->GetPosGlobal());

	if(distance_to_camera < discard_distance)
		if(App->engine_state == EngineState::STOP)
			part_system->PreUpdate(0.02);
		else part_system->PreUpdate(dt);
}

void CompParticleSystem::Update(float dt)
{
	if (pop_up_load_open) ImGuiLoadPopUp();
	if (pop_up_save_open) 
	{ 
		ImGuiSavePopUp();
		pop_up_save_open = false;
	}

	if (!child_loaded && part_system->IsEmitterDead() && !child_particle.empty() && !child_emitter.empty())
	{
		//Load Child Particle
		ParticleState InitialState;
		ParticleState FinalState;

		child_particle_name = App->fs->GetOnlyName(child_particle);
		LoadParticleStates(child_particle_name.c_str(),this, InitialState, FinalState);
	
		part_system->SetInitialStateResource(InitialState);
		part_system->SetFinalStateResource(FinalState);

		//Load Child Emitter
		ParticleEmitter Emitter;
		child_emitter_name = App->fs->GetOnlyName(child_emitter);
		LoadParticleEmitter(child_emitter_name.c_str(), this, Emitter);
		part_system->SetEmitterResource(Emitter);

		child_loaded = false;
	}

	part_system->SetEmitterTransform(parent->GetComponentTransform()->GetGlobalTransform().Transposed());
	
	if (distance_to_camera < discard_distance)
	{
		if (App->engine_state == EngineState::STOP)
			part_system->Update(0.02, preview);
		else part_system->Update(dt);

		part_system->DrawImGuiEditorWindow();

		if (App->engine_state == EngineState::STOP)
			part_system->PostUpdate(0.02);
		else part_system->PostUpdate(dt);
	}
	
}

void CompParticleSystem::Clear()
{
	App->event_system->ClearEvents(EventType::EVENT_PARTICLE_DRAW);

	if (part_system != nullptr)
	{
		part_system->CleanUp();
		delete part_system;
		part_system = nullptr;
	}

	if (texture_resource && texture_resource->num_game_objects_use_me > 0)
	{
		texture_resource->num_game_objects_use_me--;
		texture_resource = nullptr;
	}
}

void CompParticleSystem::Draw()
{
	if (part_system->ShowEmitter) part_system->DebugDrawEmitter();
	if (part_system->ShowEmitterBoundBox) part_system->DebugDrawEmitterAABB();
}

void CompParticleSystem::SetTextureResource(const char * Path, int columns, int rows, int numberOfFrames, uint AnimationOrder)
{
	file_to_load = Path;
	size_t bar_pos = file_to_load.rfind("\\") + 1;
	file_to_load_name = file_to_load.substr(bar_pos);
	ResourceMaterial* res = (ResourceMaterial*)App->resource_manager->GetResource(file_to_load_name.c_str());
	if(res)
		SetTextureResource(res->GetUUID(), columns, rows, numberOfFrames, AnimationOrder);
}

void CompParticleSystem::SetTextureResource(uint uuid, int columns, int rows, int numberOfFrames, uint AnimationOrder)
{
	if (texture_resource != nullptr && texture_resource->num_game_objects_use_me > 0)
	{
		texture_resource->num_game_objects_use_me--;
	}		
	
	texture_resource = (ResourceMaterial*)App->resource_manager->GetResource(uuid);//(texture_resource*)App->resources->Get(uuid);
	if (texture_resource != nullptr)
	{
		if (texture_resource->GetState() != Resource::State::LOADED)
		{
			texture_resource->LoadToMemory();
			App->importer->iMaterial->LoadResource(std::to_string(texture_resource->GetUUID()).c_str(), texture_resource);
			
		}
		texture_resource->num_game_objects_use_me++;

		part_system->SetTextureResource(texture_resource->GetTextureID(), texture_resource->GetTextureWidth(), texture_resource->GetTextureHeight(), columns, rows, numberOfFrames, AnimationOrder);
	}
	else LOG("WARNING: Texture resource in particles is nullptr");
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

void CompParticleSystem::ActivateEmitter(bool a)
{
	if (a)
		part_system->ActivateEmitter();
	else part_system->DeactivateEmitter();
}


// LOAD / SAVE ---------------------------------------------------------

void CompParticleSystem::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());

	json_object_dotset_string_with_std(object, name + "ParticleResource:", particle_resource_name.c_str());
	json_object_dotset_string_with_std(object, name + "EmitterResource:", emitter_resource_name.c_str());
	json_object_dotset_boolean_with_std(object, name + "Preview:", preview);
	json_object_dotset_boolean_with_std(object, name + "Active:", part_system->IsEmitterActive());

	json_object_dotset_number_with_std(object, name + "DiscardDistance", discard_distance);


	ImGuiSaveParticlePopUp();
	ImGuiSaveEmitterPopUp();

	
}

void CompParticleSystem::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	particle_resource_name = json_object_dotget_string_with_std(object, name + "ParticleResource:");
	emitter_resource_name = json_object_dotget_string_with_std(object, name + "EmitterResource:");
	
	file_to_load = particle_resource_name;
	ImGuiLoadParticlePopUp();

	file_to_load = emitter_resource_name;
	ImGuiLoadEmitterPopUp();

	preview = json_object_dotget_boolean_with_std(object, name + "Preview:");
	bool active = json_object_dotget_boolean_with_std(object, name + "Active:");
	if (active)
		part_system->ActivateEmitter();
	else
		part_system->DeactivateEmitter();

	discard_distance = json_object_dotget_number_with_std(object, name + "DiscardDistance");


	
}



bool CompParticleSystem::SaveParticleStates(ResourceMaterial* TextureResource, const ParticleTextureData* TexData, const ParticleState* stateI, const ParticleState* stateF) const
{
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;


	std::string assets_file_path;
	if (!App->mode_game)
	{
		App->fs->CreateFolder("Assets/ParticleSystem");
		App->fs->CreateFolder("Assets/ParticleSystem/Particles");
		assets_file_path = App->fs->GetFullPath("Assets\\ParticleSystem\\Particles");
		assets_file_path += "\\";
		assets_file_path += particle_resource_name;
		assets_file_path += ".json";

	}
	App->fs->CreateFolder("Library/ParticleSystem");
	App->fs->CreateFolder("Library/ParticleSystem/Particles");
	std::string library_file_path = App->fs->GetFullPath("Library\\ParticleSystem\\Particles");
	
	library_file_path += "\\";
	library_file_path += particle_resource_name;
	library_file_path += ".json";

	root_value = json_parse_file(library_file_path.c_str());
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
		SetString(conf, "texture_path", TextureResource->name.c_str());
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

	if(!App->mode_game)
		json_serialize_to_file(root_value, assets_file_path.c_str());
	json_serialize_to_file(root_value, library_file_path.c_str());
	json_value_free(root_value);
	return true;
}

bool CompParticleSystem::LoadParticleStates(const char* file_name, CompParticleSystem* system, ParticleState& stateI, ParticleState& stateF)
{
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	std::string file_path = App->fs->GetFullPath("Library\\ParticleSystem\\Particles");
	file_path += "\\";
	file_path += file_name;
	particle_resource_name = file_name;
	file_path += ".json";
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



bool CompParticleSystem::SaveParticleEmitter(const CompParticleSystem* system, const ParticleEmitter* emitter) const
{
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;

	App->fs->CreateFolder("Library/ParticleSystem");
	App->fs->CreateFolder("Library/ParticleSystem/Emitters");
	std::string assets_file_path;
	std::string library_file_path = App->fs->GetFullPath("Library\\ParticleSystem\\Emitters");

	if (!App->mode_game)
	{
		App->fs->CreateFolder("Assets/ParticleSystem");
		App->fs->CreateFolder("Assets/ParticleSystem/Emitters");
		assets_file_path = App->fs->GetFullPath("Assets\\ParticleSystem\\Emitters");
		assets_file_path += "\\";
		assets_file_path += emitter_resource_name;
		assets_file_path += ".json";
	}

	library_file_path += "\\";
	library_file_path += emitter_resource_name;
	library_file_path += ".json";

	root_value = json_parse_file(library_file_path.c_str());
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
		SetString(conf, "ChildParticle", GetChildParticle()->c_str());
	if (system->GetChildEmitter() != nullptr)
		SetString(conf, "ChildEmitter", GetChildEmitter()->c_str());

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

	if(!App->mode_game)
		json_serialize_to_file(root_value, assets_file_path.c_str());

	json_serialize_to_file(root_value, library_file_path.c_str());
	json_value_free(root_value);
	return true;
}

bool CompParticleSystem::LoadParticleEmitter(const char* file_name, CompParticleSystem* system, ParticleEmitter& emitter) 
{
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	std::string file_path = App->fs->GetFullPath("Library\\ParticleSystem\\Emitters");
	file_path += "\\";
	file_path += file_name;
	emitter_resource_name = file_name;
	file_path += ".json";

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
	if (ImGui::MenuItem("Copy Component", NULL, false))
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

	bool emitter_active = part_system->IsEmitterActive();
	if (ImGui::Checkbox("Active", &emitter_active))
	{
		if (emitter_active)
			part_system->ActivateEmitter();
		else 
			part_system->DeactivateEmitter();
	}
	ImGui::Checkbox("Preview", &preview);

	ImGui::DragFloat("Discard distance", &discard_distance, 1.0f, 0, 1000, "%.2f");
	
	//Emitter options
	if (ImGui::TreeNodeEx("Emitter"))
	{
		ImGui::Checkbox("Show Emitter Editor", &part_system->EmitterEditorOpen);

		static char emitter_name[100];
		memcpy_s(emitter_name, 99, emitter_resource_name.c_str(), 99);
		ImGui::InputText("- Emitter Name", emitter_name, 99);
		emitter_resource_name = emitter_name;

		if (ImGui::SmallButton("Save Emitter Conf"))
		{
			file_type = Emitter_Resource;
			pop_up_save_open = true;
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Load Emitter Conf"))
		{
			file_type = Emitter_Resource;
			pop_up_load_open = true;
		}
		ImGui::TreePop();
	}	
	

	//Particle options
	if (ImGui::TreeNodeEx("Particle"))
	{
		ImGui::Checkbox("Show Particle Editor", &part_system->ParticleEditorOpen);

		static char particle_name[100];
		memcpy_s(particle_name, 99, particle_resource_name.c_str(), 99);
		ImGui::InputText("- Particle Name", particle_name, 99);
		particle_resource_name = particle_name;

		if (ImGui::SmallButton("Save Particles Conf"))
		{
			file_type = Particle_Resource;
			pop_up_save_open = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Particles Conf"))
		{
			file_type = Particle_Resource;
			pop_up_load_open = true;
		}

		if (ImGui::Button("Load Texture", ImVec2(120, 30)))
		{
			file_type = Texture_Resource;
			pop_up_load_open = true;
		}
		ImGui::TreePop();
	}

	//Child options
	/*if (ImGui::TreeNodeEx("Child"))
	{
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
		ImGui::TreePop();
	}*/
	
	/*
	if (ImGui::Button("Load Mesh", ImVec2(120, 30)))
	{
	FileType = MeshResource;
	PopUpLoadOpen = true;
	}
	*/

	
	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsParticleSystem"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	ImGui::TreePop();
}

void CompParticleSystem::CopyComponent()
{
}

void CompParticleSystem::DrawDirectory(const char * directory)
{
	for (std::experimental::filesystem::directory_iterator::value_type file_in_path : std::experimental::filesystem::directory_iterator(directory))
	{
		char title[1000] = "";
		if (std::experimental::filesystem::is_directory(file_in_path.path()) && (file_in_path.path().string().length() < 1000))
		{
			sprintf_s(title, 1000, "%S", file_in_path.path().filename().c_str());
			if (ImGui::TreeNodeEx(title, 0))
			{
				sprintf_s(title, 1000, "%S", file_in_path.path().c_str());
				DrawDirectory(title);
				ImGui::TreePop();
			}
		}
		if (std::experimental::filesystem::is_regular_file(file_in_path.path()) && (file_in_path.path().string().length() < 1000))
		{
			sprintf_s(title, 1000, "%S", file_in_path.path().extension().c_str());
			directory_temporal_str = title;
			bool Valid = false;
			//Change Particle_Resource extention to .particle.json
			//Change Emitter_Resource extention to .emitter.json
			switch (file_type)
			{
			case Texture_Resource: if ((directory_temporal_str == ".png") || (directory_temporal_str == ".PNG") || (directory_temporal_str == ".jpg") || (directory_temporal_str == ".JPG") || (directory_temporal_str == ".tga") || (directory_temporal_str == ".TGA") || (directory_temporal_str == ".dds") || (directory_temporal_str == ".DDS")) Valid = true; break;
			case Particle_Resource: case Child_Particle_Resource: case Emitter_Resource: case Child_Emitter_Resource: if (directory_temporal_str == ".json") Valid = true; break;
			case MeshResource: Valid = true; break;
			}
			if (Valid)
			{
				sprintf_s(title, 1000, "%S", file_in_path.path().filename().c_str());
				if (ImGui::TreeNodeEx(title, ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::IsItemClicked())
					{
						char path[1000] = "";
						sprintf_s(path, 1000, "%S", file_in_path.path().c_str());
						file_to_load = path;
					}
					ImGui::TreePop();
				}
			}
		}
	}
}

// PopUps ---------------------------------------------------
void CompParticleSystem::ImGuiLoadPopUp()
{
	const char* Str = "Wrong Type";
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
		case Texture_Resource: DrawDirectory(App->fs->GetFullPath("Assets").c_str()); break;
		case Particle_Resource: case Child_Particle_Resource: DrawDirectory(App->fs->GetFullPath("Assets\\ParticleSystem\\Particles").c_str()); break;
		case Emitter_Resource: case Child_Emitter_Resource: DrawDirectory(App->fs->GetFullPath("Assets\\ParticleSystem\\Emitters").c_str()); break;
		case MeshResource: DrawDirectory(App->fs->GetFullPath("Assets").c_str()); break;
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
	}
}

void CompParticleSystem::ImGuiLoadTexturePopUp()
{
	size_t bar_pos = file_to_load.rfind("\\") + 1;
	file_to_load_name = file_to_load.substr(bar_pos);
	/*ResourceMaterial* new_text = (ResourceMaterial*)App->resource_manager->GetResource(file_to_load_name.c_str());
	if (new_text == nullptr)
	{
		new_text = (ResourceMaterial*)App->resource_manager->CreateNewResource(Resource::Type::MATERIAL);
		new_text->LoadToMemory();
	}*/
	SetTextureResource(file_to_load_name.c_str());
}

void CompParticleSystem::ImGuiLoadParticlePopUp()
{
	ParticleState InitialState;
	ParticleState FinalState;

	file_to_load_name = App->fs->GetOnlyName(file_to_load);
	LoadParticleStates(file_to_load_name.c_str(),this, InitialState, FinalState);
	

	part_system->SetInitialStateResource(InitialState);
	part_system->SetFinalStateResource(FinalState);
}

void CompParticleSystem::ImGuiLoadEmitterPopUp()
{
	ParticleEmitter Emitter;
	file_to_load_name = App->fs->GetOnlyName(file_to_load);
	LoadParticleEmitter(file_to_load_name.c_str(), this, Emitter);

	part_system->SetEmitterResource(Emitter);
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
				file_to_save = file_name;
				switch (file_type)
				{
				case Particle_Resource: ImGuiSaveParticlePopUp(); break;
				case Emitter_Resource: ImGuiSaveEmitterPopUp(); break;
				}
			}
			pop_up_save_open = false;
			file_to_load.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			pop_up_save_open = false;
		}
		ImGui::EndPopup();
	}
	*/

	switch (file_type)
	{
	case Particle_Resource: ImGuiSaveParticlePopUp(); break;
	case Emitter_Resource: ImGuiSaveEmitterPopUp(); break;
	}
}

void CompParticleSystem::ImGuiSaveParticlePopUp()const
{
	ParticleState InitialState;
	part_system->GetInitialState(InitialState);
	ParticleState FinalState;
	part_system->GetFinalState(FinalState);
	SaveParticleStates(texture_resource, part_system->GetTextureResource(), &InitialState, &FinalState);
}

void CompParticleSystem::ImGuiSaveEmitterPopUp()const
{
	ParticleEmitter Emitter;
	part_system->GetEmitter(Emitter);
	SaveParticleEmitter(this, &Emitter);
}


/*DEBUG FUNCS*/

void CompParticleSystem::SetDebugOptions(bool ShowEmitterBoundBox, bool ShowEmitter)
{
	part_system->ShowEmitterBoundBox = ShowEmitterBoundBox;
	part_system->ShowEmitter = ShowEmitter;
}

void CompParticleSystem::GetDebugOptions(bool& ShowEmitterBoundBox, bool& ShowEmitter) const
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