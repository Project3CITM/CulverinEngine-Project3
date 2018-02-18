#include "CompParticleSystem.h"
#include "Application.h"
#include "Scene.h"

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

}

void CompParticleSystem::Update(float dt)
{

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