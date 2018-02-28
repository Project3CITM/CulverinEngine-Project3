#ifndef MODULE_KEY_BINDING_H_
#define MODULE_KEY_BINDING_H_
#include "Module.h"
#include"SDL\include\SDL_scancode.h"
#include"SDL\include\SDL_gamecontroller.h"
#include"SDL\include\SDL_events.h"

struct Key_Relation 
{
	Key_Relation(std::string name) : name(name) {}
	std::string name = "";
};

struct Keyboard_Device: public Key_Relation
{
	Keyboard_Device(SDL_Scancode type_event, std::string name) :Key_Relation(name), event_scancode(type_event) {}
	SDL_Scancode event_scancode;
};

struct Controller_Axis_Device : public Key_Relation
{
	Controller_Axis_Device(SDL_GameControllerAxis type_event, std::string name) :Key_Relation(name), event_axis_controller(type_event) {}
	SDL_GameControllerAxis event_axis_controller;
};

struct Controller_Button_Device : public Key_Relation
{
	Controller_Button_Device(SDL_GameControllerButton type_event, std::string name) :Key_Relation(name), event_button_controller(type_event) {}
	SDL_GameControllerButton event_button_controller;
};

struct Mouse_Button_Device : public Key_Relation
{
	Mouse_Button_Device(int type_event, std::string name) :Key_Relation(name), event_button_mouse(type_event) {}
	int event_button_mouse=0;

	/*
	#define SDL_BUTTON_LEFT     1
	#define SDL_BUTTON_MIDDLE   2
	#define SDL_BUTTON_RIGHT    3
	*/
};

struct Mouse_Axis_Device : public Key_Relation
{
	Mouse_Axis_Device(SDL_EventType type_event, std::string name) :Key_Relation(name), event_axis_mouse(type_event) {}
	SDL_EventType event_axis_mouse;
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
