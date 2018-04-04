using CulverinEditor.Debug;
using CulverinEditor;

public class Stun_Action : Action
{
    public float stun_duration = 1.0f;

    void Start()
    {
    }

    public Stun_Action()
    {
        action_type = ACTION_TYPE.STUN_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<Align_Steering>().SetEnabled(false);
        GetComponent<Arrive_Steering>().SetEnabled(false);
        GetComponent<Seek_Steering>().SetEnabled(false);
        GetComponent<CompAnimation>().SetTransition("ToStun");
        GetComponent<CompAnimation>().SetClipDuration("Stun", stun_duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
            return ACTION_RESULT.AR_FAIL;

        if (GetComponent<CompAnimation>().IsAnimationStopped("Stun"))
        {
            Debug.Log("Stun finished");
            GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
            return ACTION_RESULT.AR_SUCCESS;
        }

        // Does nothing
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}