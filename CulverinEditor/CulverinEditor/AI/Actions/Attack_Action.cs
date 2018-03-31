using CulverinEditor.Debug;
using CulverinEditor;

public class Attack_Action : Action
{
    public float block_point = 0.3f;
    public float damage_point = 0.7f;
    public float damage = 1.0f;
    public uint fatigue = 50;
    public float attack_duration = 1.0f;
    public GameObject target = null;
    CompAnimation anim = null;
    CharactersManager player = null;
    bool shield_attack = false;
    bool damage_done = false;

    public Attack_Action()
    {
        action_type = ACTION_TYPE.ATTACK_ACTION;
    }

    public Attack_Action(float dmg)
    {
        action_type = ACTION_TYPE.ATTACK_ACTION;
        damage = dmg;
    }

    public override bool ActionStart()
    {
        anim = GetComponent<CompAnimation>();

        if (GetComponent<EnemyShield_BT>() != null && (player.player_obj.GetComponent<Shield>().IsActive() || player.GetCurrCharacterState() == (int)CharacterController.State.COVER))
        {
            shield_attack = true;
            anim.SetTransition("ToShieldAttack");
            anim.SetClipDuration("ShieldAttack", attack_duration);
        }
        else
        {
            anim.SetTransition("ToAttack");
            anim.SetClipDuration("Attack", attack_duration);
        }
            
        player = GetLinkedObject("target").GetComponent<CharactersManager>();
        damage_done = false;
        shield_attack = false;
        //Interrupt player action
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        anim = GetComponent<CompAnimation>();

        if (anim.IsAnimOverXTime(damage_point) && damage_done == false)
        {
            damage_done = true;

            if (shield_attack)
                player.ApplyFatigue(fatigue);
            else
            {
                if (player.GetDamage(damage) == true)
                    anim.SetActiveBlendingClipWeight(0.0f);
                else
                    anim.SetActiveBlendingClipWeight(1.0f);
            }
        }

        if (shield_attack == true)
        {
            if (anim.IsAnimationStopped("ShieldAttack"))
                return ACTION_RESULT.AR_SUCCESS;
        }
        else
        {
            if (anim.IsAnimationStopped("Attack"))
                return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
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
}
