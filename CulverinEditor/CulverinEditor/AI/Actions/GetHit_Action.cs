using CulverinEditor;

class GetHit_Action: Action
{
    public float duration = 1.0f;

    public GetHit_Action()
    {
        action_type = ACTION_TYPE.GET_HIT_ACTION;
    }

    void Start()
    {}

    public override bool ActionStart()
    {
        interupt = false;
        GetComponent<CompAnimation>().SetTransition("ToHit");
        GetComponent<CompAnimation>().SetClipDuration("Hit", duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (GetComponent<CompAnimation>().IsAnimationStopped("Hit"))
            return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}
