using CulverinEditor.Debug;
using CulverinEditor;

public class Idle_Action : Action
{
    public Idle_Action()
    {
        action_type = ACTION_TYPE.IDLE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetTransition("ToIdle");
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
        {
            return ACTION_RESULT.AR_FAIL;
        }
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

