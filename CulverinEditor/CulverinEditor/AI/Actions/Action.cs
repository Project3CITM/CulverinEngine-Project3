using CulverinEditor;
using CulverinEditor.Debug;

public class Action : CulverinBehaviour
{
    protected bool interupt = false;
    protected float anim_speed = 1.0f;

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
        ATTACK_ACTION,
        IDLE_ACTION,
        IDLE_ATTACK_ACTION,
        INVESTIGATE_ACTION,
        CHASE_ACTION,
        PUSHBACK_ACTION,
        STUN_ACTION,
        GET_HIT_ACTION,
        SHIELD_BLOCK_ACTION,
        ENGAGE_ACTION,
        DISENGAGE_ACTION,
        DIE_ACTION
    }

    public ACTION_TYPE action_type = ACTION_TYPE.NO_ACTION;

    public Action()
    {}

    public Action(float speed)
    {
        anim_speed = speed;
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
        interupt = false;
        return false;
    }

    public void Interupt()
    {
        interupt = true;
    }

    public bool IsInterrupted()
    {
        return interupt == true;
    }

    public void SetAnimSpeed(float speed)
    {
        anim_speed = speed;
    }

    public void Init(float speed)
    {
        anim_speed = speed;
    }
}

