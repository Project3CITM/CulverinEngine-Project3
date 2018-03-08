using CulverinEditor.Debug;
using CulverinEditor;

class Idle_Action : Action
{
    public override bool ActionStart()
    {
        Debug.Log("IM IDLE!");
        GetComponent<CompAnimation>().PlayAnimation("Idle");
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true) return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        return true;
    }
}

