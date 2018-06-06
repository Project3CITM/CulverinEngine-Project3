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
    CompAnimation anim_comp;
    CompAudio audio_comp;

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
        {
            Debug.Log("[error] Attack Action Start: Player is null!");
        }

        anim_comp = GetComponent<CompAnimation>();
        audio_comp = GetComponent<CompAudio>();

        if (player.dying == false)
        {
            anim_comp.SetClipDuration("Attack", attack_duration);
            anim_comp.PlayAnimationNode("Attack");
            Debug.Log("Back to Attack", Department.IA);
            audio_comp.PlayEvent("Enemy1_Slash");
        }
      

        damage_done = false;
        shield_attack = false;

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

        if (GetComponent<CompAnimation>().IsAnimOverXTime(damage_point) && damage_done == false)
        {
            damage_done = true;

            if (shield_attack)
            {
                player.ApplyFatigue(fatigue);
            }
            else
            {
                if (player.GetDamage(damage) == true)
                {
                    audio_comp.PlayEvent("SwordHit");
                    anim_comp.SetFirstActiveBlendingClipWeight(0.0f);
                }
                else { 
                    anim_comp.SetFirstActiveBlendingClipWeight(1.0f);
                }
            }
        }

        if (anim_comp.IsAnimationStopped("Attack"))
        {
            GetComponent<CompAnimation>().PlayAnimationNode("IdleAttack");
            Debug.Log("Back to Idle", Department.IA);
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
