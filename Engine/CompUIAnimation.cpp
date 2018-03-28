#include "CompUIAnimation.h"
#include "AnimableComponent.h"


CompUIAnimation::CompUIAnimation(Comp_Type t, GameObject * parent):Component(t,parent)
{
}

CompUIAnimation::~CompUIAnimation()
{
}

void CompUIAnimation::Update(float dt)
{
	if (play)
		PlayAnimation(dt);
}

void CompUIAnimation::PlayAnimation(float dt)
{
}
