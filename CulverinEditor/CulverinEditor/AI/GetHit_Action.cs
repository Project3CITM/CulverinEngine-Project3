using CulverinEditor;

class GetHit_Action: Action
{
    CompAnimation anim;
    public float hit_speed = 1.0f;

    public GetHit_Action()
    {
        action_type = ACTION_TYPE.GET_HIT_ACTION;
    }

    public GetHit_Action(float speed):base(speed)
    {
        action_type = ACTION_TYPE.GET_HIT_ACTION;
    }

    void Start()
    {
        anim = GetComponent<CompAnimation>();
    }

    public override bool ActionStart()
    {
        //interupt = false;
        //anim.SetTransition("ToHit");
        //anim.SetClipsSpeed(hit_speed);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        //if (anim.IsAnimationStopped("Hit"))
        //{
            return ACTION_RESULT.AR_SUCCESS;
        //}
        //return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        //anim.SetClipsSpeed(anim_speed);
        return true;
    }
}
