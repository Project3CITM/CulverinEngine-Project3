#include "CompParticleSystem.h"
#include "Application.h"
#include "Scene.h"
#include "ParticleSystem.h"
#include "ResourceMaterial.h"
#include "parson.h"

CompParticleSystem::CompParticleSystem(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	name_component = "Particle_System";
}

CompParticleSystem::CompParticleSystem(const CompParticleSystem& copy, GameObject* parent) : Component(copy.GetType(), parent)
{

}

CompParticleSystem::~CompParticleSystem()
{

}

void CompParticleSystem::Init()
{

}

void CompParticleSystem::PreUpdate(float dt)
{
	//If the engine is in play mode unpaused, try to get main camera pos
/*	if (App->engine_state == EngineState::PLAY)
	{
		const ComponentCamera* camera = App->scene->GetActiveCamera();
		//If we have main camera, get pos and load
		if (camera != nullptr) PartSystem->SetCameraPosToFollow(camera->parent->GetTransform()->GetPos());
		//If not, continue loading editor pos
		else PartSystem->SetCameraPosToFollow(App->camera->Position);
	}
	//Load editor camera pos
	else PartSystem->SetCameraPosToFollow(App->camera->Position);
	PartSystem->PreUpdate(dt);
	return true;*/
}

void CompParticleSystem::Update(float dt)
{
	if (pop_up_load_open) ImGuiLoadPopUp();
	if (pop_up_save_open) ImGuiSavePopUp();
}

void CompParticleSystem::Draw()
{

}

void CompParticleSystem::Clear()
{

}

void CompParticleSystem::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{

}

void CompParticleSystem::Load(const JSON_Object* object, std::string name)
{

}

void CompParticleSystem::ShowOptions()
{

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