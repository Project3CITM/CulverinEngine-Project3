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
    CharactersManager player = null;


    public override bool ActionStart()
    {
        state = SWA_STATE.PRE_APPLY;
        player = GetLinkedObject("target").GetComponent<CharactersManager>();

        if (player.dying == false)
        {
            GetComponent<CompAnimation>().SetClipDuration("Attack", attack_duration);
            GetComponent<CompAnimation>().PlayAnimationNode("Attack");
            GetComponent<CompAudio>().PlayEvent("Enemy2_Slash");
        }

        //Interrupt player action
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (player.dying)
        {
            Debug.Log("DON'T ATTACK PLAYER", Department.PLAYER, Color.YELLOW);
            return ACTION_RESULT.AR_FAIL; //Player is dead, don't attack
        }

        if (melee_attack)
        {
            if (interupt == true)
            {
                return ACTION_RESULT.AR_FAIL;
            }

            //Doing attack
            if (state == SWA_STATE.PRE_APPLY && GetComponent<CompAnimation>().IsAnimOverXTime(apply_damage_point))
            {
                state = SWA_STATE.POST_APPLY;
                player.GetDamage(damage);
                GetComponent<CompAudio>().PlayEvent("SwordHit");
            }
            else if (state == SWA_STATE.POST_APPLY && GetComponent<CompAnimation>().IsAnimationStopped("Attack"))
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
            if (state == SWA_STATE.PRE_APPLY && GetComponent<CompAnimation>().IsAnimOverXTime(apply_damage_point))
            {
                state = SWA_STATE.POST_APPLY;
                player.GetDamage(damage);
                GetComponent<CompAudio>().PlayEvent("SwordHit");
            }
            else if (state == SWA_STATE.POST_APPLY && GetComponent<CompAnimation>().IsAnimationStopped("Attack"))
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
