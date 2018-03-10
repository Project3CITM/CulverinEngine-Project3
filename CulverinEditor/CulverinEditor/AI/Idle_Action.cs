using CulverinEditor.Debug;
using CulverinEditor;

public class Idle_Action : Action
{
    public Idle_Action()
    {
        action_type = ACTION_TYPE.IDLE_ACTION;
    }

    public Idle_Action(float speed): base (speed)
    {
        action_type = ACTION_TYPE.IDLE_ACTION;
    }

    public override bool ActionStart()
    {
        Debug.Log("IM IDLE!");
        //GetComponent<CompAnimation>().PlayAnimation("Idle");
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

