using CulverinEditor;
using CulverinEditor.Debug;

public class BT : CulverinBehaviour
{
    public enum AI_STATE
    {
        AI_IDLE,
        AI_STUNNED,
        AI_MOVING,
        AI_ATTACKING,
        AI_DEAD
    }

    protected Action current_action = new Action();
    protected AI_STATE state = AI_STATE.AI_IDLE;

    //Blackboard
    public bool player_detected = false;
    public bool heard_something = false;

    //Path Go and Back
    public int origin_path_x;
    public int origin_path_y;

    public int end_path_x;
    public int end_path_y;

    virtual public void Start()
    {
        current_action = MakeDecision();
    }

    public virtual void Update()
    {
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
        Debug.Log("BT decision move defined!");

        //Behaviour tree structure
        if (player_detected)
        {
            //Chase and attack
        }

        if (heard_something)
        {
            //Investigate
        }

        //If none of them -> patrol
        int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        if (my_tile_x != origin_path_x && my_tile_y != origin_path_y)
        {
            GetComponent<Movement_Action>().GoTo(my_tile_x, my_tile_y, origin_path_x, origin_path_y);
            GetComponent<Movement_Action>().ActionStart();
            return GetComponent<Movement_Action>();
        }
        else
        {
            GetComponent<Movement_Action>().GoTo(my_tile_x, my_tile_y, end_path_x, end_path_y);
            GetComponent<Movement_Action>().ActionStart();
            return GetComponent<Movement_Action>();
        }
    }

    public AI_STATE GetState()
    {
        return state;
    }
}
