#include "Component.h"
#include "GameObject.h"
#include "Application.h"

Component::Component(Comp_Type t, GameObject* parent) :type(t), parent(parent), active(true)
{
}

Component::~Component()
{
}

bool Component::Enable()
{
	if (!active)
	{
		active = true;
	}
	return active;
}

bool Component::Disable()
{
	if (active)
	{
		active = false;
	}
	return active;
}

void Component::Init()
{
}

void Component::preUpdate(float dt)
{
}

void Component::Update(float dt)
{
}

void Component::Draw()
{
}

void Component::Clear()
{
}

void Component::ShowOptions()
{
}

void Component::ShowInspectorInfo()
{
}

Comp_Type Component::GetType() const
{
	return type;
}

bool Component::isActive() const
{
	return active;
}

void Component::SetActive(bool active)
{
	this->active = active;
}

uint Component::GetUUID() const
{
	return uid;
}

void Component::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
}

void Component::Load(const JSON_Object* object, std::string name)
{
}