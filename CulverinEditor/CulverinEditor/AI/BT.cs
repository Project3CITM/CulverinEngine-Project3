using CulverinEditor;
using CulverinEditor.Debug;

public class BT : CulverinBehaviour
{
    Action current_action = new Action();

    void Start()
    {
        current_action = MakeDecision();
    }

    void Update()
    {
        //Debug.Log(current_action.action_type.ToString());

        if (current_action.action_type != Action.ACTION_TYPE.NO_ACTION)
        {
            switch (current_action.ActionUpdate())
            {
                case Action.ACTION_RESULT.AR_FAIL:
                    current_action.ActionEnd();
                    current_action = MakeDecision();
                    break;

                case Action.ACTION_RESULT.AR_IN_PROGRESS:
                    break;

                case Action.ACTION_RESULT.AR_SUCCESS:
                    current_action.ActionEnd();
                    current_action = MakeDecision();
                    break;

                default:
                    Debug.Log("Error on action state!");
                    break;
            }             
        }
    }

    public virtual Action MakeDecision()
    {
        Debug.Log("BT decision not defined!");
        
        Movement_Action move = GetComponent<Movement_Action>();
        Debug.Log("BT decision move defined!");
        int x, y;
        GetComponent<Movement_Action>().GetCurrentTile(out x, out y);
        Debug.Log("BT decision move defined!" + x.ToString() + "||" + y.ToString());
        GetComponent<Movement_Action>().GoTo(x, y, 10, 10);
        Debug.Log("BT decision move defined!");
        current_action = GetComponent<Movement_Action>();
        Debug.Log("defined!");

        return GetComponent<Movement_Action>();
    }
}

