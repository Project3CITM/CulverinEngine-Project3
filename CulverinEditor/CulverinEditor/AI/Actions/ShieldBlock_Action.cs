using CulverinEditor;

class ShieldBlock_Action : Action
{

    public float block_duration = 1.0f;
    float current_time = 0.0f;
    CompAnimation animator = null;
    private bool blocking = false;

    public ShieldBlock_Action()
    {
        action_type = ACTION_TYPE.SHIELD_BLOCK_ACTION;
    }

    public void Start()
    {
        animator = GetComponent<CompAnimation>();
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().PlayAnimationNode("BlockIdle");
        blocking = true;
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        current_time += Time.deltaTime;

        if(current_time >= block_duration)
        {
            blocking = false;
            current_time = 0.0f;
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        GetComponent<EnemyShield_BT>().ResetShieldBlockTimer();
        interupt = false;
        return false;
    }

    public bool IsBlocking()
    {
        return blocking;
    }
}

