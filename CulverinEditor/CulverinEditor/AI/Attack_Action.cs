using CulverinEditor.Debug;
using CulverinEditor;

public class Attack_Action : Action
{
    public Attack_Action()
    {
        action_type = ACTION_TYPE.ATTACK_ACTION;
    }

    public Attack_Action(float attack_speed, float dmg): base(attack_speed)
    {
        action_type = ACTION_TYPE.ATTACK_ACTION;
        damage = dmg;
    }

    public enum SWA_STATE
    {
        WAITING,
        PRE_APPLY,
        POST_APPLY
    }

    SWA_STATE state = SWA_STATE.WAITING;
    float damage = 1.0f;
    public float apply_damage_point = 0.5f;

    public GameObject target = null;
    public GameObject my_object = null;
    CompAnimation anim = null;
    CharactersManager player = null;

    public override bool ActionStart()
    {
        state = SWA_STATE.PRE_APPLY;
        anim = GetComponent<CompAnimation>();
        anim.SetTransition("ToAttack"); //This will be attack
        anim.SetClipsSpeed(anim_speed);
        player = GetLinkedObject("target").GetComponent<CharactersManager>();
        //Interrupt player action
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
        {
            return ACTION_RESULT.AR_FAIL;
        }

        //Doing attack
        if (state == SWA_STATE.PRE_APPLY && anim.IsAnimOverXTime(apply_damage_point))
        {
            state = SWA_STATE.POST_APPLY;
            player.GetDamage(damage);
            //Apply damage to the target
            //Play audio fx
        }

        else if (state == SWA_STATE.POST_APPLY && anim.IsAnimationStopped("Attack"))
        {
            Debug.Log("ATTACK_EEEEND!");
            state = SWA_STATE.WAITING;
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}
