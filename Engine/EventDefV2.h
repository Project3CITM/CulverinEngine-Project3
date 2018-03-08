#ifndef EVENTDEF_V2
#define EVENTDEF_V2
#include <vector>
#include <map>
#include "Math\float2.h"

/*-----------------------------------------------------------------------------------------------------------*/
/*---Event System v2.0---------------------------------------------------------------------------------------*/
/*---TODO: EventDefV2.h Guide--------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------*/
/*----------------Forwad Declarations---------------*/
/*--------------------------------------------------*/
class Component;
class CompInteractive;
class CompLight;
class CompScript;
class GameObject;
class Shader;
class Particle;
enum ShaderType;
enum JP_COLLISION_TYPE;
union Event;

/*--------------------------------------------------*/
/*--------------------Events enum-------------------*/
/*--------------------------------------------------*/
enum EventType
{
	EVENT_UNKNOWN = -1,
	/*-------------------Audio Engine-------------------*/

	/*------------------Gameplay System-----------------*/
	EVENT_SCRIPT_DISABLED,
	/*------------------Particle System-----------------*/
	EVENT_PARTICLE_DRAW,
	/*----------------------Physics---------------------*/
	EVENT_TRIGGER_COLLISION,
	/*------------------Shader Pipeline-----------------*/
	EVENT_CREATE_SHADER_PROGRAM,
	EVENT_SEND_ALL_SHADER_OBJECTS,
	EVENT_OPEN_SHADER_EDITOR,
	EVENT_REQUEST_3D_3DA_MM,		//Call it from your code to get a reference to the drawable events multimap
	EVENT_SEND_3D_3DA_MM,			//This event is sent if you call EVENT_REQUEST_3D_3DA_MM with the drawable events multimap
	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/
	EVENT_AXIS,
	EVENT_BUTTON_DOWN,
	EVENT_BUTTON_UP,
	EVENT_CANCEL,
	EVENT_MOUSE_MOTION,
	EVENT_PASS_COMPONENT,
	EVENT_PASS_SELECTED,
	EVENT_SUBMIT,
	/*----------------------Engine----------------------*/
	EVENT_DOCKING_MODIF,
	EVENT_DRAW,
	EVENT_DROPPED_FILE,
	EVENT_TIME_MANAGER,
	EVENT_WINDOW_RESIZE,
	EVENT_DELETE_GO,				//Keep this event last
	MAXEVENTS						//Keep this at the bottom, needed to know how many events se have
};

/*--------------------------------------------------*/
/*----------------Events Data struct----------------*/
/*--------------------------------------------------*/
struct EventData
{
	EventType type = EventType::EVENT_UNKNOWN;
	float delay = 0.0f;
	enum Consumability
	{
		CONSUMABILITY_CONSUMABLE,
		CONSUMABILITY_UNCONSUMABLE,
		CONSUMABILITY_CONSUMED
	};
	Consumability button = Consumability::CONSUMABILITY_UNCONSUMABLE;
};

/*--------------------------------------------------*/
/*-------------------Audio Engine-------------------*/
/*--------------------------------------------------*/

/*--------------------------------------------------*/
/*------------------Gameplay System-----------------*/
/*--------------------------------------------------*/
struct EScriptDisabled
{
	EventData event_data;
	CompScript* script = nullptr;
};

/*--------------------------------------------------*/
/*------------------Particle System-----------------*/
/*--------------------------------------------------*/
struct EParticleDraw
{
	EventData event_data;
	Particle* ToDraw = nullptr;
};

/*--------------------------------------------------*/
/*----------------------Physics---------------------*/
/*--------------------------------------------------*/
struct ETrigger
{
	EventData event_data;
	JP_COLLISION_TYPE collision_type;
	Component* trigger = nullptr;
	Component* actor = nullptr;
};

/*--------------------------------------------------*/
/*------------------Shader Pipeline-----------------*/
/*--------------------------------------------------*/
struct ECreateShaderProgram
{
	EventData event_data;
	const char* name = nullptr;	//std::string?
	Shader* Shader1 = nullptr;
	Shader* Shader2 = nullptr;
};

struct ESendAllShaderObject
{
	EventData event_data;
	std::vector<Shader*>* shaders = nullptr;
};

struct EOpenShaderEditor
{
	EventData event_data;
	ShaderType shader_type;
	const char* name = nullptr;
	bool open_editor = false;
};

