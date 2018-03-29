#include "AnimableComponent.h"



AnimableComponent::AnimableComponent()
{
}


AnimableComponent::~AnimableComponent()
{
}

void AnimableComponent::SetNewAnimationValue(const AnimationData & value)
{
}

AnimationData AnimableComponent::ShowParameters()
{
	AnimationData ret;
	ret.type = ParameterValue::PARAMETER_NONE;
	return ret;
}

AnimationData::AnimationData()
{

}

AnimationData::~AnimationData()
{
}
