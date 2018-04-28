#ifndef COMPONENT_INTERACTIVE_H
#define COMPONENT_INTERACTIVE_H
#include "Component.h"
#include <list>
#include "Math\float4.h"
#include "Math\float2.h"
#include "EventDefV2.h"

class CompGraphic;
class CompImage;
class ResourceMaterial;
class CompInteractive;
struct Navigation
{

	enum NavigationMode
{
	NAVIGATION_NONE,
	NAVIGATION_EXTRICTE,
	NAVIGATION_AUTOMATIC
};
	NavigationMode current_navigation_mode = NavigationMode::NAVIGATION_NONE;
	CompInteractive* interactive_up = nullptr;
	int inteactive_up_uid = 0;
	CompInteractive* interactive_down = nullptr;
	int inteactive_down_uid = 0;
	CompInteractive* interactive_right = nullptr;
	int inteactive_right_uid = 0;
	CompInteractive* interactive_left = nullptr;
	int inteactive_left_uid = 0;
	std::vector<CompInteractive*> assigned_to_me;

};

class CompInteractive:public Component 
{
public:
	CompInteractive(Comp_Type t, GameObject* parent);
	CompInteractive(const CompInteractive& copy, GameObject* parent);

	~CompInteractive();

	bool Enable();

	bool Disable();


	void PreUpdate(float dt);
	void Update(float dt);
	void Clear();
	void ShowOptions();
	void CopyValues(const CompInteractive * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void SyncComponent(GameObject* sync_parent);
	bool IsActivate()const;
	bool IsDragrable() const;
	void Activate();

	bool IsInteractiveEnabled() const;
	

	void Deactive();
	void NavigationRemove(CompInteractive* to_remove);
	virtual void ForceClear();
	void TryConversion();
	void ShowNavigationInfo();
	Component* ShowInteractiveWindow();
	CompInteractive* FindNavigationOnUp();

	CompInteractive* FindNavigationOnDown();
	CompInteractive* FindNavigationOnRight();
	CompInteractive* FindNavigationOnLeft();
	CompInteractive* FindInteractive(float3 direction);

	virtual void OnPointDown(Event event_input);
	virtual void OnPointUP(Event event_input);
	virtual void OnPointEnter(Event event_input);
	virtual void OnPointExit(Event event_input);
	virtual void OnInteractiveSelected(Event event_input);
	virtual void OnInteractiveUnSelected(Event event_input);
	virtual void OnDrag(Event event_input);
	virtual void OnMove(Event event_data);
	virtual void OnSubmit(Event event_data);
	virtual void OnCancel(Event event_data);

	bool PointerInside(float2 position);
	void SetTargetGraphic(CompGraphic* target_graphic);
	//Setters Color tint parameters
	void SetNormalColor(const float4& set_rgba);
	void SetNormalColor(float set_r, float set_g, float set_b, float set_a);
	void SetHighlightedColor(const float4& set_rgba);
	void SetHighlightedColor(float set_r, float set_g, float set_b, float set_a);
	void SetPressedColor(const float4& set_rgba);
	void SetPressedColor(float set_r, float set_g, float set_b, float set_a);
	void SetDisabledColor(const float4& set_rgba);
	void SetDisabledColor(float set_r, float set_g, float set_b, float set_a);
	/*Setter for Interactivity*/
	void SetInteractivity(bool enable);
	//Getters Sprite Swap  parameters
	void SetHighlightedSprite(ResourceMaterial* set_sprite);
	void SetPressedSprite(ResourceMaterial* set_sprite);
	void SetDisabledSprite(ResourceMaterial* set_sprite);
	//Getters Color tint parameters
	float4 GetNormalColor()const;
	float4 GetHighlightedColor()const;
	float4 GetPressedColor()const;
	float4 GetDisabledColor()const;
	//Getters Sprite Swap  parameters
	ResourceMaterial* GetHighligtedSprite()const;
	ResourceMaterial* GetPressedSprite()const;
	ResourceMaterial* GetDisabledSprite()const;
	//OtherGetters
	Navigation::NavigationMode GetNavigationMode()const;
	enum SelectionStates
	{
		STATE_NONE = -1,
		STATE_NORMAL,
		STATE_HIGHLIGHTED,
		STATE_PRESSED,
		STATE_DISABLED,
		STATE_MAX
	};
	enum Transition
	{
		TRANSITION_NONE = -1,
		TRANSITION_COLOR,
		TRANSITION_SPRITE,
		TRANSITION_ANIMATION,
		TRANSITION_MAX
	};
protected:
	virtual bool IsPressed();
	virtual bool IsHighlighted(Event event_data);
	virtual void UpdateSelectionState(Event event_data);
	void ShowInspectorColorTransition();
	void ShowInspectorSpriteTransition();
	void ShowInspectorAnimationTransition();

	void PrepareHandleTransition();
	void HandleTransition(SelectionStates selection_state);

private:
	void Navigate(Event event_data, CompInteractive* interactive);
	void StartTransitionColor(float4 color_to_change, bool no_fade);
	void UpdateTransitionColor(float dt);
	void StartTransitionSprite(ResourceMaterial* sprite_to_change);
	void StartAnimationSprite();

public:

	

private:
	static std::list<CompInteractive*> iteractive_list;
	uint nav_selected;
	bool select_interactive = false;

protected:
	SelectionStates current_selection_state = STATE_NORMAL;
	Transition current_transition_mode = TRANSITION_COLOR;
	Navigation navigation;
	//Color tint parameters
	float4 normal_color = float4::one;
	float4 highlighted_color = float4::one;
	float4 pressed_color = float4::one;
	float4 disabled_color = float4::one;
	float4 desired_color = float4::zero;
	float color_multiply = 1.0f;
	float fade_duration = 0.1f;
	bool no_fade = false;
	bool start_transition = false;

	//Sprite Swap parameters
	ResourceMaterial* sprite[3];
	uint uuid_reimported_sprite[3];
	CompGraphic* target_graphic = nullptr;
	uint target_graphic_uid = 0;
	CompImage* image = nullptr;
	int sprite_value = -1;
	bool select_sprite = false;

	//Animation parameters
	SelectionStates to_anim = STATE_NORMAL;
	CompImage* image_state[3];
	//State values

	bool disabled = false;
	bool point_down = false;
	bool point_inside = false;
	bool interactive_selected = false;
	bool dragable = false;

	bool interactive_enabled = true;

private:
	
	Event component_event;
};
#endif // !COMPONENT_INTERACTIVE_H



