using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;
using CulverinEditor.Map;
using System.Collections.Generic;
using System.Collections;

public class AIManager : CulverinBehaviour
{
   /* public enum MYDIRECTION
    {
        NO_DIR = -1,
        NORTH = 0,
        SOUTH = 180,
        EAST = 270,
        WEST = 90

    }

    public enum MYSTATE
    {
        IDLE,
        MOVE_TO_PLAYER,
        ATTACK,
        RETURN_TO_START,
        DEAD
    }

    //Pathfinding
    GameObject this_obj_enemy;
    GameObject enemy_anim;
    Pathfinder path_creator;
    List<PathNode> my_path;
    bool path_reached;
    float pathfinder_timer;

    public uint current_x, current_y;
    uint init_tile_x, init_tile_y;

    //Movement
    int my_dir;
    public float tile_size;
    public float detection_radius;
    public float movement_speed, rotation_speed;

    //State machine
    MYSTATE state;
    public int player_x, player_y;
    bool player_detected;

    float attack_cooldown_timer;
    public float damage;

    public float player_time_in_memory;
    private int memory_player_x, memory_player_y;
    float player_memory_timer;
    CompAnimation animation_controller;

    // Use this for initialization
    void Start()
    {
        //Set Variables
        path_creator = new Pathfinder();
        path_reached = false;
        state = MYSTATE.IDLE;
        player_detected = false;
        init_tile_x = current_x;
        init_tile_y = current_y;

        enemy_anim = GetLinkedObject("enemy_anim");
        animation_controller = enemy_anim.GetComponent<CompAnimation>();
        animation_controller.PlayAnimation("Idle");

        player_memory_timer = 0.0f;
        attack_cooldown_timer = 0.0f;
        pathfinder_timer = 0.0f;

        my_path = new List<PathNode>();

        //Init Pathfinder 
        path_creator.Start();

    }

    void Update()
    {
        UpdatePlayerPos();
        UpdateRotation();
        PlayerDetected();
        AmIAlive();

        switch (state)
        {
            case MYSTATE.IDLE:

                if (player_detected)
                {
                    state = MYSTATE.MOVE_TO_PLAYER;
                    CalculateAdjacentPath(player_x, player_y);

                    enemy_anim = GetLinkedObject("enemy_anim");
                    animation_controller = enemy_anim.GetComponent<CompAnimation>();
                    animation_controller.SetTransition("ToChase");
                }

                break;

            case MYSTATE.MOVE_TO_PLAYER:

                if (ForgotPlayer())
                {
                    state = MYSTATE.RETURN_TO_START;
                    CalculatePath(init_tile_x, init_tile_y);

                    enemy_anim = GetLinkedObject("enemy_anim");
                    animation_controller = enemy_anim.GetComponent<CompAnimation>();
                    animation_controller.SetTransition("ToPatrol");
                }

                if (!path_reached)
                {
                    MoveNextTile();

                    //Check if the player has moved
                    if (pathfinder_timer >= 1.0f)
                    {
                        if (player_x != memory_player_x || player_y != memory_player_y)
                        {
                            CalculateAdjacentPath(player_x, player_y);
                        }
                        pathfinder_timer = 0.0f;
                    }
                    else
                    {
                        pathfinder_timer += Time.DeltaTime();
                    }
                }
                else
                {
                    state = MYSTATE.ATTACK;

                    enemy_anim = GetLinkedObject("enemy_anim");
                    animation_controller = enemy_anim.GetComponent<CompAnimation>();
                    animation_controller.SetTransition("ToAttack");
                }



                break;

            case MYSTATE.ATTACK:

                MYDIRECTION player_dir = CheckRotation(player_x, player_y);

                if (my_dir == (int)player_dir)
                {
                    if (RecheablePlayer())
                    {
                        enemy_anim = GetLinkedObject("enemy_anim");
                        animation_controller = enemy_anim.GetComponent<CompAnimation>();

                        if (animation_controller.IsAnimationStopped("Attack"))
                        {
                            Attack();
                            state = MYSTATE.MOVE_TO_PLAYER;
                        }
                    }
                    else
                    {
                        CalculateAdjacentPath(player_x, player_y);
                        state = MYSTATE.MOVE_TO_PLAYER;

                        enemy_anim = GetLinkedObject("enemy_anim");
                        animation_controller = enemy_anim.GetComponent<CompAnimation>();
                        animation_controller.SetTransition("ToChase");
                    }

                }
                else
                {
                    Rotate(player_dir);
                }

                break;

            case MYSTATE.RETURN_TO_START:

                if (!path_reached)
                {
                    MoveNextTile();

                    if (player_detected)
                    {
                        state = MYSTATE.MOVE_TO_PLAYER;
                        CalculatePath(player_x, player_y);

                        enemy_anim = GetLinkedObject("enemy_anim");
                        animation_controller = enemy_anim.GetComponent<CompAnimation>();
                        animation_controller.SetTransition("ToChase");
                    }
                }
                else
                {
                    state = MYSTATE.IDLE;

                    enemy_anim = GetLinkedObject("enemy_anim");
                    animation_controller = enemy_anim.GetComponent<CompAnimation>();
                    animation_controller.SetTransition("ToIdle");
                }

                break;

            case MYSTATE.DEAD:

                enemy_anim = GetLinkedObject("enemy_anim");
                animation_controller = enemy_anim.GetComponent<CompAnimation>();
                animation_controller.SetTransition("ToDie");

                break;
        }


    }

    void MoveNextTile()
    {


        if (my_path.Count == 0)
        {
            path_reached = true;
            GetLinkedObject("this_obj_enemy").GetComponent<Transform>().SetPosition(new Vector3(current_x * tile_size, GetLinkedObject("this_obj_enemy").GetComponent<Transform>().position.y, current_y * tile_size));

            return;
        }

        MYDIRECTION new_dir = CheckRotation(my_path[0].GetTileX(), my_path[0].GetTileY());

        if (my_dir == (int)new_dir || new_dir == MYDIRECTION.NO_DIR)
        {
            if (my_dir == (int)MYDIRECTION.EAST || my_dir == (int)MYDIRECTION.WEST)
                GetComponent<Transform>().local_position += GetComponent<Transform>().GetForwardVector().Normalized * -movement_speed * Time.DeltaTime();
            else GetComponent<Transform>().local_position += GetComponent<Transform>().GetForwardVector().Normalized * movement_speed * Time.DeltaTime();

            if (ReachedTile())
            {
                current_x = my_path[0].GetTileX();
                current_y = my_path[0].GetTileY();

                //UpdateCollider();

                my_path.Remove(my_path[0]);
            }


        }
        else
        {
            Rotate(new_dir);
        }


    }

    void CalculateAdjacentPath(int pos_x, int pos_y)
    {
        //Clean the path and recalculate
        my_path.Clear();
        my_path = path_creator.CalculatePath(new PathNode(current_x, current_y), new PathNode(pos_x, pos_y));

        //Go to Adjacent tile
        my_path.Remove(my_path[my_path.Count - 1]);

        memory_player_x = pos_x;
        memory_player_y = pos_y;

        path_reached = false;
    }

    void CalculatePath(int pos_x, int pos_y)
    {
        //Clean the path and recalculate
        my_path.Clear();
        my_path = path_creator.CalculatePath(new PathNode(current_x, current_y), new PathNode(pos_x, pos_y));

        path_reached = false;
    }

    void Rotate(MYDIRECTION dir_to_go)
    {
        switch (my_dir)
        {
            case (int)MYDIRECTION.NORTH:

                if (dir_to_go == MYDIRECTION.EAST || dir_to_go == MYDIRECTION.SOUTH)
                {
                    if (my_dir != (int)MYDIRECTION.EAST)
                        RotateAngle(-rotation_speed * Time.DeltaTime());

                    return;
                }

                if (dir_to_go == MYDIRECTION.WEST)
                {
                    if (my_dir != (int)MYDIRECTION.WEST)
                        RotateAngle(rotation_speed * Time.DeltaTime());

                    return;
                }

                break;

            case (int)MYDIRECTION.SOUTH:

                if (dir_to_go == MYDIRECTION.EAST || dir_to_go == MYDIRECTION.NORTH)
                {
                    if (my_dir != (int)MYDIRECTION.EAST)
                        RotateAngle(rotation_speed * Time.DeltaTime());

                    return;
                }

                if (dir_to_go == MYDIRECTION.WEST)
                {
                    if (my_dir != (int)MYDIRECTION.WEST)
                        RotateAngle(-rotation_speed * Time.DeltaTime());

                    return;
                }

                break;

            case (int)MYDIRECTION.EAST:

                if (dir_to_go == MYDIRECTION.NORTH || dir_to_go == MYDIRECTION.WEST)
                {
                    if (my_dir != (int)MYDIRECTION.NORTH)
                        RotateAngle(rotation_speed * Time.DeltaTime());
                    return;
                }

                if (dir_to_go == MYDIRECTION.SOUTH)
                {
                    if (my_dir != (int)MYDIRECTION.SOUTH)
                        RotateAngle(-rotation_speed * Time.DeltaTime());

                    return;
                }

                break;

            case (int)MYDIRECTION.WEST:

                if (dir_to_go == MYDIRECTION.NORTH || dir_to_go == MYDIRECTION.EAST)
                {
                    if (my_dir != (int)MYDIRECTION.NORTH)
                        RotateAngle(-rotation_speed * Time.DeltaTime());

                    return;
                }

                if (dir_to_go == MYDIRECTION.SOUTH)
                {
                    if (my_dir != (int)MYDIRECTION.SOUTH)
                        RotateAngle(rotation_speed * Time.DeltaTime());

                    return;
                }

                break;
        }
    }

    void UpdateRotation()
    {
        float y_rotation = (int)GetComponent<Transform>().GetRotation().y;

        if (y_rotation > 360)
        {
            int tmp = (int)(y_rotation / 360);
            y_rotation -= (tmp * 360);
        }
        else
        {
            if (y_rotation < 0 && y_rotation > -360)
            {
                y_rotation += 360;
            }
            else
            {
                int tmp = (int)(y_rotation / 360);
                y_rotation -= (tmp * 360);
                y_rotation += 360;
            }
        }

        if (y_rotation >= -15 && y_rotation <= 15)
        {
            my_dir = (int)MYDIRECTION.NORTH;
            GetComponent<Transform>().SetRotation(new Vector3(0, 0, 0));
            return;
        }

        if (y_rotation >= 255 && y_rotation <= 285)
        {
            my_dir = (int)MYDIRECTION.EAST;
            GetComponent<Transform>().SetRotation(new Vector3(0, 270, 0));
            return;
        }

        if (y_rotation >= 165 && y_rotation <= 195)
        {
            my_dir = (int)MYDIRECTION.SOUTH;
            GetComponent<Transform>().SetRotation(new Vector3(0, 180, 0));
            return;
        }

        if (y_rotation >= 75 && y_rotation <= 105)
        {
            my_dir = (int)MYDIRECTION.WEST;
            GetComponent<Transform>().SetRotation(new Vector3(0, 90, 0));
            return;
        }
    }

    void RotateAngle(float angle_in_deg)
    {
        GetComponent<Transform>().RotateAroundAxis(new Vector3(0, 1, 0), angle_in_deg);
    }

    public MYDIRECTION CheckRotation(uint tile_to_go_x, uint tile_to_go_y)
    {
        int west_east = current_x - (int)tile_to_go_x;
        int north_south = current_y - (int)tile_to_go_y;

        if (west_east > 0)
            return MYDIRECTION.WEST;

        if (west_east < 0)
            return MYDIRECTION.EAST;

        if (north_south > 0)
            return MYDIRECTION.NORTH;

        if (north_south < 0)
            return MYDIRECTION.SOUTH;

        return MYDIRECTION.NO_DIR;
    }

    bool ReachedTile()
    {
        Vector3 my_pos = GetComponent<Transform>().local_position;

        switch (my_dir)
        {
            case (int)MYDIRECTION.NORTH:

                if (my_pos.z <= (my_path[0].GetTileY() * tile_size))
                    return true;

                break;

            case (int)MYDIRECTION.SOUTH:

                if (my_pos.z >= (my_path[0].GetTileY() * tile_size))
                    return true;

                break;

            case (int)MYDIRECTION.EAST:

                if (my_pos.x >= (my_path[0].GetTileX() * tile_size))
                    return true;

                break;

            case (int)MYDIRECTION.WEST:

                if (my_pos.x <= (my_path[0].GetTileX() * tile_size))
                    return true;

                break;
        }
        return false;

    }

    void PlayerDetected()
    {

        float distance = (GetLinkedObject("player_obj").GetComponent<Transform>().GetGlobalPosition() - GetLinkedObject("this_obj_enemy").GetComponent<Transform>().local_position).Length;

        if (distance < detection_radius)
        {
            player_detected = true;
            player_memory_timer = 0.0f;
        }
        else player_detected = false;
    }

    bool RecheablePlayer()
    {
        //Check if the player is in adjacent nodes

        if (player_x == (current_x - 1) && player_y == current_y)
            return true;

        if (player_x == (current_x + 1) && player_y == current_y)
            return true;

        if (player_x == current_x && player_y == (current_y - 1))
            return true;

        if (player_x == current_x && player_y == (current_y + 1))
            return true;

        return false;
    }

    void Attack()
    {
        GetLinkedObject("player_obj").GetComponent<CharacterController>().GetDamage(damage);
        attack_cooldown_timer = 0.0f;
    }

    bool ForgotPlayer()
    {
        if (player_detected == false)
        {
            if (player_memory_timer >= player_time_in_memory)
            {
                return true;
            }
            else
            {
                player_memory_timer += Time.DeltaTime();
                return false;
            }
        }

        return false;
    }

    void UpdatePlayerPos()
    {
        player_x = GetLinkedObject("player_obj").GetComponent<TestMovement>().curr_x;
        player_y = GetLinkedObject("player_obj").GetComponent<TestMovement>().curr_y;
    }

    void UpdateCollider()
    {
        //GetComponent<CompCollider>().MoveKinematic(GetComponent<Transform>().local_position);
    }

    void AmIAlive()
    {
        if (GetComponent<EnemyController>().hp <= 0)
            state = MYSTATE.DEAD;
    }*/
}