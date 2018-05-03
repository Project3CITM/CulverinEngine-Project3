using CulverinEditor;
using CulverinEditor.Debug;

class Engage_Action : Action
{
    public float duration = 1.0f;
    bool play_audio = false;
    CompAnimation comp_animation;
    CompAudio comp_audio;
    public Engage_Action()
    {
        action_type = ACTION_TYPE.ENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        comp_animation = GetComponent<CompAnimation>();
        comp_audio = GetComponent<CompAudio>();
        comp_animation.SetTransition("ToDraw");
        comp_animation.SetClipDuration("Draw", duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (comp_animation.IsAnimOverXTime(0.2f) && play_audio == false && GetComponent<EnemySpear_BT>() == null)
        {
            comp_audio.PlayEvent("Enemy_SwordDraw");
            play_audio = true;
        }

        if (comp_animation.IsAnimationStopped("Draw"))
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
