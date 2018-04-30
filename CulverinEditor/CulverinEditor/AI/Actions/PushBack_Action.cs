using CulverinEditor.Debug;
using CulverinEditor;

public class PushBack_Action : Action
{
    Vector3 push_direction;

    int target_x;
    int target_y;

    public float push_time = 1.0f;
    string animation_clip_push="a";
    public float push_velocity = 2.5f;

    float push_timer = 0.0f;

    Movement_Action move;
    Arrive_Steering arrive_comp;
    CompAnimation anim_comp;
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
        push_direction = push_dir;
    }

    public override bool ActionStart()
    {
        Debug.Log("PUSH ANIMATION", Department.PHYSICS, Color.ORANGE);
        arrive_comp = GetComponent<Arrive_Steering>();
        GetComponent<Align_Steering>().SetEnabled(false);
        Movement_Action.Direction dir = GetComponent<Movement_Action>().GetDirection();

        move = GetComponent<Movement_Action>();
        anim_comp = GetComponent<CompAnimation>();

        GetComponent<PerceptionSightEnemy>().GetPlayerTilePos(out int player_x, out int player_y);
        int tile_x = move.GetCurrentTileX();
        int tile_y = move.GetCurrentTileY();

        int dif_x = player_x - tile_x;
        int dif_y = player_y - tile_y;

        animation_clip_push = "Push";
        anim_comp.PlayAnimationNode("Push");

        target_x = move.GetCurrentTileX();
        target_y = move.GetCurrentTileY();

        target_x += (int)push_direction.x;
        target_y += (int)push_direction.z;

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
        {
            return ACTION_RESULT.AR_FAIL;
        }

        Vector3 my_pos = transform.position;

        Vector3 movement = new Vector3(Vector3.Zero);

        if (anim_comp.IsAnimationStopped(animation_clip_push))
        {
            anim_comp.SetTransition("ToIdleAttack");
            Debug.Log("Tile Destiny x: " + target_x + "y:" + target_y, Department.IA, Color.YELLOW);
            move.SetEnemyTile(target_x, target_y);

            movement.x = (target_x * move.tile_size) - my_pos.x;
            movement.z = (target_y * move.tile_size) - my_pos.z;

            movement = (movement.Normalized * move.tile_size * (1 - arrive_comp.min_distance)) * push_velocity;
            transform.SetPosition(my_pos + ((movement * Time.deltaTime) / push_time));

            if (push_timer >= 0.01f)
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

        movement.x = (target_x * move.tile_size) - my_pos.x;
        movement.z = (target_y * move.tile_size) - my_pos.z;

        movement = (movement.Normalized * move.tile_size * (1 - arrive_comp.min_distance)) * push_velocity;
        transform.SetPosition(my_pos + ((movement * Time.deltaTime) / push_time));

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

