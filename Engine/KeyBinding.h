#ifndef MODULE_KEY_BINDING_H_
#define MODULE_KEY_BINDING_H_
#include"SDL\include\SDL_scancode.h"
#include"SDL\include\SDL_gamecontroller.h"
#include"SDL\include\SDL_events.h"
#include <string>
#include <vector>

enum DeviceCombinationType {

	NULL_COMB_DEVICE = -1,
	KEYBOARD_AND_MOUSE_COMB_DEVICE,
	CONTROLLER_COMB_DEVICE,
};
enum KeyBindingType {

	NULL_DEVICE=-1,
	KEYBOARD_DEVICE,
	CONTROLLER_AXIS_DEVICE,
	CONTROLLER_BUTTON_DEVICE,
	MOUSE_BUTTON_DEVICE,
	MOUSE_AXIS_DEVICE,

};


struct KeyRelation 
{
	KeyRelation(int event, std::string name, std::string device, KeyBindingType type) : event_value(event), name(name), key_type(type)
	{
		SelectDeviceCombination(device.c_str());
	}
	std::string name = "default";
	KeyBindingType key_type = KeyBindingType::NULL_DEVICE;
	DeviceCombinationType device = DeviceCombinationType::NULL_COMB_DEVICE;
	DeviceCombinationType SelectDeviceCombination(const char* value);
	int event_value;
};

/*
struct KeyboardDevice: public KeyRelation
{
	KeyboardDevice(int type_event, std::string name) :KeyRelation(name), event_scancode(type_event) { key_type = KeyBindingType::KEYBOARD_DEVICE; }
	int event_scancode;
};

struct ControllerAxisDevice : public KeyRelation
{
	ControllerAxisDevice(SDL_GameControllerAxis type_event, std::string name) :KeyRelation(name), event_axis_controller(type_event) { key_type = KeyBindingType::CONTROLLER_AXIS_DEVICE; }
	SDL_GameControllerAxis event_axis_controller;
};

struct ControllerButtonDevice : public KeyRelation
{
	ControllerButtonDevice(SDL_GameControllerButton type_event, std::string name) :KeyRelation(name), event_button_controller(type_event) { key_type = KeyBindingType::CONTROLLER_BUTTON_DEVICE; }
	SDL_GameControllerButton event_button_controller;
};

struct MouseButtonDevice : public KeyRelation
{
	MouseButtonDevice(int type_event, std::string name) :KeyRelation(name), event_button_mouse(type_event) { key_type = KeyBindingType::MOUSE_BUTTON_DEVICE; }
	int event_button_mouse=0;


	#define SDL_BUTTON_LEFT     1
	#define SDL_BUTTON_MIDDLE   2
	#define SDL_BUTTON_RIGHT    3
	
};

struct MouseAxisDevice : public KeyRelation
{
	MouseAxisDevice(SDL_EventType type_event, std::string name) :KeyRelation(name), event_axis_mouse(type_event) { key_type = KeyBindingType::MOUSE_AXIS_DEVICE; }
	SDL_EventType event_axis_mouse;
};*/


class KeyBinding 
{
public:
	KeyBinding();
	~KeyBinding();

	bool InitKeyBinding();
	

	bool CleanUp();

	KeyRelation* FindKeyBinding(const char* name);

private:

	std::vector<KeyRelation> key_binding_relations;

/*	Key_Relation a_key;
	Key_Relation b_key;
	Key_Relation c_key;
	Key_Relation d_key;*/
	
};

#endif
