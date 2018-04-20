#ifndef EVENTDEF_V2
#define EVENTDEF_V2
#include <vector>
#include <map>
#include "Math\float2.h"
#include "Math\float3.h"

/*-----------------------------------------------------------------------------------------------------------*/
/*---Event System v2.0---------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/
/*---Try to add as least as possible includes----------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/
/*---How to create new event:--------------------------------------------------------------------------------*/
/*-->Add forward declarations for your use in "Forwad Declarations" area-------------------------------------*/
/*-->Add new event enum type with a descriptive name, in the correct system area and carefull because--------*/
/*---events are called by order, so lower enum value types are called before higher enum value types---------*/
/*-->With the same order of the enum, add a struct for your event, with a name starting by E + Event type----*/
/*-->All struct MUST have as FIRST element -> "uint64_t event_data = 0;"-------------------------------------*/
/*-->Now the final step is to add an instance of your struct in the Event union, with the same order---------*/ 
/*---followed in the enum and struct position----------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/
/*---Usage:--------------------------------------------------------------------------------------------------*/
/*-->Inside the structs you can add almost every variable type, but because Event is a Union, you might------*/
/*---find some problems, be careful--------------------------------------------------------------------------*/
/*-->The events store, type, time delay, frame delay and one bool inside the "uint64_t event_data" variable,-*/
/*---this is stored and read with bitwise operators, use the Event union methods to store and read the-------*/
/*---variables. With events first use, initialize using "Set_event_data" or "Set_event_data_f" for saffer----*/
/*---memory management---------------------------------------------------------------------------------------*/
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
	EVENT_CUBEMAP_REQUEST,
	EVENT_CUBEMAP_DRAW,
	EVENT_CREATE_SHADER_PROGRAM,
	EVENT_SEND_ALL_SHADER_OBJECTS,
	EVENT_OPEN_SHADER_EDITOR,
	EVENT_DELETE_LIGHT,
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
	/*------------------Request & Send------------------*/
	EVENT_MIN_REQUEST_SEND,									//Keep this above request & send events, and don't use as an event type
	EVENT_REQUEST_3D_3DA_MM,								//Call it from your code to get a reference to the drawable events multimap
	EVENT_SEND_3D_3DA_MM,									//This event is sent if you call EVENT_REQUEST_3D_3DA_MM with the drawable events multimaps
	EVENT_MAX_REQUEST_SEND,									//Keep this below request & send events, and don't use as an event type
	/*----------------------Engine----------------------*/
	EVENT_DOCKING_MODIF,
	EVENT_DRAW,
	EVENT_DROPPED_FILE,
	EVENT_TIME_MANAGER,
	EVENT_SPAWN_GAMEOBJECT,
	EVENT_WINDOW_RESIZE,
	EVENT_DELETE_GO,										//Keep this event last
	MAXEVENTS												//Keep this at the bottom, needed to know how many events we have
};

/*--------------------------------------------------*/
/*-------------------Audio Engine-------------------*/
/*--------------------------------------------------*/


/*--------------------------------------------------*/
/*------------------Gameplay System-----------------*/
/*--------------------------------------------------*/
struct EScriptDisabled
{
	uint64_t event_data = 0;
	CompScript* script = nullptr;
};

/*--------------------------------------------------*/
/*------------------Particle System-----------------*/
/*--------------------------------------------------*/
struct EParticleDraw
{
	uint64_t event_data = 0;
	Particle* ToDraw = nullptr;
};

/*--------------------------------------------------*/
/*----------------------Physics---------------------*/
/*--------------------------------------------------*/
struct ETrigger
{
	uint64_t event_data = 0;
	JP_COLLISION_TYPE coll_type = (JP_COLLISION_TYPE)0;
	Component* actor0 = nullptr;
	Component* actor1 = nullptr;
	float3 impact_point = float3::zero;
	float3 impact_normal = float3::zero;
};

/*--------------------------------------------------*/
/*------------------Shader Pipeline-----------------*/
/*--------------------------------------------------*/
struct ECubeMapRequest
{
	uint64_t event_data = 0;
	Component* comp_cubemap = nullptr;

};
struct ECubeMapDraw
{
	uint64_t event_data = 0;
	//const std::multimap<uint, Event>* MM3DDrawEvent = nullptr;
	const std::vector<GameObject*>* all_gameobjects = nullptr;
};



struct ECreateShaderProgram
{
	uint64_t event_data = 0;
	const char* name = nullptr;
	Shader* Shader1 = nullptr;
	Shader* Shader2 = nullptr;
};

struct ESendAllShaderObject
{
	uint64_t event_data = 0;
	std::vector<Shader*>* shaders = nullptr;
};

struct EOpenShaderEditor
{
	uint64_t event_data = 0;
	ShaderType shader_type;
	const char* name = nullptr;
	bool open_editor = false;
};

