using CulverinEditor;
using CulverinEditor.Debug;

class Disengage_Action : Action
{
    public float duration = 1.0f;

    public Disengage_Action()
    {
        action_type = ACTION_TYPE.DISENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetTransition("ToSheatheSword");
        GetComponent<CompAnimation>().SetClipDuration("Sheathe Sword", duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (GetComponent<CompAnimation>().IsAnimationStopped("Sheathe Sword"))
            return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

