#ifndef _ANIMABLE_COMPONENT_H_
#define _ANIMABLE_COMPONENT_H_
#include "MathGeoLib.h"
#include "ResourceMaterial.h"
enum ParameterValue
{
	PARAMETER_NONE = -1,
	RECT_TRANSFORM_POSITION,
	RECT_TRANSFORM_ROTATION,
	RECT_TRANSFORM_SCALE,
	RECT_TRANSFORM_WIDTH,
	RECT_TRANSFORM_HEIGHT,
	IMAGE_ALPHA_VALUE,
};

union AnimationValue
{
	AnimationValue(){}
	~AnimationValue(){}
	int i_value;
	float f_value;
	double d_value;
	float3 f3_value;
	Quat q_value;
	ResourceMaterial* sprite = nullptr;
};
struct AnimationData
{
	AnimationData();
	~AnimationData();

	ParameterValue type;
	AnimationValue value;
};
class AnimableComponent
{
public:
	AnimableComponent();
	~AnimableComponent();

	virtual void SetNewAnimationValue(const AnimationData& value);
	virtual AnimationData ShowParameters();
	virtual const char* ReturnParameterName(ParameterValue parameter);
	virtual AnimationValue GetParameter(ParameterValue parameter);
};

#endif //_MODULE_ANIMATION_H_