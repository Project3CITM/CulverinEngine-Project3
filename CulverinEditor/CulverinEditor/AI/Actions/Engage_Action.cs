using CulverinEditor;
using CulverinEditor.Debug;

class Engage_Action : Action
{
    public float duration = 1.0f;
    bool play_audio = false;

    public Engage_Action()
    {
        action_type = ACTION_TYPE.ENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetTransition("ToDraw");
        GetComponent<CompAnimation>().SetClipDuration("Draw", duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (GetComponent<CompAnimation>().IsAnimOverXTime(0.2f) && play_audio == false && GetComponent<EnemySpear_BT>() == null)
        {
            GetComponent<CompAudio>().PlayEvent("Enemy_SwordDraw");
            play_audio = true;
        }

        if (GetComponent<CompAnimation>().IsAnimationStopped("Draw"))
            return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        play_audio = false;
        return true;
    }
}
