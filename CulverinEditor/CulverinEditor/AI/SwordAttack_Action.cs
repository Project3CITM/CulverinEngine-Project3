using CulverinEditor.Debug;
using CulverinEditor;

public class SwordAttack_Action : Action
{

    public SwordAttack_Action()
    {
        action_type = ACTION_TYPE.ATTACK_ACTION;
    }

    public SwordAttack_Action(float attack_speed)
    {
        action_type = ACTION_TYPE.ATTACK_ACTION;
        speed = attack_speed;
    }

    public enum SWA_STATE
    {
        WAITING,
        PRE_APPLY,
        POST_APPLY
    }

    SWA_STATE state = SWA_STATE.WAITING;
    float speed = 1.0f;
    public float apply_damage_point = 0.5f;

    public GameObject target = null;
    public GameObject my_object = null;
    CompAnimation animator = null;

    public override bool ActionStart()
    {
        Debug.Log("Start Called!");
        state = SWA_STATE.PRE_APPLY;
        animator = GetComponent<CompAnimation>();
        animator.PlayAnimation("Cover"); //This will be attack
        animator.SetClipsSpeed(speed);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        Debug.Log("Doing Attack");

        //Doing attack
        if (state == SWA_STATE.PRE_APPLY && animator.IsAnimOverXTime(apply_damage_point))
        {
            state = SWA_STATE.POST_APPLY;
            //Apply damage to the target
            //Play audio fx
        }

        else if (state == SWA_STATE.POST_APPLY && animator.IsAnimationStopped("Cover"))
        {
            Debug.Log("ATTACK_EEEEND!");
            state = SWA_STATE.WAITING;
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        return true;
    }
}
