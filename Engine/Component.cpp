#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "CompTransform.h"

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

bool Component::GetEnabled()
{
	return active;
}

void Component::Init()
{
}

void Component::PreUpdate(float dt)
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

bool Component::IsActive() const
{
	return active;
}

const char * Component::GetName() const
{
	return name_component;
}

bool Component::WantDelete() const
{
	return to_delete;
}

float3 Component::GetGameObjectPos() const
{
   	CompTransform* transfrom = (CompTransform*)parent->FindComponentByType(C_TRANSFORM);
	if (transfrom != nullptr) return transfrom->GetPosGlobal();
	else return float3::zero;
}

GameObject* Component::GetParent() const
{
	return parent;
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

void Component::SyncComponent(GameObject* sync_parent)
{
}
