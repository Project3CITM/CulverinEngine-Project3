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
        AI_BLOCKING,
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

    public float anim_speed = 1.0f;


    virtual public void Start()
    {
        Debug.Log("Base BT");
        idle_action.SetAnimSpeed(anim_speed);
        current_action = idle_action;
        current_action.ActionStart();
        MakeDecision();
    }

    public virtual void Update()
    {
        if(Input.GetKeyDown(KeyCode.Z))
        {
            Debug.Log("seta");
            int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
            int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

            GetComponent<PerceptionEmitter>().TriggerHearEvent(PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT, 5, 5, my_tile_x, my_tile_y, 2, 23);
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
    {}

    public AI_STATE GetState()
    {
        return state;
    }

    public void InterruptAction()
    {
        current_action.Interupt();
    }
}
