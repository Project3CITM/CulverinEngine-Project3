using CulverinEditor;
using CulverinEditor.Debug;

public class BT : CulverinBehaviour
{
    Action current_action;

    void Start()
    {
        current_action = MakeDecision();
        while (current_action.Start())
        {
            Debug.Log("Current action start failed!");
            current_action = MakeDecision();
        }
    }

    void Update()
    {
        if (current_action != null)
        {
            switch (current_action.Update())
            {
                case Action.ACTION_RESULT.AR_FAIL:
                    current_action.End();
                    current_action = MakeDecision();
                    while (current_action.Start())
                    {
                        Debug.Log("Current action start failed!");
                        current_action = MakeDecision();
                    }
                    break;

                case Action.ACTION_RESULT.AR_IN_PROGRESS:
                    break;

                case Action.ACTION_RESULT.AR_SUCCESS:
                    current_action.End();
                    current_action = MakeDecision();
                    while(current_action.Start())
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
        
        if(Input.GetKeyDown(KeyCode.W))
        {
            Movement_Action move = new Movement_Action();
            int x, y;
            move.GetCurrentTile(out x, out y);
            move.GoTo(x, y, 10, 10);
        }

        return null;
    }
}

