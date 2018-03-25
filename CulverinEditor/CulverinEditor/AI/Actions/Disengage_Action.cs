using CulverinEditor;
using CulverinEditor.Debug;

class Disengage_Action : Action
{
    public Disengage_Action()
    {
        action_type = ACTION_TYPE.DISENGAGE_ACTION;
    }

    public Disengage_Action(float anim_speed) : base(anim_speed)
    {
        action_type = ACTION_TYPE.DISENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetTransition("ToSheatheSword");
        GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        //We need to fix this if becaus this causes infinite loop.
        //if (GetComponent<CompAnimation>().IsAnimationStopped("Sheathe Sword"))
            return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

