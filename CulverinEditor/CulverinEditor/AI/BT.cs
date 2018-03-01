using CulverinEditor;
using CulverinEditor.Debug;

public class BT : CulverinBehaviour
{
    Action current_action = null;

    void Start()
    {
        current_action = MakeDecision();
        /*while (current_action.ActionStart() == false)
        {
            Debug.Log("Current action start failed!");
            current_action = MakeDecision();
        }*/
    }

    void Update()
    {
        if (!(current_action == null))
        {
            switch (current_action.ActionUpdate())
            {
                case Action.ACTION_RESULT.AR_FAIL:
                    current_action.ActionEnd();
                    current_action = MakeDecision();
                    while (current_action.ActionStart())
                    {
                        Debug.Log("Current action start failed!");
                        current_action = MakeDecision();
                    }
                    break;

                case Action.ACTION_RESULT.AR_IN_PROGRESS:
                    break;

                case Action.ACTION_RESULT.AR_SUCCESS:
                    current_action.ActionEnd();
                    current_action = MakeDecision();
                    while(current_action.ActionStart())
                    {
                        Debug.Log("Current action start failed!");
                        current_action = MakeDecision();
                    }
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

        //if(Input.GetKeyDown(KeyCode.W))
        
        Movement_Action move = GetComponent<Movement_Action>();
        Debug.Log("BT decision move defined!");
        int x, y;
        GetComponent<Movement_Action>().GetCurrentTile(out x, out y);
        Debug.Log("BT decision move defined!" + x.ToString() + "||" + y.ToString());
        GetComponent<Movement_Action>().GoTo(x, y, 10, 10);
        Debug.Log("BT decision move defined!");
        current_action = GetComponent<Movement_Action>();
        Debug.Log("BT decision move defined!");

        return GetComponent<Movement_Action>();
    }
}

