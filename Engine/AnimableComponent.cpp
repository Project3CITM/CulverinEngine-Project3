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

const char * AnimableComponent::ReturnParameterName(ParameterValue parameter)
{
	return nullptr;
}

AnimationValue AnimableComponent::GetParameter(ParameterValue parameter)
{
	return AnimationValue();
}

AnimationData::AnimationData()
{

}

AnimationData::~AnimationData()
{
}
