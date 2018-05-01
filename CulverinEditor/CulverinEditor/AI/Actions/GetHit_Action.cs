using CulverinEditor;
using CulverinEditor.Debug;

class GetHit_Action: Action
{
    public float duration = 1.0f;

    string animation_clip = "a";

    bool played_audio = false;

    private Enemy_BT.ENEMY_GET_DAMAGE_TYPE next_dmg_type = Enemy_BT.ENEMY_GET_DAMAGE_TYPE.DEFAULT;
    Movement_Action move;
    CompAnimation anim_comp;
    CompAudio audio_comp;

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
        move = GetComponent<Movement_Action>();
        anim_comp = GetComponent<CompAnimation>();
        audio_comp = GetComponent<CompAudio>();


        if (next_dmg_type == Enemy_BT.ENEMY_GET_DAMAGE_TYPE.DEFAULT)
        {

            int tile_x = move.GetCurrentTileX();
            int tile_y = move.GetCurrentTileY();
            int player_x = tile_x;
            int player_y = tile_y;
            MovementController temp = GetLinkedObject("player_obj").GetComponent<MovementController>();
            if (temp != null)
            {
                temp.GetPlayerPos(out int x, out int y);
                player_x = x;
                player_y = y;
            }

            int dif_x = player_x - tile_x;
            int dif_y = player_y - tile_y;

            Movement_Action.Direction dir = move.GetDirection();

            switch (dir)
            {
                case Movement_Action.Direction.DIR_EAST:
                    if (dif_x < 0)
                    {
                        animation_clip = "HitBack";
                        anim_comp.SetTransition("ToHitBack");
                    }
                    else if (dif_x > 0)
                    {
                        animation_clip = "HitFront";
                        anim_comp.SetTransition("ToHitFront");
                    }
                    else if (dif_y < 0)
                    {
                        animation_clip = "HitLeft";
                        anim_comp.SetTransition("ToHitLeft");
                    }
                    else if (dif_y > 0)
                    {
                        animation_clip = "HitRight";
                        anim_comp.SetTransition("ToHitRight");
                    }
                    break;

                case Movement_Action.Direction.DIR_NORTH:
                    if (dif_x < 0)
                    {
                        animation_clip = "HitLeft";
                        anim_comp.SetTransition("ToHitLeft");
                    }
                    else if (dif_x > 0)
                    {
                        animation_clip = "HitRight";
                        anim_comp.SetTransition("ToHitRight");
                    }
                    else if (dif_y < 0)
                    {
                        animation_clip = "HitFront";
                        anim_comp.SetTransition("ToHitFront");
                    }
                    else if (dif_y > 0)
                    {
                        animation_clip = "HitBack";
                        anim_comp.SetTransition("ToHitBack");
                    }
                    break;

                case Movement_Action.Direction.DIR_SOUTH:
                    if (dif_x < 0)
                    {
                        animation_clip = "HitRight";
                        anim_comp.SetTransition("ToHitRight");
                    }
                    else if (dif_x > 0)
                    {
                        animation_clip = "HitLeft";
                        anim_comp.SetTransition("ToHitLeft");
                    }
                    else if (dif_y < 0)
                    {
                        animation_clip = "HitBack";
                        anim_comp.SetTransition("ToHitBack");

                    }
                    else if (dif_y > 0)
                    {
                        animation_clip = "HitFront";
                        anim_comp.SetTransition("ToHitFront");
                    }
                    break;

                case Movement_Action.Direction.DIR_WEST:
                    if (dif_x < 0)
                    {
                        animation_clip = "HitFront";
                        anim_comp.SetTransition("ToHitFront");
                    }
                    else if (dif_x > 0)
                    {
                        animation_clip = "HitBack";
                        anim_comp.SetTransition("ToHitBack");
                    }
                    else if (dif_y < 0)
                    {
                        animation_clip = "HitRight";
                        anim_comp.SetTransition("ToHitRight");
                    }
                    else if (dif_y > 0)
                    {
                        animation_clip = "HitLeft";
                        anim_comp.SetTransition("ToHitLeft");
                    }
                    break;
            }
        }

        else if(next_dmg_type == Enemy_BT.ENEMY_GET_DAMAGE_TYPE.FIREWALL)
        {
            animation_clip = "HitFire";
            anim_comp.PlayAnimationNode("HitFire");
        }

        anim_comp.SetClipDuration(animation_clip, duration);
        //TODO_AI: Hurt audio
        //GetComponent<CompAudio>().PlayEvent("JaimeHurt");
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
        {
            return ACTION_RESULT.AR_FAIL;
        }

        /*if(anim_comp.IsAnimOverXTime(0.3f) && !played_audio)
        {
            if(GetComponent<EnemyShield_BT>() != null)
                audio_comp.PlayEvent("Enemy3_Hurt");

            if (GetComponent<EnemySpear_BT>() != null)
                audio_comp.PlayEvent("Enemy2_Hurt");

            if (GetComponent<EnemySword_BT>() != null)
                audio_comp.PlayEvent("Enemy1_Hurt");

            played_audio = true;
        }*/

        if (anim_comp.IsAnimationStopped(animation_clip))
            return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        played_audio = false;
        return true;
    }

    public void SetHitType(Enemy_BT.ENEMY_GET_DAMAGE_TYPE dmg_type)
    {
        next_dmg_type = dmg_type;
    }
}
