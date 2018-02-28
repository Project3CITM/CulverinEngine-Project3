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
    public virtual bool Start()
    {
        Debug.Log("Action start not defined!");
        return false;
    }

    public virtual ACTION_RESULT Update()
    {
        Debug.Log("Action update not defined!");
        return ACTION_RESULT.AR_FAIL;
    }

    public virtual bool End()
    {
        Debug.Log("Action end not defined!");
        return false;
    }
}

