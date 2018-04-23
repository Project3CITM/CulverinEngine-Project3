using CulverinEditor;
using CulverinEditor.Debug;

public class Action : CulverinBehaviour
{
    protected bool interupt = false;

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
        SPEARATTACK_ACTION,
        IDLE_ACTION,
        IDLE_ATTACK_ACTION,
        INVESTIGATE_ACTION,
        CHASE_ACTION,
        INFINITE_CHASE_ACTION,
        SEPARATE_ACTION,
        FACE_PLAYER_ACTION,
        PUSHBACK_ACTION,
        STUN_ACTION,
        GET_HIT_ACTION,
        SHIELD_BLOCK_ACTION,
        ENGAGE_ACTION,
        DISENGAGE_ACTION,
        DIE_ACTION,
        BOSS_ATTACK_SWORD_DOWN_ACTION,
        BOSS_ATTACK_WIDE_ACTION,
        BOSS_ATTACK_STRONG_ACTION,
        BOSS_ATTACK_GRAB_ACTION,
        BOSS_ENGAGE_ACTION
    }

    public ACTION_TYPE action_type = ACTION_TYPE.NO_ACTION;

    public Action()
    {
        action_type = ACTION_TYPE.NO_ACTION;
    }

    public virtual bool ActionStart()
    {
        Debug.Log("[error] Action start not defined!");
        return false;
    }

    public virtual ACTION_RESULT ActionUpdate()
    {
        Debug.Log("[error] Action update not defined!");
        return ACTION_RESULT.AR_FAIL;
    }

    public virtual bool ActionEnd()
    {
        Debug.Log("[error] Action end not defined!");
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
}

