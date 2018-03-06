using CulverinEditor.Debug;
using CulverinEditor;

public class SwordAttack_Action : Action
{

    public SwordAttack_Action(float attack_speed)
    {
        speed = attack_speed;
    }

    enum SWA_STATE
    {
        WAITING,
        PRE_APPLY,
        POST_APPLY
    }

    SWA_STATE state = SWA_STATE.WAITING;
    float speed = 1.0f;

    GameObject target = null;
    GameObject my_object = null;

    void Start()
    {
        state = SWA_STATE.PRE_APPLY;
        GetLinkedObject("my_object").GetComponent<CompAnimation>().PlayAnimation("sword_attack");
        GetComponent<CompAnimation>().SetClipsSpeed(speed);
    }

    public override ACTION_RESULT ActionUpdate()
    {      
        //Doing attack
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
        
        return ACTION_RESULT.AR_IN_PROGRESS;
    }
}
