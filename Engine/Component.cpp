#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "CompTransform.h"
#include "JSONSerialization.h"

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

bool Component::InScripting() const
{
	return in_script;
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

void Component::SetParent(GameObject * new_parent)
{
	if (new_parent != nullptr)
	{
		parent = new_parent;
	}
}

void Component::SetActive(bool active)
{
	this->active = active;
}

void Component::SetUUIDRandom()
{
	uid = App->random->Int();
}

void Component::SetInScripting()
{
	in_script = true;
}

void Component::SetDelete(bool set_delete)
{
	to_delete = set_delete;
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

void Component::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(int);			//identifier
	buffer_size += sizeof(int);			//Type
	buffer_size += sizeof(int);			//UID Parent
	//buffer_size += sizeof(int);			//position
}

void Component::SaveBinary(char ** cursor, int position) const
{
	// Identificator Component
	App->json_seria->SaveIntBinary(cursor, IDENTIFICATOR_COMPONENT);
	// Type Component--------
	App->json_seria->SaveIntBinary(cursor, (int)type);
	// UUID Parent --------
	App->json_seria->SaveIntBinary(cursor, parent->GetUUID());
	// Position in Parent --------
	//App->json_seria->SaveIntBinary(cursor, position);
}

void Component::LoadBinary(char ** cursor)
{
}

void Component::SyncComponent(GameObject* sync_parent)
{
}


/* 
Base:
- Identificator Component: IDENTIFICATOR_COMPONENT
- Int: type
- Int: UID parent
- Int: position in parent
In All Components SaveBinary Function:
first put this line-> Component::SaveBinary(cursor, position);
*/