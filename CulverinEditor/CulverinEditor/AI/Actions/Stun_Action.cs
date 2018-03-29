using CulverinEditor.Debug;
using CulverinEditor;

public class Stun_Action : Action
{
    public float stun_duration = 1.0f;
    float stun_timer = 0.0f;

    void Start()
    {
    }

    public Stun_Action()
    {
        action_type = ACTION_TYPE.STUN_ACTION;
    }

    public Stun_Action(float speed) : base(speed)
    {
        action_type = ACTION_TYPE.STUN_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<Align_Steering>().SetEnabled(false);
        GetComponent<Arrive_Steering>().SetEnabled(false);
        GetComponent<Seek_Steering>().SetEnabled(false);
        GetComponent<CompAnimation>().SetTransition("ToStun");

        stun_timer = 0.0f;

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
            return ACTION_RESULT.AR_FAIL;

        stun_timer += Time.deltaTime;
        if (stun_timer >= stun_duration)
        {
            Debug.Log("Stun finished");
            GetComponent<CompAnimation>().SetTransition("ToIdle");
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