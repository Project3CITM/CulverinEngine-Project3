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
    protected Action next_action = new Action();
    protected AI_STATE state = AI_STATE.AI_IDLE;

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
        current_action = GetComponent<Idle_Action>();
        current_action.SetAnimSpeed(anim_speed);
        current_action.ActionStart();
        MakeDecision();
    }

    public virtual void Update()
    {
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
        if(next_action.action_type != Action.ACTION_TYPE.NO_ACTION)
        {
            current_action = next_action;
            next_action = new Action();
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