struct ERequest3D3DAMM
{
	EventData event_data;
	const CompLight* light = nullptr;
};

struct ESend3D3DAMM
{
	EventData event_data;
	const std::multimap<float, Event>* MM3DDrawEvent = nullptr;
	const std::multimap<float, Event>* MM3DADrawEvent = nullptr;
	const CompLight* light = nullptr;
};

/*--------------------------------------------------*/
/*----------------Skeletal Animation----------------*/
/*--------------------------------------------------*/

/*--------------------------------------------------*/
/*------------------User Interface------------------*/
/*--------------------------------------------------*/
struct EGUIAxis
{
	EventData event_data;
	enum Direction
	{
		DIRECTION_NONE,

		DIRECTION_UP,
		DIRECTION_DOWN,
		DIRECTION_RIGHT,
		DIRECTION_LEFT,
	};
	Direction direction = Direction::DIRECTION_NONE;
	float value = 0.0f;
};

struct EPoint
{
	EventData event_data;
	enum InputButton
	{
		INPUT_NONE = -1,
		INPUT_MOUSE_LEFT,
		INPUT_MOUSE_RIGHT,
		INPUT_MOUSE_MIDDLE,
		INPUT_MAX
	};
	InputButton button;
	float2 position = float2::zero;
	float2 motion = float2::zero;
	GameObject* focus = nullptr;
};

struct EGUICancel
{
	EventData event_data;
	bool active = false;
};

struct EPassComponent
{
	EventData event_data;
	Component* component = nullptr;
};

struct EPassSelected
{
	EventData event_data;
	CompInteractive* component = nullptr;
};

struct EGUISubmit
{
	EventData event_data;
	bool active = false;
};

/*--------------------------------------------------*/
/*----------------------Engine----------------------*/
/*--------------------------------------------------*/
struct EDockingModif
{
	EventData event_data;
	//WIP
};

struct EDraw
{
	EventData event_data;
	enum DrawType
	{
		DRAW_3D,			//3D Game Objects without alpha
		DRAW_3D_ALPHA,		//3D Game Objects with alpha & World space canvas
		DRAW_2D,			//2D GameObject / game object you want to draw with orthogonal perspective
		DRAW_WORLD_CANVAS,	//World space canvas
		DRAW_SCREEN_CANVAS	//Screen space canvas
	};
	DrawType Dtype;
	Component* ToDraw = nullptr;
};

struct EDroppedFile
{
	EventData event_data;
	const char* FilePath = nullptr;	//std::string?
};

struct ETimeManager
{
	EventData event_data;
	enum TimeEvent
	{
		TIME_PLAY,
		TIME_PAUSE,
		TIME_UNPAUSE,
		TIME_FRAME,
		TIME_STOP
	};
	TimeEvent time = TimeEvent::TIME_STOP;
};

struct EWindowResize
{
	EventData event_data;
	unsigned int w = 0;
	unsigned int h = 0;
};

struct EDeleteGO
{
	EventData event_data;
	GameObject* Todelte = nullptr;
};

/*--------------------------------------------------*/
/*----------------------Event-----------------------*/
/*--------------------------------------------------*/
union Event
{
	Event() {}
	~Event() {}

	/*----------------Event Data Struct-----------------*/
	EventData event_data;

	/*-------------------Audio Engine-------------------*/

	/*------------------Gameplay System-----------------*/
	EScriptDisabled script_disabled;

	/*------------------Particle System-----------------*/
	EParticleDraw particle_draw;

	/*----------------------Physics---------------------*/
	ETrigger trigger;

	/*------------------Shader Pipeline-----------------*/
	ECreateShaderProgram shader_program;
	ESendAllShaderObject send_shader_object;
	EOpenShaderEditor shader_editor;
	ERequest3D3DAMM request_3d3damm;
	ESend3D3DAMM send_3d3damm;

	/*----------------Skeletal Animation----------------*/

	/*------------------User Interface------------------*/
	EGUIAxis gui_axis;
	EPoint pointer;
	EGUICancel gui_cancel;
	EPassComponent pass_component;
	EPassSelected pass_selected;
	EGUISubmit gui_submit;

	/*----------------------Engine----------------------*/
	EDockingModif dock_modif;
	EDraw draw;
	EDroppedFile file_drop;
	ETimeManager time;
	EWindowResize window_resize;
	EDeleteGO delete_go;

};

#endif //EVENTDEF_V2
/**/