#ifndef _EVENTDEF_
#define _EVENTDEF_

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
	EVENT_CREATE_SHADER,
	EVENT_CREATE_SHADER_PROGRAM,
	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/
	
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


/*--------------------------------------------------*/
/*------------------Shader Pipeline-----------------*/
/*--------------------------------------------------*/
struct ECreateShader
{
	EventType type;
	ShaderType shader_type;
	const char* name;	//std::string?
	//const char* code;	//std::string?
};

struct ECreateShaderProgram
{
	EventType type;
	const char* name;	//std::string?
	//std::queue<int> ShaderObjects;
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
	ECreateShader shader;
	ECreateShaderProgram shaderprogram;
	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/

};

#endif //_EVENTDEF_