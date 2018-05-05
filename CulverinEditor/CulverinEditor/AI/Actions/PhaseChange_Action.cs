using CulverinEditor;
using CulverinEditor.Debug;

class PhaseChange_Action : Action
{
    public float duration = 1.0f;
    public float first_hit_fx = 0.2f;
    public float second_hit_fx = 0.4f;
    public float third_hit_fx = 0.6f;
    private bool first_hit = false;
    private bool second_hit = false;
    private bool third_hit = false;

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
        if (GetComponent<CompAnimation>().IsAnimOverXTime(first_hit_fx) && first_hit == false)
        {
            GetComponent<CompAudio>().PlayEvent("BossHitGround");
            first_hit = true;
        }

        if (GetComponent<CompAnimation>().IsAnimOverXTime(second_hit_fx) && second_hit == false)
        {
            GetComponent<CompAudio>().PlayEvent("BossHitGround");
            second_hit = true;
        }

        if (GetComponent<CompAnimation>().IsAnimOverXTime(third_hit_fx) && third_hit == false)
        {
            GetComponent<CompAudio>().PlayEvent("BossHitGround");
            third_hit = true;
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