/*--------------------------------------------------*/
/*----------------Skeletal Animation----------------*/
/*--------------------------------------------------*/

/*--------------------------------------------------*/
/*------------------User Interface------------------*/
/*--------------------------------------------------*/
struct EGUIAxis
{
	uint64_t event_data = 0;
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
	uint64_t event_data = 0;
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
	uint64_t event_data = 0;
	bool active = false;
};

struct EPassComponent
{
	uint64_t event_data = 0;
	Component* component = nullptr;
};

struct EPassSelected
{
	uint64_t event_data = 0;
	CompInteractive* component = nullptr;
};

struct EGUISubmit
{
	uint64_t event_data = 0;
	bool active = false;
};

/*--------------------------------------------------*/
/*------------------Request & Send------------------*/
/*--------------------------------------------------*/

struct ERequest3D3DAMM
{
	uint64_t event_data = 0;
	const CompLight* light = nullptr;
};

struct ESend3D3DAMM
{
	uint64_t event_data = 0;
	const std::multimap<uint, Event>* MM3DDrawEvent = nullptr;
	const std::multimap<float, Event>* MM3DADrawEvent = nullptr;
	const CompLight* light = nullptr;
};

struct DeleteLight
{
	uint64_t event_data = 0;	
	CompLight* light = nullptr;
};

/*--------------------------------------------------*/
/*----------------------Engine----------------------*/
/*--------------------------------------------------*/
struct EDockingModif
{
	uint64_t event_data = 0;
	//WIP
};

struct EDraw
{
	uint64_t event_data = 0;
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
	uint64_t event_data = 0;
	const char* FilePath = nullptr;	//std::string?
};

struct ETimeManager
{
	uint64_t event_data = 0;
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

struct ESpawnGO
{
	uint64_t event_data = 0;
	GameObject* Tospawn = nullptr;
};

struct EWindowResize
{
	uint64_t event_data = 0;
	unsigned int w = 0;
	unsigned int h = 0;
};

struct EDeleteGO
{
	uint64_t event_data = 0;
	GameObject* Todelte = nullptr;
	uint uuid = 0;
};

/*--------------------------------------------------*/
/*----------------------Event-----------------------*/
/*--------------------------------------------------*/
union Event
{
	Event() {}
	~Event() {}

	//Setters
	inline void Set_event_data(EventType type, unsigned int FrameDelay = 0, unsigned int TimeDelay = 0);							//Initialize event_data with time delay as miliseconds uint
	inline void Set_event_data_f(EventType type, unsigned int FrameDelay = 0, float TimeDelay = 0.0f);								//Initialize event_data with time delay as seconds float
	inline void Set_event_data_type(EventType type);																				//When the event is initialized, you can change type with this method
	inline void Set_event_data_frame_delay(unsigned int FrameDelay);																//When the event is initialized, you can change frame delay with this method
	inline void Set_event_data_time_delay(unsigned int TimeDelay);																	//When the event is initialized, you can change time delay as miliseconds with this method
	inline void Set_event_data_time_delay_f(float TimeDelay);																		//When the event is initialized, you can change time delay as seconds with this method
	//inline void Set_event_data_PushedWhileIteriting(bool PushedWhileIteriting);													//DO NOT USE, This is to set the bool, used by Event System for correct event management
	//Getters
	inline void Get_event_data(EventType& type, unsigned int& FrameDelay, unsigned int& TimeDelay/*, bool& PushedWhileIteriting*/);	//Get all event_data variables, time delay as miliseconds uint
	inline void Get_event_data_f(EventType& type, unsigned int& FrameDelay, float& TimeDelay/*, bool& PushedWhileIteriting*/);		//Get all event_data variables, time delay as seconds float
	inline void Get_event_data_type(EventType& type);																				//Get event type by reference
	inline EventType Get_event_data_type();																							//Get event type as return value
	inline void Get_event_data_frame_delay(unsigned int& FrameDelay);																//Get event frame delay by reference
	inline unsigned int Get_event_data_frame_delay();																				//Get event frame delay as return value
	inline void Get_event_data_time_delay(unsigned int& TimeDelay);																	//Get event time delay as miliseconds uint by reference
	inline unsigned int Get_event_data_time_delay();																				//Get event time delay as miliseconds uint as return value
	inline void Get_event_data_time_delay_f(float& TimeDelay);																		//Get event time delay as seconds float by reference
	inline float Get_event_data_time_delay_f();																						//Get event time delay as seconds float as return value
	//inline void Get_event_data_PushedWhileIteriting(bool& PushedWhileIteriting);													//DO NOT USE, This is to get the bool by refernce, used by Event System for correct event management
	//inline bool Get_event_data_PushedWhileIteriting();																			//DO NOT USE, This is to get the bool as return value, used by Event System for correct event management

