#include "Application.h"
#include "ResourceMesh.h"
#include "CompBone.h"

CompBone::CompBone(Comp_Type t, GameObject * parent): Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Bone";
}

CompBone::CompBone(const CompBone & copy, GameObject * parent) : Component(Comp_Type::C_BONE, parent)
{
	resource_mesh = copy.resource_mesh;
	if (resource_mesh != nullptr)
		resource_mesh->num_game_objects_use_me++;
	name_component = "Bone";
}

CompBone::~CompBone()
{}

bool CompBone::Enable()
{
	return false;
}

bool CompBone::Disable()
{
	return false;
}

void CompBone::Init()
{
}

void CompBone::PreUpdate(float dt)
{
}

void CompBone::Update(float dt)
{
}

void CompBone::Draw()
{
}

void CompBone::Clear()
{
}

void CompBone::ShowOptions()
{
}
