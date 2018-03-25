using CulverinEditor;
using CulverinEditor.Debug;

class Die_Action : Action
{
    public Die_Action()
    {
        action_type = ACTION_TYPE.DIE_ACTION;
    }

    public Die_Action(float anim_speed) : base(anim_speed)
    {
        action_type = ACTION_TYPE.DIE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetTransition("ToDie");
        GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
        return true;
    }

    /*public override ACTION_RESULT ActionUpdate()
    {
        if (GetComponent<CompAnimation>().IsAnimationRunning("Die"))
        {
            Debug.Log("Dead");
           // return ACTION_RESULT.AR_SUCCESS;
        }
        return ACTION_RESULT.AR_IN_PROGRESS;
    }*/

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}
