#include "Resource_.h"


Resource::Resource(uint uid, Resource::Type type, Resource::State state) : uuid(uid), type(type), state(state)
{
}

Resource::~Resource()
{
	RELEASE_ARRAY(name);
}

Resource::Type Resource::GetType() const
{
	return type;
}

Resource::State Resource::GetState() const
{
	return state;
}

uint Resource::GetUUID() const
{
	return uuid;
}

void Resource::SetState(Resource::State newstate)
{
	state = newstate;
}

