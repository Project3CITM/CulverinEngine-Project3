#ifndef _RESOURCE_
#define _RESOURCE_

#include "Globals.h"
#include <string>

typedef struct json_object_t JSON_Object;

class Resource
{
public:
	enum Type {
		UNKNOWN,
		FOLDER,
		MATERIAL,
		MESH,
		SCRIPT,
		FONT,
		ANIMATION,
		SKELETON
	};
	enum State {
		UNLOADED,
		LOADED,
		REIMPORTED,
		WANTDELETE,
		FAILED,
		REIMPORTEDSCRIPT
	};

public:

	Resource(uint uuid, Resource::Type type, Resource::State state);
	virtual ~Resource();

	Resource::Type GetType() const;
	Resource::State GetState() const;
	uint GetUUID() const;
	void SetState(Resource::State state);

	virtual void DeleteToMemory();
	virtual Resource::State IsLoadedToMemory();

protected:

	Type type = Type::UNKNOWN;
	State state = State::UNLOADED;
	uint uuid = 0;

public:

	char* name = "Name Resource";
	std::string path_assets;
	uint num_game_objects_use_me = 0;

};

#endif