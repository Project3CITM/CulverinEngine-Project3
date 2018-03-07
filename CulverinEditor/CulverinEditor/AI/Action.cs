using CulverinEditor;
using CulverinEditor.Debug;

public class Action : CulverinBehaviour
{
    protected bool interupt = false;

    public enum ACTION_RESULT
    {
        AR_IN_PROGRESS,
        AR_FAIL,
        AR_SUCCESS

    }

    public enum ACTION_TYPE
    {
        NO_ACTION,
        MOVE_ACTION,
        ATTACK_ACTION
    }

    public ACTION_TYPE action_type = ACTION_TYPE.NO_ACTION;

    public virtual bool ActionStart()
    {
        Debug.Log("Action start not defined!");
        return false;
    }

    public virtual ACTION_RESULT ActionUpdate()
    {
        Debug.Log("Action update not defined!");
        return ACTION_RESULT.AR_FAIL;
    }

    public virtual bool ActionEnd()
    {
        Debug.Log("Action end not defined!");
        return false;
    }

    public void Interupt()
    {
        interupt = true;
    }
}