	/*--------------------Event Data--------------------*/
	uint64_t event_data = 0;

	/*-------------------Audio Engine-------------------*/


	/*------------------Gameplay System-----------------*/
	EScriptDisabled script_disabled;

	/*------------------Particle System-----------------*/
	EParticleDraw particle_draw;

	/*----------------------Physics---------------------*/
	ETrigger trigger;

	/*------------------Shader Pipeline-----------------*/
	ECubeMapRequest	cube_map_request;
	ECubeMapDraw	cube_map_draw;
	ECreateShaderProgram shader_program;
	ESendAllShaderObject send_shader_object;
	EOpenShaderEditor shader_editor;
	ERequest3D3DAMM request_3d3damm;
	ESend3D3DAMM send_3d3damm;
	DeleteLight delete_light;
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
	ESpawnGO spawnGO;
	EWindowResize window_resize;
	EDeleteGO delete_go;
};

inline void Event::Set_event_data(EventType type, unsigned int FrameDelay, unsigned int TimeDelay)
{
	Set_event_data_type(type);
	Set_event_data_frame_delay(FrameDelay);
	Set_event_data_time_delay(TimeDelay);
	//Set_event_data_PushedWhileIteriting(false);
	event_data = event_data | (uint64_t)false;
}

inline void Event::Set_event_data_f(EventType type, unsigned int FrameDelay, float TimeDelay)
{
	Set_event_data(type, FrameDelay, (unsigned int)(TimeDelay * 1000.0f));
}

inline void Event::Set_event_data_type(EventType type)
{
	event_data = (uint64_t)type << 48;
}

inline void Event::Set_event_data_frame_delay(unsigned int FrameDelay)
{
	event_data = event_data | ((uint64_t)FrameDelay << 32);
}

inline void Event::Set_event_data_time_delay(unsigned int TimeDelay)
{
	event_data = event_data | ((uint64_t)TimeDelay << 16);
}

inline void Event::Set_event_data_time_delay_f(float TimeDelay)
{
	Set_event_data_time_delay((unsigned int)(TimeDelay * 1000.0f));
}

/*
inline void Event::Set_event_data_PushedWhileIteriting(bool PushedWhileIteriting)
{
	event_data = event_data | (uint64_t)PushedWhileIteriting;
}
*/

inline void Event::Get_event_data(EventType& type, unsigned int& FrameDelay, unsigned int& TimeDelay/*, bool& PushedWhileIteriting*/)
{
	Get_event_data_type(type);
	Get_event_data_frame_delay(FrameDelay);
	Get_event_data_time_delay(TimeDelay);
	//Get_event_data_PushedWhileIteriting(PushedWhileIteriting);
}

inline void Event::Get_event_data_f(EventType& type, unsigned int& FrameDelay, float& TimeDelay/*, bool& PushedWhileIteriting*/)
{
	unsigned int TimeDlayMS = 0;
	Get_event_data(type, FrameDelay, TimeDlayMS/*, PushedWhileIteriting*/);
	TimeDelay = (float)TimeDlayMS / 1000.0f;
}

inline void Event::Get_event_data_type(EventType& type)
{
	type = (EventType)((event_data & 0xFFFF000000000000) >> 48);
}

inline EventType Event::Get_event_data_type()
{
	return (EventType)((event_data & 0xFFFF000000000000) >> 48);
}

inline void Event::Get_event_data_frame_delay(unsigned int& FrameDelay)
{
	FrameDelay = (event_data & 0x0000FFFF00000000) >> 32;
}

inline unsigned int Event::Get_event_data_frame_delay()
{
	return (event_data & 0x0000FFFF00000000) >> 32;
}

inline void Event::Get_event_data_time_delay(unsigned int& TimeDelay)
{
	TimeDelay = (event_data & 0x00000000FFFF0000) >> 16;
}

inline unsigned int Event::Get_event_data_time_delay()
{
	return (event_data & 0x00000000FFFF0000) >> 16;
}

inline void Event::Get_event_data_time_delay_f(float& TimeDelay)
{
	unsigned int TimeDlayMS = 0;
	Get_event_data_time_delay(TimeDlayMS);
	TimeDelay = (float)TimeDlayMS / 1000.0f;
}

inline float Event::Get_event_data_time_delay_f()
{
	unsigned int TimeDlayMS = 0;
	Get_event_data_time_delay(TimeDlayMS);
	return (float)TimeDlayMS / 1000.0f;
}

/*
inline void Event::Get_event_data_PushedWhileIteriting(bool& PushedWhileIteriting)
{
	PushedWhileIteriting = (event_data & 0x000000000000FFFF);
}

inline bool Event::Get_event_data_PushedWhileIteriting()
{
	return (event_data & 0x000000000000FFFF);
}
*/

#endif //EVENTDEF_V2