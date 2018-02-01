#ifndef _EVENTDEF_
#define _EVENTDEF_

/*----------------------------------------------------------------------------------------------------------*/
/*---Add new events:----------------------------------------------------------------------------------------*/
/*---Add type in EventType enum (in the proper place and alphabetical order (easier to search))-------------*/
/*---Add the new Event struct (in the proper place and alphabetical order (easier to search))---------------*/
/*---In the new Event structure DON'T declare anything (this can be modified in the future)-----------------*/
/*---Add the new Event in the Event union (in the proper place and alphabetical order (easier to search))---*/
/*----------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------*/
/*-----------All forwad declarations here-----------*/
/*--------------------------------------------------*/
class GameObject;

/*--------------------------------------------------*/
/*--------------------Events enum-------------------*/
/*--------------------------------------------------*/
enum EventType
{
	/*----------------------Engine----------------------*/
	DELETE_GO,
	DOCKING_MODIF,
	DRAW,
	DROPPED_FILE,
	TIME_MANAGER,
	WINDOW_RESIZE,
	/*-------------------Audio Engine-------------------*/

	/*------------------Gameplay System-----------------*/

	/*------------------Particle System-----------------*/

	/*----------------------Physics---------------------*/

	/*------------------Shader Pipeline-----------------*/

	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/
	
	MAXEVENTS
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
		DRAW3D,
		DRAW3DAlpha,
		DRAW2D
	};
	DrawType Dtype;
	float DistanceCamToObject;
	GameObject* ToDraw;
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
		PLAY,
		PAUSE,
		UNPAUSE,
		FRAME,
		STOP
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

	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/

};

#endif //_EVENTDEF_