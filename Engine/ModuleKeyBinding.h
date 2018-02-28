#ifndef MODULE_KEY_BINDING_H_
#define MODULE_KEY_BINDING_H_
#include "Module.h"
#include"SDL\include\SDL_scancode.h"

struct Key_Relation {

	Key_Relation(SDL_Scancode type_event, std::string name) : event_scancode(type_event), name(name) {}

	SDL_Scancode event_scancode;
	std::string name = "";
};


class ModuleKeyBinding :
	public Module
{
public:
	ModuleKeyBinding();
	~ModuleKeyBinding();

	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);
	bool CleanUp();

	Key_Relation Find_key_binding(const char* name);

private:

	std::vector<Key_Relation> key_binding_relations;

/*	Key_Relation a_key;
	Key_Relation b_key;
	Key_Relation c_key;
	Key_Relation d_key;*/
	
};

#endif
