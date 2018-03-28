#ifndef _ANIMABLE_COMPONENT_H_
#define _ANIMABLE_COMPONENT_H_
#include "MathGeoLib.h"
enum ParameterValue
{
	RECT_TRANSFORM_POSITION,
	RECT_TRANSFORM_ROTATION,
	RECT_TRANSFORM_SCALE,
	RECT_TRANSFORM_WIDTH,
	RECT_TRANSFORM_HEIGHT,

};
union AnimationValue
{
	AnimationValue() {}
	~AnimationValue() {}

	ParameterValue type;
	int i_value;
	float f_value;
	double d_value;
	float3 f3_value;
	Quat q_value;

};

class AnimableComponent
{
public:
	AnimableComponent();
	~AnimableComponent();

	virtual void SetNewAnimationValue(const AnimationValue& value);

};

#endif //_MODULE_ANIMATION_H_