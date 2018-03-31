using CulverinEditor;
using CulverinEditor.Debug;

class Die_Action : Action
{
    public float duration = 1.0f;

    public Die_Action()
    {
        action_type = ACTION_TYPE.DIE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetTransition("ToDie");
        //TODO_AI: Die audio
        //GetComponent<CompAudio>().PlayEvent("JaimeHurt");
        GetComponent<CompAnimation>().SetClipDuration("Die", duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}
