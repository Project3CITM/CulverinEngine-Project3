#include "ModuleShaders.h"

ModuleShaders::ModuleShaders()
{
}

ModuleShaders::~ModuleShaders()
{
}

bool ModuleShaders::Init(JSON_Object * node)
{
	return true;
}

bool ModuleShaders::Start()
{
	return true;
}

update_status ModuleShaders::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleShaders::Update(float dt)
{
	
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
