using CulverinEditor;
using CulverinEditor.Debug;

class Die_Action : Action
{
    public float duration = 1.0f;
    float y_move = -8.0f;
    public float time_to_move = 0.75f;
    float cur_time = 0.0f;

    public Die_Action()
    {
        action_type = ACTION_TYPE.DIE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().PlayAnimationNode("Die");
        //TODO_AI: Die audio
        GetComponent<CompAudio>().PlayEvent("Enemy_SwordDrop");

        //PLAY COMBAT MUSIC
        Audio.ChangeState("MusicState", "None");
        GetLinkedObject("event_manager").GetComponent<PerceptionManager>().player_seen = false;
        Debug.Log("COMBAT OFF", Department.PLAYER, Color.BLUE);

        StatsScore.KillEnemy();

        //Play Dead Audio
        if (GetComponent<EnemyShield_BT>() != null)
            GetComponent<CompAudio>().PlayEvent("Enemy3_Dead");

        if (GetComponent<EnemySpear_BT>() != null)
            GetComponent<CompAudio>().PlayEvent("Enemy2_Dead");

        if (GetComponent<EnemySword_BT>() != null)
            GetComponent<CompAudio>().PlayEvent("Enemy1_Dead");

        GetComponent<CompAnimation>().SetClipDuration("Die", duration);
        GetComponent<CompCollider>().CollisionActive(false);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        
        if (GetComponent<EnemySword_BT>() != null && cur_time <= time_to_move)
        {
            /*cur_time += Time.deltaTime;
            float val = y_move * (cur_time / time_to_move);
            GetComponent<Transform>().SetPosition(new Vector3(GetComponent<Transform>().position.x, val, GetComponent<Transform>().position.z));*/
        }
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}
