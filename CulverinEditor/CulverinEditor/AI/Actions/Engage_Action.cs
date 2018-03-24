using CulverinEditor;
using CulverinEditor.Debug;

class Engage_Action : Action
{
    public Engage_Action()
    {
        action_type = ACTION_TYPE.ENGAGE_ACTION;
    }

    public Engage_Action(float anim_speed) : base(anim_speed)
    {
        action_type = ACTION_TYPE.ENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        Debug.Log("EngagedStart");
        GetComponent<CompAnimation>().SetTransition("ToDraw");
        GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        Debug.Log("Engageing");
        if (GetComponent<CompAnimation>().IsAnimationStopped("Draw"))
        {
            Debug.Log("Engaged");
            return ACTION_RESULT.AR_SUCCESS;
        }
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}
