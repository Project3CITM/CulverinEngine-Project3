using CulverinEditor;
using CulverinEditor.Debug;

class Die_Action : Action
{
    public float duration = 1.0f;
    float y_move = -8.0f;
    public float time_to_move = 0.75f;
    float cur_time = 0.0f;
    CompAnimation anim_comp;
    CompAudio audio_comp;
    bool blood_particle = false;

    public Die_Action()
    {
        action_type = ACTION_TYPE.DIE_ACTION;
    }

    public override bool ActionStart()
    {

        anim_comp = GetComponent<CompAnimation>();
        audio_comp = GetComponent<CompAudio>();

        anim_comp.PlayAnimationNode("Die");
        //TODO_AI: Die audio
        audio_comp.PlayEvent("Enemy_SwordDrop");

        //PLAY COMBAT MUSIC
        Audio.ChangeState("MusicState", "None");
        GetLinkedObject("event_manager").GetComponent<PerceptionManager>().player_seen = false;
        Debug.Log("COMBAT OFF", Department.PLAYER, Color.BLUE);

        StatsScore.KillEnemy();

        //Play Dead Audio
        if (GetComponent<EnemyShield_BT>() != null)
            audio_comp.PlayEvent("Enemy3_Dead");

        if (GetComponent<EnemySpear_BT>() != null)
            audio_comp.PlayEvent("Enemy2_Dead");

        if (GetComponent<EnemySword_BT>() != null)
            audio_comp.PlayEvent("Enemy1_Dead");

        anim_comp.SetClipDuration("Die", duration);
        GetComponent<CompCollider>().CollisionActive(false);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (anim_comp.IsAnimationStopped("Die") && blood_particle == false)
        {
            GameObject floor_blood = SpawnPrefabFromPos("Floor_Blood", transform.GetGlobalPosition(), transform.GetRotation(), new Vector3(0, 0.1f, 5));
            blood_particle = true;
            Debug.Log("I'M DEAD", Department.PLAYER, Color.PINK);
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}
