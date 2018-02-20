#ifndef _EVENTDEF_
#define _EVENTDEF_
#include <vector>
#include <map>
#include "Math\float2.h"
/*-----------------------------------------------------------------------------------------------------------*/
/*---Add new events:-----------------------------------------------------------------------------------------*/
/*---Add type in EventType enum (in the proper place and alphabetical order (easier to search))--------------*/
/*---Add the new Event struct (in the proper place and alphabetical order (easier to search))----------------*/
/*---Its mandatory to put "EventType type;" at the begining of every Event struct----------------------------*/
/*---In the new Event structure DON'T DEFINE ANYTHING (this can be modified in the future)-------------------*/
/*---Add the new Event in the Event union (in the proper place and alphabetical order (easier to search))----*/
/*---If you are testing and don't put in the proper place and alphabetical order the events copy&paste this:-*/
/*---(Not ordered)-------------------------------------------------------------------------------------------*/
/*---When the time comes, someone must order them :)---------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------*/
/*-----------All forwad declarations here-----------*/
/*--------------------------------------------------*/
class GameObject;
class Component;
enum ShaderType;
class Shader;
union Event;
class CompLight;
enum JP_COLLISION_TYPE;
/*--------------------------------------------------*/
/*--------------------Events enum-------------------*/
/*--------------------------------------------------*/
enum EventType
{
	EVENT_UNKNOWN = -1,
	/*----------------------Engine----------------------*/
	EVENT_DOCKING_MODIF,
	EVENT_DRAW,
	EVENT_DROPPED_FILE,
	EVENT_TIME_MANAGER,
	EVENT_WINDOW_RESIZE,
	/*-------------------Audio Engine-------------------*/

	/*------------------Gameplay System-----------------*/

	/*------------------Particle System-----------------*/

	/*----------------------Physics---------------------*/
	EVENT_TRIGGER_COLLISION,
	/*------------------Shader Pipeline-----------------*/
	EVENT_CREATE_SHADER_PROGRAM,
	EVENT_SEND_ALL_SHADER_OBJECTS,
	EVENT_REQUEST_3D_3DA_MM, //Call it from your code to get a reference to the multimaps
	EVENT_SEND_3D_3DA_MM,	 //This event is sent if you call EVENT_REQUEST_3D_3DA_MM
	EVENT_OPEN_SHADER_EDITOR,
	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/
	EVENT_BUTTON_DOWN,
	EVENT_BUTTON_UP,
	EVENT_MOUSE_MOTION,
	EVENT_PASS_COMPONENT,

	EVENT_DELETE_GO, //Keep this event last
	MAXEVENTS//Keep this at the bottom, needed to know how many events se have
};

/*--------------------------------------------------*/
/*----------------------Engine----------------------*/
/*--------------------------------------------------*/
struct EDeleteGO
{
	EventType type;
	GameObject* Todelte;
};

struct EDockingModif
{
	EventType type;
	//WIP
};

struct EDraw
{
	EventType type;
	enum DrawType
	{
		DRAW_3D,			//3D Game Objects without alpha
		DRAW_3D_ALPHA,		//3D Game Objects with alpha & World space canvas
		DRAW_2D,			//2D GameObject / game object you want to draw with orthogonal perspective
		DRAW_WORLD_CANVAS,	//World space canvas
		DRAW_SCREEN_CANVAS	//Screen space canvas
	};
	DrawType Dtype;
	//float DistanceCamToObject;
	Component* ToDraw;
};

struct EDroppedFile
{
	EventType type;
	const char* FilePath;	//std::string?
};

struct ETimeManager
{
	EventType type;
	enum TimeEvent
	{
		TIME_PLAY,
		TIME_PAUSE,
		TIME_UNPAUSE,
		TIME_FRAME,
		TIME_STOP
	};
	TimeEvent time;
};

struct EWindowResize
{
	EventType type;
	uint w;
	uint h;
};

/*--------------------------------------------------*/
/*-------------------Audio Engine-------------------*/
/*--------------------------------------------------*/


/*--------------------------------------------------*/
/*------------------Gameplay System-----------------*/
/*--------------------------------------------------*/


/*--------------------------------------------------*/
/*------------------Particle System-----------------*/
/*--------------------------------------------------*/


/*--------------------------------------------------*/
/*----------------------Physics---------------------*/
/*--------------------------------------------------*/
struct ETrigger
{
	EventType type;
	JP_COLLISION_TYPE collision_type;
	Component* trigger;
	Component* actor;
};


/*--------------------------------------------------*/
/*------------------Shader Pipeline-----------------*/
/*--------------------------------------------------*/
struct ECreateShaderProgram
{
	EventType type;
	const char* name;	//std::string?
	Shader* Shader1;
	Shader* Shader2;
};

struct ESendAllShaderObject
{
	EventType type;
	std::vector<Shader*>* shaders;
};

struct ERequest3D3DAMM
{
	EventType type;
	const CompLight* light;
};

struct ESend3D3DAMM
{
	EventType type;
	const std::multimap<float, Event>* MM3DDrawEvent;
	const std::multimap<float, Event>* MM3DADrawEvent;
	const CompLight* light;
};

struct EOpenShaderEditor
{
	EventType type;
	ShaderType shader_type;
	const char* name;
	bool open_editor;
};

/*--------------------------------------------------*/
/*----------------Skeletal Animation----------------*/
/*--------------------------------------------------*/


/*--------------------------------------------------*/
/*------------------User Interface------------------*/
/*--------------------------------------------------*/
struct EPoint
{
	EventType type;
	enum InputButton
	{
		INPUT_NONE = -1,
		INPUT_MOUSE_LEFT,
		INPUT_MOUSE_RIGHT,
		INPUT_MOUSE_MIDDLE,
		INPUT_MAX
	};
	InputButton button;
	float2 position;
	float2 motion;
	GameObject* focus = nullptr;
};
struct EPassComponent
{
	EventType type;
	Component* component = nullptr;

};
/*--------------------------------------------------*/
/*----------------------Event-----------------------*/
/*--------------------------------------------------*/
union Event
{
	Event() {}
	~Event() {}
	EventType type;
	/*----------------------Engine----------------------*/
	EDeleteGO delete_go;
	EDockingModif dock_modif;
	EDraw draw;
	EDroppedFile file_drop;
	ETimeManager time;
	EWindowResize window_resize;
	/*-------------------Audio Engine-------------------*/

	/*------------------Gameplay System-----------------*/

	/*------------------Particle System-----------------*/

	/*----------------------Physics---------------------*/
	ETrigger physics_collision;
	/*------------------Shader Pipeline-----------------*/
	ECreateShaderProgram shader_program;
	ESendAllShaderObject send_shader_object;
	ERequest3D3DAMM request_3d3damm;
	ESend3D3DAMM send_3d3damm;
	EOpenShaderEditor shader_editor;
	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/
	EPoint pointer;
	EPassComponent pass_component;
};

#endif //_EVENTDEF_