#ifndef _EVENTDEF_
#define _EVENTDEF_
#include<vector>
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

/*--------------------------------------------------*/
/*--------------------Events enum-------------------*/
/*--------------------------------------------------*/
enum EventType
{
	EVENT_UNKNOWN = -1,
	/*----------------------Engine----------------------*/
	EVENT_DELETE_GO,
	EVENT_DOCKING_MODIF,
	EVENT_DRAW,
	EVENT_DROPPED_FILE,
	EVENT_TIME_MANAGER,
	EVENT_WINDOW_RESIZE,
	/*-------------------Audio Engine-------------------*/

	/*------------------Gameplay System-----------------*/

	/*------------------Particle System-----------------*/

	/*----------------------Physics---------------------*/

	/*------------------Shader Pipeline-----------------*/
	EVENT_CREATE_SHADER_PROGRAM,
	EVENT_SEND_ALL_SHADER_OBJECTS,
	EVENT_OPEN_SHADER_EDITOR,

	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/
	EVENT_BUTTON_DOWN,
	EVENT_BUTTON_UP,
	EVENT_MOUSE_MOTION,
	EVENT_PASS_COMPONENT,
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

};
struct EPassComponent
{
	EventType type;
	Component* component = nullptr;

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
	ESendAllShaderObject() {}
	EventType type= EventType::EVENT_SEND_ALL_SHADER_OBJECTS;
	
	//This vector makes std pair of the cpp errors
	//need to fix this
	std::vector<Shader*>* shaders;
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

/*--------------------------------------------------*/
/*----------------------Event-----------------------*/
/*--------------------------------------------------*/
union Event
{
	Event() {};
	~Event() {};
	EventType type;
	/*----------------------Engine----------------------*/
	EDeleteGO deletego;
	EDockingModif dockmodif;
	EDraw draw;
	EDroppedFile filedrop;
	ETimeManager time;
	EWindowResize windowresize;
	/*-------------------Audio Engine-------------------*/

	/*------------------Gameplay System-----------------*/

	/*------------------Particle System-----------------*/

	/*----------------------Physics---------------------*/

	/*------------------Shader Pipeline-----------------*/
	ECreateShaderProgram shaderprogram;
	ESendAllShaderObject sendshaderobject;
	EOpenShaderEditor shadereditor;
	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/
	EPoint pointer;
	EPassComponent pass_component;
};

#endif //_EVENTDEF_