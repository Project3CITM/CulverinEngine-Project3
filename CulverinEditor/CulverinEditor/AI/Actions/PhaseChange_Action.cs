using CulverinEditor;
using CulverinEditor.Debug;

class PhaseChange_Action : Action
{
    public float duration = 1.0f;
    bool play_audio = false;

    public PhaseChange_Action()
    {
        action_type = ACTION_TYPE.BOSS_ENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.GRABBED);
        GetComponent<CompAnimation>().SetTransition("ToPhaseChange");
        GetComponent<CompAnimation>().SetClipDuration("PhaseChange", duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (GetComponent<CompAnimation>().IsAnimOverXTime(0.2f) && play_audio == false)
        {
            GetComponent<CompAudio>().PlayEvent("Enemy_SwordDraw");
            play_audio = true;
        }

        if (GetComponent<CompAnimation>().IsAnimationStopped("PhaseChange"))
        {
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        Debug.Log("Combat starts");
        GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.IDLE);
        interupt = false;
        play_audio = false;
        
        return true;
    }


}