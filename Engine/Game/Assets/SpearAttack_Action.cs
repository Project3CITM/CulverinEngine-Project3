using CulverinEditor.Debug;
using CulverinEditor;

public class SpearAttack_Action : Action
{
    bool melee_attack = false;

    public SpearAttack_Action()
    {
        action_type = ACTION_TYPE.SPEARATTACK_ACTION;
    }

    public SpearAttack_Action(float dmg)
    {
        action_type = ACTION_TYPE.SPEARATTACK_ACTION;
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
    public float attack_duration = 1.0f;

    public GameObject target = null;
    CompAnimation anim = null;
    CharactersManager player = null;


    public override bool ActionStart()
    {
        state = SWA_STATE.PRE_APPLY;
        anim = GetComponent<CompAnimation>();
        anim.SetTransition("ToAttack");
        anim.SetClipDuration("Attack", attack_duration);
        player = GetLinkedObject("target").GetComponent<CharactersManager>();
        //Interrupt player action
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
       // Debug.Log("[error]Is melee attack:" + melee_attack);
        if (melee_attack)
        {
            if (interupt == true)
            {
                return ACTION_RESULT.AR_FAIL;
            }

            //Doing attack
            anim = GetComponent<CompAnimation>();
            if (state == SWA_STATE.PRE_APPLY && anim.IsAnimOverXTime(apply_damage_point))
            {
                state = SWA_STATE.POST_APPLY;
                player.GetDamage(damage);
                //Apply damage to the target
                //Play audio fx
            }
            else if (state == SWA_STATE.POST_APPLY && anim.IsAnimationStopped("Attack"))
            {
                state = SWA_STATE.WAITING;
                return ACTION_RESULT.AR_SUCCESS;
            }
            return ACTION_RESULT.AR_IN_PROGRESS;
        }
        else
        {
            if (interupt == true)
            {
                return ACTION_RESULT.AR_FAIL;
            }

            //Doing attack
            anim = GetComponent<CompAnimation>();
            if (state == SWA_STATE.PRE_APPLY && anim.IsAnimOverXTime(apply_damage_point))
            {
                state = SWA_STATE.POST_APPLY;
                player.GetDamage(damage);
                //Apply damage to the target
                //Play audio fx
            }
            else if (state == SWA_STATE.POST_APPLY && anim.IsAnimationStopped("Attack"))
            {
                state = SWA_STATE.WAITING;
                return ACTION_RESULT.AR_SUCCESS;
            }
            return ACTION_RESULT.AR_IN_PROGRESS;
        }
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }

    public void SetDamage(float attack_damage)
    {
        damage = attack_damage;
    }

    public void IsMeleeAttack(bool melee_b)
    {
        melee_attack = melee_b;
    }

}
