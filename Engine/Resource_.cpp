#include "Resource_.h"
#include "Application.h"
#include "Globals.h"

Resource::Resource(uint uid, Resource::Type type, Resource::State state) : uuid(uid), type(type), state(state)
{
}

Resource::~Resource()
{
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

void Resource::DeleteToMemory()
{

}

Resource::State Resource::IsLoadedToMemory()
{
	return state;
}

