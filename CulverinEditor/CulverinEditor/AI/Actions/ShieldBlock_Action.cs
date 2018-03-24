using CulverinEditor;

class ShieldBlock_Action : Action
{

    public float block_duration = 1.0f;
    float current_time = 0.0f;
    CompAnimation animator = null;

    public ShieldBlock_Action()
    {
        action_type = ACTION_TYPE.SHIELD_BLOCK_ACTION;
    }

    public ShieldBlock_Action(float speed)
    {
        anim_speed = speed;
        action_type = ACTION_TYPE.SHIELD_BLOCK_ACTION;
    }

    public void Start()
    {
        animator = GetComponent<CompAnimation>();
    }

    public override bool ActionStart()
    {
        animator.SetTransition("ToBlock");

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        current_time += Time.deltaTime;

        if(current_time >= block_duration)
        {
            current_time = 0.0f;
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return false;
    }
}

