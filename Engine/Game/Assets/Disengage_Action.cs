using CulverinEditor;
using CulverinEditor.Debug;

class Disengage_Action : Action
{
    public Disengage_Action()
    {
        action_type = ACTION_TYPE.ENGAGE_ACTION;
    }

    public Disengage_Action(float anim_speed) : base(anim_speed)
    {
        action_type = ACTION_TYPE.ENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetTransition("ToSheathe");
        GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (GetComponent<CompAnimation>().IsAnimationRunning("Idle"))
        {
            Debug.Log("Disengaged");
            return ACTION_RESULT.AR_SUCCESS;
        }
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        return true;
    }
}

