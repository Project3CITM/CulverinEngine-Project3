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
        player = GetLinkedObject("target").GetComponent<CharactersManager>();
        if (player == null)
            Debug.Log("[error] Attack Action Start: Player is null!");

        GetComponent<CompAnimation>().SetTransition("ToAttack");
        GetComponent<CompAnimation>().SetClipDuration("Attack", attack_duration);
       
        GetComponent<CompAudio>().PlayEvent("Enemy1_Slash");
      

        damage_done = false;
        shield_attack = false;
        //Interrupt player action
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
      
        if (GetComponent<CompAnimation>().IsAnimOverXTime(damage_point) && damage_done == false)
        {
            damage_done = true;

            if (shield_attack)
                player.ApplyFatigue(fatigue);
            else
            {
                if (player.GetDamage(damage) == true)
                { 
                    GetComponent<CompAudio>().PlayEvent("SwordHit");
                    GetComponent<CompAnimation>().SetFirstActiveBlendingClipWeight(0.0f);
                }
                else
                    GetComponent<CompAnimation>().SetFirstActiveBlendingClipWeight(1.0f);
            }
        }

        if (GetComponent<CompAnimation>().IsAnimationStopped("Attack"))
            return ACTION_RESULT.AR_SUCCESS;
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
