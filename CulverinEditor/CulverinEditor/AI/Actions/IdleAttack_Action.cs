using CulverinEditor;
using CulverinEditor.Debug;

public class IdleAttack_Action : Action
{
    public IdleAttack_Action()
    {
        action_type = ACTION_TYPE.IDLE_ATTACK_ACTION;
    }

    public IdleAttack_Action(float speed) : base(speed)
    {
        action_type = ACTION_TYPE.IDLE_ATTACK_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
        GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
        {
            return ACTION_RESULT.AR_FAIL;
        }
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

