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
    protected Action idle_action = new Idle_Action();

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
        current_action = idle_action;
        current_action.ActionStart();
        MakeDecision();
    }

    public virtual void Update()
    {
        if(Input.GetKeyDown(KeyCode.W))
        {
            Debug.Log("uvedoble");
            int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
            int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

            GetComponent<PerceptionEmitter>().TriggerHearEvent(PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT, 10, 2, my_tile_x, my_tile_y, 8, 4);
        }

        if (current_action.action_type != Action.ACTION_TYPE.IDLE_ACTION)
        {
            switch (current_action.ActionUpdate())
            {
                case Action.ACTION_RESULT.AR_FAIL:
                    current_action.ActionEnd();
                    MakeDecision();
                    break;

                case Action.ACTION_RESULT.AR_IN_PROGRESS:
                    break;

                case Action.ACTION_RESULT.AR_SUCCESS:
                    current_action.ActionEnd();
                    MakeDecision();
                    break;

                default:
                    Debug.Log("Error on action state!");
                    break;
            }
        }
        else
        {
            MakeDecision();
        }
    }

    public virtual void MakeDecision()
    {
        Debug.Log("BT decision move defined!");

        //Behaviour tree structure
        if (player_detected)
        {
            // TODO: Check for attack

            // Chase
            Debug.Log("Chase Player Action");
            GetComponent<ChasePlayer_Action>().ActionStart();
            current_action = GetComponent<ChasePlayer_Action>();
            return;
        }

        if (heard_something)
        {
            //Investigate
            GetComponent<Investigate_Action>().ActionStart();
            current_action = GetComponent<Investigate_Action>();
            return;
        }

        //If none of them -> patrol
        int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        if (my_tile_x != origin_path_x && my_tile_y != origin_path_y)
        {
            GetComponent<Movement_Action>().GoTo(my_tile_x, my_tile_y, origin_path_x, origin_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
        }
        else
        {
            GetComponent<Movement_Action>().GoTo(my_tile_x, my_tile_y, end_path_x, end_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
        }
    }

    public AI_STATE GetState()
    {
        return state;
    }

    public void InterruptAction()
    {
        current_action.Interupt();
    }
}
