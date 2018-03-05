using CulverinEditor.Debug;
using CulverinEditor;

public class SwordAttack_Action : Action
{
    enum SWA_STATE
    {
        WAITING,
        PRE_APPLY,
        POST_APPLY
    }

    SWA_STATE state = SWA_STATE.WAITING;

    GameObject target = null;
    GameObject my_object = null;

    public float attack_cooldown = 1.0f;
    public float attack_speed = 1.0f;
    float attack_timer = 0.0f;
    bool doing_attack = false;

    public override bool ActionStart()
    {
        attack_timer = attack_cooldown;
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        //Check if player is in range
        //if(!player_in_range)
        //{
            //return ACTION_RESULT.AR_SUCCESS;
        //}

        //Attack time
        attack_timer += Time.DeltaTime();
        if(attack_timer > (attack_cooldown * attack_speed))
        {
            doing_attack = true;
            state = SWA_STATE.PRE_APPLY;
            GetLinkedObject("my_object").GetComponent<CompAnimation>().PlayAnimation("sword_attack");
        }
        
        //Doing attack
        if(doing_attack)
        {
            if(state == SWA_STATE.PRE_APPLY && GetLinkedObject("my_object").GetComponent<CompAnimation>().IsAnimOverXTime(0.5f))
            {
                state = SWA_STATE.POST_APPLY;
                //Apply damage to the player
                //Play audio fx
            }

            else if(state == SWA_STATE.POST_APPLY && GetLinkedObject("my_object").GetComponent<CompAnimation>().IsAnimationStopped("sword_attack"))
            {
                state = SWA_STATE.WAITING;
                return ACTION_RESULT.AR_SUCCESS;
            }
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }
}
