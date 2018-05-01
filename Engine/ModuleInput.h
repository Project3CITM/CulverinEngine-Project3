#ifndef _MODULEINPUT_
#define _MODULEINPUT_

#include "Module.h"
#include "Globals.h"
#include "parson.h"
#include "KeyBinding.h"

#define MAX_MOUSE_BUTTONS 5
#define MAX_CONTROLLER_BUTTONS 15
class InputManager;
class PlayerActions;
struct GamePad
{
	bool Empty();
	void Clear();
	_SDL_GameController* controller = nullptr;
	SDL_Joystick* joystick = NULL;
	SDL_Haptic* haptic = NULL;
	int id = -1;
};
enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};
/*struct Controller
{
	KEY_STATE				controller_buttons[MAX_CONTROLLER_BUTTONS];
	uint					right_joystick_dir[4];
	uint					left_joystick_dir[4];
	_SDL_GameController*	pad = nullptr;
};*/
struct KeyRelation;
class KeyBinding;
class ModuleInput : public Module
{
public:
	ModuleInput(bool start_enabled = true);
	~ModuleInput();

	bool Init(JSON_Object* node);
	//bool Start();
	update_status PreUpdate(float dt);
	//update_status Update(float dt);
	//update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool CleanUp();

	void UIInputManagerUpdate(float dt);

	int GetMouseXMotionNormalized();
	int GetMouseYMotionNormalized();
	SDL_Scancode GetKeyFromName(const char* name);

	KEY_STATE GetKey(int id) const
	{
		if (id < 300 && id >= 0)
		{
			return keyboard[id];
		}
		return KEY_STATE::KEY_IDLE;
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	int GetMouseXGlobal() const
	{
		return mouse_x_global;
	}

	int GetMouseYGlobal() const
	{
		return mouse_y_global;
	}

	int GetMouseXMotionGlobal() const
	{
		return mouse_x_motion_global;
	}

	int GetMouseYMotionGlobal() const
	{
		return mouse_y_motion_global;
	}
	bool GetPressAnyKey()const
	{
		return press_any_key;
	}
	void SetMotionGlobal(int x, int y)
	{
		mouse_x_global = x;
		mouse_y_global = y;
	}
	bool GetUpdateNewDevice()const;
	void SavePlayerAction()const;
	void LoadDefaultPlayerAction();
	DeviceCombinationType GetActualDeviceCombo()const;
	void UpdateDeviceType(DeviceCombinationType actual_player_action);
	void RumblePlay(float intensity, int milliseconds);
	_SDL_GameController* GetGameController()const;
	SDL_Joystick* GetJoystick() const;
	SDL_Haptic* GetHaptic() const;
	KeyRelation* FindKeyBinding(const char* string);

	KeyRelation * FindKeyBinding(DeviceCombinationType device, int event_value);

private:
	bool ConnectGameController();
	void SetLastInputEvent(int value, bool nega=false);
public:
	bool quit = false;

private:
	struct LastInput
	{
		void SetLastInputEvent(int value, bool negative = false);
		bool CheckLastInputEqual(int value, bool negative_key = false);
	public:
		int last_input_event = 0;
		bool negative = false;
		bool can_block = false;
	};
	KeyBinding* key_binding = nullptr;
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x = 0;
	int mouse_y = 0;
	int mouse_z = 0;
	int mouse_x_motion = 0;
	int mouse_y_motion = 0;

	bool chek = false;
	bool press_any_key = false;
	int mouse_x_global = 0;
	int mouse_y_global = 0;
	int mouse_x_motion_global = 0;
	int mouse_y_motion_global = 0;

	std::string ui_input_manager;
	std::string submit;
	std::string cancel;
	std::string vertical;
	std::string horizontal;
	float ui_input_update = 0.0f;
	float current_time_ui_input_update = 0.0f;
	bool ui_conected = false;
	bool update_new_device = false;
	bool rumble_active = true;
	DeviceCombinationType actual_device_combo = DeviceCombinationType::KEYBOARD_AND_MOUSE_COMB_DEVICE;
	LastInput last_input;

public:
	std::list<const char*> dropedfiles;
	GamePad gamepad;
	PlayerActions* player_action = nullptr;
	InputManager* ui_manager = nullptr;
};

#endif