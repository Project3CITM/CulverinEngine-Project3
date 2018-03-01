using CulverinEditor;
using CulverinEditor.Debug;

public class Action : CulverinBehaviour
{    
    public enum ACTION_RESULT
    {
        AR_IN_PROGRESS,
        AR_FAIL,
        AR_SUCCESS

    }
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
}

