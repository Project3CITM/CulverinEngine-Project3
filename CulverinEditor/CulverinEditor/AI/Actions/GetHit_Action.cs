using CulverinEditor;

class GetHit_Action: Action
{
    public float duration = 1.0f;

    string animation_clip = "a";

    bool played_audio = false;

    public GetHit_Action()
    {
        action_type = ACTION_TYPE.GET_HIT_ACTION;
    }

    public override bool ActionStart()
    {
        interupt = false;
        /*CompAudio audio = GetComponent<CompAudio>();
        audio.PlayEvent("Enemy2_Hurt");
        audio.PlayEvent("SwordHit");*/
        GetComponent<PerceptionSightEnemy>().GetPlayerTilePos(out int player_x, out int player_y);
        int tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        int dif_x = player_x - tile_x;
        int dif_y = player_y - tile_y;

        Movement_Action.Direction dir = GetComponent<Movement_Action>().GetDirection();

        switch(dir)
        {
            case Movement_Action.Direction.DIR_EAST:
                if (dif_x < 0)
                {
                    animation_clip = "HitBack";
                    GetComponent<CompAnimation>().SetTransition("ToHitBack");
                }
                else if (dif_x > 0)
                {
                    animation_clip = "HitFront";
                    GetComponent<CompAnimation>().SetTransition("ToHitFront");
                }
                else if (dif_y < 0)
                {
                    animation_clip = "HitLeft";
                    GetComponent<CompAnimation>().SetTransition("ToHitLeft");
                }
                else if (dif_y > 0)
                {
                    animation_clip = "HitRight";
                    GetComponent<CompAnimation>().SetTransition("ToHitRight");                 
                }
                break;

            case Movement_Action.Direction.DIR_NORTH:
                if (dif_x < 0)
                {
                    animation_clip = "HitLeft";
                    GetComponent<CompAnimation>().SetTransition("ToHitLeft");
                }
                else if (dif_x > 0)
                {
                    animation_clip = "HitRight";
                    GetComponent<CompAnimation>().SetTransition("ToHitRight");
                }
                else if (dif_y < 0)
                {
                    animation_clip = "HitFront";
                    GetComponent<CompAnimation>().SetTransition("ToHitFront");
                }
                else if (dif_y > 0)
                {
                    animation_clip = "HitBack";
                    GetComponent<CompAnimation>().SetTransition("ToHitBack");
                }
                break;

            case Movement_Action.Direction.DIR_SOUTH:
                if (dif_x < 0)
                {
                    animation_clip = "HitRight";
                    GetComponent<CompAnimation>().SetTransition("ToHitRight");
                }
                else if (dif_x > 0)
                {
                    animation_clip = "HitLeft";
                    GetComponent<CompAnimation>().SetTransition("ToHitLeft");
                }
                else if (dif_y < 0)
                {
                    animation_clip = "HitBack";
                    GetComponent<CompAnimation>().SetTransition("ToHitBack");
                    
                }
                else if (dif_y > 0)
                {
                    animation_clip = "HitFront";
                    GetComponent<CompAnimation>().SetTransition("ToHitFront");                   
                }
                break;

            case Movement_Action.Direction.DIR_WEST:
                if (dif_x < 0)
                {
                    animation_clip = "HitFront";
                    GetComponent<CompAnimation>().SetTransition("ToHitFront");
                }
                else if (dif_x > 0)
                {
                    animation_clip = "HitBack";
                    GetComponent<CompAnimation>().SetTransition("ToHitBack");
                }
                else if (dif_y < 0)
                {
                    animation_clip = "HitRight";
                    GetComponent<CompAnimation>().SetTransition("ToHitRight");
                }
                else if (dif_y > 0)
                {
                    animation_clip = "HitLeft";
                    GetComponent<CompAnimation>().SetTransition("ToHitLeft");
                }
                break;
        }
        GetComponent<CompAnimation>().SetClipDuration(animation_clip, duration);
        //TODO_AI: Hurt audio
        //GetComponent<CompAudio>().PlayEvent("JaimeHurt");
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if(GetComponent<CompAnimation>().IsAnimOverXTime(0.3f) && !played_audio)
        {
            GetComponent<CompAudio>().PlayEvent("JaimeHurt");
            played_audio = true;
        }

        if (GetComponent<CompAnimation>().IsAnimationStopped(animation_clip))
            return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        played_audio = false;
        return true;
    }
}
