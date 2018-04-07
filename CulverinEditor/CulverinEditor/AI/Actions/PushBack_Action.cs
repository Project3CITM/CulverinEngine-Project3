using CulverinEditor.Debug;
using CulverinEditor;

public class PushBack_Action : Action
{
    Vector3 push_direction;

    int target_x;
    int target_y;

    public float push_time = 1.0f;
    string animation_clip_push="a";

    float push_timer = 0.0f;

    void Start()
    {
        target_x = 0;
        target_y = 0;
    }

    public PushBack_Action()
    {
        action_type = ACTION_TYPE.PUSHBACK_ACTION;
    }

    public PushBack_Action(Vector3 push_direction_)
    {
        action_type = ACTION_TYPE.PUSHBACK_ACTION;
        push_direction = push_direction_;
    }

    public void SetPushDirection(Vector3 push_dir)
    {
        Debug.Log("push direction is: " + push_dir);
        push_direction = push_dir;
    }

    public override bool ActionStart()
    {
        GetComponent<Align_Steering>().SetEnabled(false);
        Movement_Action.Direction dir = GetComponent<Movement_Action>().GetDirection();

        GetComponent<PerceptionSightEnemy>().GetPlayerTilePos(out int player_x, out int player_y);
        int tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        int dif_x = player_x - tile_x;
        int dif_y = player_y - tile_y;

        switch (dir)
        {
            case Movement_Action.Direction.DIR_EAST:
                if (dif_x < 0)
                {
                    animation_clip_push = "HitBack";
                    GetComponent<CompAnimation>().SetTransition("ToHitBack");
                }
                else if (dif_x > 0)
                {
                    animation_clip_push = "HitFront";
                    GetComponent<CompAnimation>().SetTransition("ToHitFront");
                }
                else if (dif_y < 0)
                {
                    animation_clip_push = "HitLeft";
                    GetComponent<CompAnimation>().SetTransition("ToHitLeft");
                }
                else if (dif_y > 0)
                {
                    animation_clip_push = "HitRight";
                    GetComponent<CompAnimation>().SetTransition("ToHitRight");
                }
                break;

            case Movement_Action.Direction.DIR_NORTH:
                if (dif_x < 0)
                {
                    animation_clip_push = "HitLeft";
                    GetComponent<CompAnimation>().SetTransition("ToHitLeft");
                }
                else if (dif_x > 0)
                {
                    animation_clip_push = "HitRight";
                    GetComponent<CompAnimation>().SetTransition("ToHitRight");
                }
                else if (dif_y < 0)
                {
                    animation_clip_push = "HitFront";
                    GetComponent<CompAnimation>().SetTransition("ToHitFront");
                }
                else if (dif_y > 0)
                {
                    animation_clip_push = "HitBack";
                    GetComponent<CompAnimation>().SetTransition("ToHitBack");
                }
                break;

            case Movement_Action.Direction.DIR_SOUTH:
                if (dif_x < 0)
                {
                    animation_clip_push = "HitRight";
                    GetComponent<CompAnimation>().SetTransition("ToHitRight");
                }
                else if (dif_x > 0)
                {
                    animation_clip_push = "HitLeft";
                    GetComponent<CompAnimation>().SetTransition("ToHitLeft");
                }
                else if (dif_y < 0)
                {
                    animation_clip_push = "HitBack";
                    GetComponent<CompAnimation>().SetTransition("ToHitBack");

                }
                else if (dif_y > 0)
                {
                    animation_clip_push = "HitFront";
                    GetComponent<CompAnimation>().SetTransition("ToHitFront");
                }
                break;

            case Movement_Action.Direction.DIR_WEST:
                if (dif_x < 0)
                {
                    animation_clip_push = "HitFront";
                    GetComponent<CompAnimation>().SetTransition("ToHitFront");
                }
                else if (dif_x > 0)
                {
                    animation_clip_push = "HitBack";
                    GetComponent<CompAnimation>().SetTransition("ToHitBack");
                }
                else if (dif_y < 0)
                {
                    animation_clip_push = "HitRight";
                    GetComponent<CompAnimation>().SetTransition("ToHitRight");
                }
                else if (dif_y > 0)
                {
                    animation_clip_push = "HitLeft";
                    GetComponent<CompAnimation>().SetTransition("ToHitLeft");
                }
                break;
        }
        Debug.Log("[error] animation clip: " + animation_clip_push);
        GetComponent<CompAnimation>().SetClipDuration(animation_clip_push, push_time);

        target_x = GetComponent<Movement_Action>().GetCurrentTileX();
        target_y = GetComponent<Movement_Action>().GetCurrentTileY();

        target_x += (int)push_direction.x;
        target_y += (int)push_direction.z;

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
            return ACTION_RESULT.AR_FAIL;

        if (GetComponent<CompAnimation>().IsAnimationStopped(animation_clip_push))
        {
            Debug.Log("Pushed");
            GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
            GetComponent<Movement_Action>().SetEnemyTile(target_x, target_y);

            if (push_timer >= 0.5f)
            {
                push_timer = 0.0f;
                return ACTION_RESULT.AR_SUCCESS;
            }
            else
            {
                push_timer = push_timer + Time.deltaTime;
                return ACTION_RESULT.AR_IN_PROGRESS;
            }

        }

        Vector3 my_pos = transform.position;

        Vector3 movement = new Vector3(Vector3.Zero);
        movement.x = (target_x * GetComponent<Movement_Action>().tile_size) - my_pos.x;
        movement.z = (target_y * GetComponent<Movement_Action>().tile_size) - my_pos.z;

        movement = movement.Normalized * GetComponent<Movement_Action>().tile_size * (1 - GetComponent<Arrive_Steering>().min_distance);
        transform.SetPosition(my_pos + ((movement * Time.deltaTime) / push_time));

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

