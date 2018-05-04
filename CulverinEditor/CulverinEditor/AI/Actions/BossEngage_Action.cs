using CulverinEditor;
using CulverinEditor.Debug;

class BossEngage_Action : Action
{
    public float duration = 1.0f;
    bool play_audio = false;

    public BossEngage_Action()
    {
        action_type = ACTION_TYPE.BOSS_ENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.GRABBED);
        GetComponent<CompAnimation>().SetTransition("ToPresentation");
        GetComponent<CompAnimation>().SetClipDuration("Presentation", duration);
        GetComponent<Boss_BT>().boss_fight_timer = 0.0f;
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (GetComponent<CompAnimation>().IsAnimOverXTime(0.2f) && play_audio == false)
        {
            GetComponent<CompAudio>().PlayEvent("Enemy_SwordDraw");
            play_audio = true;
        }

        if (GetComponent<CompAnimation>().IsAnimationStopped("Presentation"))
            return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        Debug.Log("Combat starts");
        GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.IDLE);
        interupt = false;
        play_audio = false;
        GetLinkedObject("hp_bar_boss").SetActive(true);
        GetLinkedObject("hp_bar_boss").GetComponent<BossHPBar>().ActivateHPBar(true);
        Audio.ChangeState("MusicState", "FinalBoss");
        if (Audio.prank)
            Audio.ChangeState("MusicState", "AlternateBoss");
        return true;
    }


}