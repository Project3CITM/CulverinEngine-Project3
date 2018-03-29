using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;
using System.Collections.Generic;

public class Movement_Action : Action
{
    public bool         look_at_player = false;
    public GameObject   map;
    public GameObject   player;
    public float        tile_size = 0.0f;
    List<PathNode>      path = null;

    Align_Steering align;
    Arrive_Steering arrive;
    Seek_Steering seek;

    public enum Direction
    {
        DIR_NO_DIR,
        DIR_NORTH,
        DIR_EAST,
        DIR_SOUTH,
        DIR_WEST
    }

    Direction dir = Direction.DIR_NO_DIR;

    public float max_vel = 5.0f;
    public float hurt_max_vel = 2.5f;
    float current_max_vel;

    public float max_accel = 2.0f;
    public float hurt_accel = 1.0f;
    float current_max_accel;

    public float max_rot_vel = 2.0f;
    public float hurt_max_rot_vel = 1.0f;
    float current_max_rot_vel;

    public float max_rot_accel = 0.7f;
    public float hurt_max_rot_accel = 0.3f;
    float current_max_rot_accel;

    Vector3 current_velocity = new Vector3(Vector3.Zero);
    Vector3 current_acceleration = new Vector3(Vector3.Zero);
    float current_rot_velocity = 0.0f;
    float current_rot_acceleration = 0.0f;
    float rot_margin = 0.05f;

    bool rotation_finished = false;
    bool translation_finished = false;

    public bool chase = false;

    float wait_timer = 0.0f;
    public float wait_time = 1.0f;

    public Movement_Action()
    {
        action_type = ACTION_TYPE.MOVE_ACTION;
    }

    public Movement_Action(float speed): base(speed)
    {
        action_type = ACTION_TYPE.MOVE_ACTION;
    }

    void Start()
    {
        look_at_player = false;
        map = GetLinkedObject("map");
        player = GetLinkedObject("player");

        align = GetComponent<Align_Steering>();
        arrive = GetComponent<Arrive_Steering>();
        seek = GetComponent<Seek_Steering>();

        Enemy_BT bt = GetComponent<EnemySword_BT>();
        if(bt == null)
            bt = GetComponent<EnemyShield_BT>();
        if (bt == null)
            bt = GetComponent<EnemySpear_BT>();

        float interpolation = bt.GetCurrentInterpolation();
        current_max_vel = hurt_max_vel + (max_vel - hurt_max_vel) * interpolation;
        current_max_accel = hurt_accel + (max_accel - hurt_accel) * interpolation;
        current_max_rot_vel = hurt_max_rot_vel + (max_rot_vel - hurt_max_rot_vel) * interpolation;
        current_max_rot_accel = hurt_max_rot_accel + (max_rot_accel - hurt_max_rot_accel) * interpolation;

        Vector3 fw = GetComponent<Transform>().GetForwardVector();
        path = new List<PathNode>();
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);

        if (chase == false)
        {
            GetComponent<CompAnimation>().SetTransition("ToPatrol");
            GetComponent<CompAnimation>().SetActiveBlendingClip("Idle");
            Debug.Log("Animation to Patrol");
        }
        else
        {
            GetComponent<CompAnimation>().SetTransition("ToChase");
            GetComponent<CompAnimation>().SetActiveBlendingClip("IdleAttack");
            Debug.Log("Animation to Chase");
        }

        if (path == null)
        {
            Debug.Log("Move: Path == null");                      
            return false;
        }

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        //Movement
        if (translation_finished == false)
        {
            if (current_acceleration.Length > current_max_accel)
            {
                current_acceleration = current_acceleration.Normalized * current_max_accel;
            }
            current_velocity.x = current_velocity.x + current_acceleration.x;
            current_velocity.z = current_velocity.z + current_acceleration.z;

            if (current_velocity.Length > current_max_vel)
            {
                current_velocity = current_velocity.Normalized * current_max_vel;
            }

            float point_in_speed = current_velocity.Length / current_max_vel;
            GetComponent<CompAnimation>().SetActiveBlendingClipWeight((1.0f - point_in_speed) * (1.0f - point_in_speed));

            //Translate
            Vector3 pos = new Vector3(GetComponent<Transform>().position);
            pos.x = pos.x + current_velocity.x * Time.deltaTime;
            pos.z = pos.z + current_velocity.z * Time.deltaTime;
            GetComponent<Transform>().position = pos;

            //Clean
            current_acceleration = new Vector3(Vector3.Zero);

            if (arrive.ReachedTile() == true)
            {
                current_velocity = new Vector3(Vector3.Zero);

                if (interupt != true)
                {
                    if(wait_timer == 0.0f)
                    {
                        arrive.SetEnabled(false);
                        seek.SetEnabled(false);
                    }

                    wait_timer += Time.deltaTime;

                    if (wait_timer >= wait_time)
                    {
                        NextTile();
                        wait_timer = 0.0f;
                    }
                }
                else
                {
                    arrive.SetEnabled(false);
                    seek.SetEnabled(false);
                    translation_finished = true;
                }                
            }
        }

        //Rotation
        if (rotation_finished == false)
        {
            UpdateRotation();

             //Clean
             current_rot_acceleration = 0.0f;

            if (FinishedRotation() == true)
            {
                rotation_finished = true;
                current_rot_velocity = 0.0f;
                Align_Steering align = GetComponent<Align_Steering>();
                align.SetEnabled(false);
                align.Reset();
                Vector3 obj_vec = new Vector3(GetTargetPosition() - GetComponent<Transform>().position);
                GetComponent<Transform>().forward = new Vector3(obj_vec.Normalized * GetComponent<Transform>().forward.Length);

                SetDirection();
            }
        }

        if (rotation_finished == true && translation_finished == true)
        {
            if (interupt == true)
            {
                Debug.Log("Movement Interupted");
                return ACTION_RESULT.AR_FAIL;
            }

            Debug.Log("Movement Finished");
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        Debug.Log("Action End");
        if (chase == false)
        {            
            GetComponent<CompAnimation>().SetTransition("ToIdle");
            Debug.Log("Animation to Idle");
        }
        else
        {
            GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
            Debug.Log("Animation to IdleAttack");
        }

        if (chase != false)
            chase = false;
        interupt = false;
        return false;
    }


    public void UpdateRotation()
    {
        if (Mathf.Abs(current_rot_acceleration) > current_max_rot_accel)
        {
            if (current_rot_acceleration > 0)
            {
                current_rot_acceleration = current_max_rot_accel;
            }
            else
            {
                current_rot_acceleration = -current_max_rot_accel;
            }
        }
        current_rot_velocity += current_rot_acceleration;

        if (Mathf.Abs(current_rot_velocity) > current_max_rot_vel)
        {
            if (current_rot_velocity > 0)
            {
                current_rot_velocity = current_max_rot_vel;
            }
            else
            {
                current_rot_velocity = -current_max_rot_vel;
            }
        }

        //Rotate
        GetComponent<Transform>().RotateAroundAxis(Vector3.Up, current_rot_velocity * Time.deltaTime);
    }


    private void NextTile()
    {
        Vector3 pos = new Vector3(GetComponent<Transform>().position);

        pos.x = path[0].GetTileX() * tile_size;
        pos.z = path[0].GetTileY() * tile_size;
        GetComponent<Transform>().position = pos;
        GetComponent<CompCollider>().MoveKinematic(new Vector3(pos.x, pos.y + 10, pos.z));

        //Tiles
        if (path.Count == 1)
        {
            arrive.SetEnabled(false);
            seek.SetEnabled(false);
            translation_finished = true;
        }
        else
        {
            arrive.SetEnabled(true);
            seek.SetEnabled(true);
            path.Remove(path[0]);
        }

        //Rotation
        if (FinishedRotation() == false)
        {
            GetDeltaAngle();
            rotation_finished = false;
            align.SetEnabled(true);
            align.SetRotation(GetDeltaAngle());
        }
    }

    public void GoTo(int obj_x, int obj_y, bool rot = false)
    {
        int current_x = GetCurrentTileX();
        int current_y = GetCurrentTileY();

        if ((obj_x == current_x && obj_y == current_y) || map.GetComponent<Pathfinder>().IsWalkableTile((uint)obj_x, (uint)obj_y) == false)
        {
            translation_finished = true;
            arrive.SetEnabled(false);
            seek.SetEnabled(false);
        }
        else
        {
            path.Clear();

            arrive.SetEnabled(true);
            seek.SetEnabled(true);

            translation_finished = false;
            rotation_finished = false;

            path = map.GetComponent<Pathfinder>().CalculatePath(new PathNode(current_x, current_y), new PathNode(obj_x, obj_y));
            look_at_player = rot;

            if (path.Count > 1)
            {
                if (arrive.ReachedTile())
                {
                    NextTile();
                }
            }
            else
            {
                if (arrive.ReachedTile())
                {
                    translation_finished = true;
                    arrive.SetEnabled(false);
                    seek.SetEnabled(false);
                }
            }
        }

        if (FinishedRotation() == false)
        {
            rotation_finished = false;
            align.SetEnabled(true);
            align.SetRotation(GetDeltaAngle());
            GetDeltaAngle();
        }
        else
        {
            rotation_finished = true;
            align.SetEnabled(false);
        }
    }

    public void GoTo(PathNode obj, bool rot = false)
    {
        GoTo(obj.GetTileX(), obj.GetTileY());
    }

    private void GoToPrevious(int obj_x, int obj_y, uint range, bool chase = false)    // Sets a path to the previous tile of your objective // Useful for chasing the player
    {
        this.chase = chase;
        Pathfinder pf = map.GetComponent<Pathfinder>();
        path.Clear();
        List<PathNode> adjacent_walkable_tiles = pf.GetWalkableAdjacents(new PathNode(obj_x, obj_y), range);
        int current_x = GetCurrentTileX();
        int current_y = GetCurrentTileY();

        if (adjacent_walkable_tiles.Count >= 0)
        {
            PathNode closest = adjacent_walkable_tiles[0];
            int closest_distance = Mathf.Abs(closest.GetTileX() - current_x) + Mathf.Abs(closest.GetTileY() - current_y);

            if (adjacent_walkable_tiles.Count > 1)
            {
                for (int i = 1; i < adjacent_walkable_tiles.Count; i++)
                {
                    int x_distance = Mathf.Abs(adjacent_walkable_tiles[i].GetTileX() - current_x);
                    int y_distance = Mathf.Abs(adjacent_walkable_tiles[i].GetTileY() - current_y);

                    int distance = x_distance + y_distance;

                    if (distance < closest_distance)
                    {
                        closest = adjacent_walkable_tiles[i];
                        closest_distance = distance;
                    }
                }
            }

            GoTo(closest);
        }
    }

    public void GoToPlayer()
    {
        player.GetComponent<MovementController>().GetPlayerPos(out int x, out int y);

        Enemy_BT bt = GetComponent<EnemySword_BT>();
        if (bt == null)
            bt = GetComponent<EnemyShield_BT>();
        if (bt == null)
            bt = GetComponent<EnemySpear_BT>();

        GoToPrevious(x, y, (uint)bt.range, true);
    }

    public void Accelerate(Vector3 acceleration)    
    {
        current_acceleration.x = current_acceleration.x + acceleration.x;
        current_acceleration.z = current_acceleration.z + acceleration.z;
    }

    public void Rotate(float rotation)
    {
        current_rot_acceleration = current_rot_acceleration + rotation;
    }

    public Vector3 GetTargetPosition()
    {
        Vector3 result = new Vector3(Vector3.Zero);
        if (path.Count > 0)
        {
            result.x = path[0].GetTileX() * tile_size;
            result.y = GetComponent<Transform>().position.y;
            result.z = path[0].GetTileY() * tile_size;
        }
        else
        {
            result = GetComponent<Transform>().position;
        }

        return result;
    }

    public float GetDeltaAngle(bool db = false)
    {
        if (look_at_player == false)
        {
            Vector3 forward = new Vector3(GetComponent<Transform>().GetForwardVector());
            Vector3 pos = new Vector3(GetComponent<Transform>().position);
            Vector3 target_pos = new Vector3(GetTargetPosition());
            Vector3 obj_vec = new Vector3(target_pos - pos);

            if(pos == target_pos)
            {
                Debug.Log("[error] Target and Current positions are equal");
                return 0.0f;
            }

            float delta = Vector3.AngleBetweenXZ(forward, obj_vec);

            if (delta > Mathf.PI)
                delta = delta - 2 * Mathf.PI;
            if (delta < (-Mathf.PI))
                delta = delta + 2 * Mathf.PI;

            if (db)
            {
                Debug.Log("Forward: " + forward);
                Debug.Log("Position: " + pos);
                Debug.Log("Target Position: " + GetTargetPosition());
                Debug.Log("Objective Vector: " + obj_vec);
                Debug.Log("Delta Angle: " + delta);
            }

            return delta;
        }
        else
        {
            Vector3 forward = new Vector3(GetComponent<Transform>().GetForwardVector());
            Vector3 pos = new Vector3(GetComponent<Transform>().position);
            Vector3 player_pos = new Vector3(player.GetComponent<Transform>().position);
            Vector3 obj_vec = new Vector3(player_pos.x - pos.x, pos.y, player_pos.z - pos.z);

            float delta = Vector3.AngleBetweenXZ(forward, obj_vec);

            if (delta > Mathf.PI)
                delta = delta - 2 * Mathf.PI;
            if (delta < (-Mathf.PI))
                delta = delta + 2 * Mathf.PI;

            Direction obj_dir = Direction.DIR_NO_DIR;

            if (delta <= (Mathf.PI / 4) && delta >= -(Mathf.PI / 4))
                obj_dir = Direction.DIR_SOUTH;
            else if (delta >= (Mathf.PI / 4) && delta <= 3 * (Mathf.PI / 4))
                obj_dir = Direction.DIR_EAST;
            else if (delta >= (3 * (Mathf.PI / 4)) || delta <= -(3 * (Mathf.PI / 4)))
                obj_dir = Direction.DIR_NORTH;
            else if (delta <= -(Mathf.PI / 4) && delta >= -(3 * (Mathf.PI / 4)))
                obj_dir = Direction.DIR_WEST;

            float obj_angle = 0.0f;

            if(obj_dir != dir)
            {
                switch (dir)
                {
                    case Direction.DIR_NORTH:
                        switch (obj_dir)
                        {
                            case Direction.DIR_EAST: obj_angle = -(Mathf.PI / 2); break;
                            case Direction.DIR_WEST: obj_angle = (Mathf.PI / 2); break;
                            case Direction.DIR_SOUTH: obj_angle = Mathf.PI; break;
                        }
                    break;
                    case Direction.DIR_EAST:
                        switch (obj_dir)
                        {
                            case Direction.DIR_NORTH: obj_angle = (Mathf.PI / 2); break;
                            case Direction.DIR_WEST: obj_angle = Mathf.PI; break;
                            case Direction.DIR_SOUTH: obj_angle = -(Mathf.PI / 2); break;
                        }
                        break;
                    case Direction.DIR_SOUTH:
                        switch (obj_dir)
                        {
                            case Direction.DIR_EAST: obj_angle = (Mathf.PI / 2); break;
                            case Direction.DIR_WEST: obj_angle = -(Mathf.PI / 2); break;
                            case Direction.DIR_NORTH: obj_angle = Mathf.PI; break;
                        }
                        break;
                    case Direction.DIR_WEST:
                        switch (obj_dir)
                        {
                            case Direction.DIR_EAST: obj_angle = Mathf.PI; break;
                            case Direction.DIR_NORTH: obj_angle = -(Mathf.PI / 2); break;
                            case Direction.DIR_SOUTH: obj_angle = (Mathf.PI / 2); break;
                        }
                        break;
                }

                Debug.Log("Direction:");
                switch (dir)
                {
                    case Direction.DIR_WEST: Debug.Log("West"); break;
                    case Direction.DIR_EAST: Debug.Log("East"); break;
                    case Direction.DIR_NORTH: Debug.Log("North"); break;
                    case Direction.DIR_SOUTH: Debug.Log("South"); break;
                }
                float test = obj_angle - delta;
                Debug.Log(test);
                return test;
            }
            Debug.Log("Retards 4 the win");
            return 0.0f;
        }
    }

    public Direction SetDirection()
    {
        Vector3 forward = new Vector3(GetComponent<Transform>().GetForwardVector());
        float delta = Mathf.Atan2(forward.x, forward.y);

        if (delta > Mathf.PI)
            delta = delta - 2 * Mathf.PI;
        if (delta < (-Mathf.PI))
            delta = delta + 2 * Mathf.PI;

        if (delta <= (Mathf.PI / 4) && delta >= -(Mathf.PI / 4))
            dir = Direction.DIR_SOUTH;
        else if (delta >= (Mathf.PI / 4) && delta <= 3 * (Mathf.PI / 4))
            dir = Direction.DIR_EAST;
        else if (delta >= (3 * (Mathf.PI / 4)) || delta <= -(3 * (Mathf.PI / 4)))
            dir = Direction.DIR_NORTH;
        else if (delta <= -(Mathf.PI / 4) && delta >= -(3 * (Mathf.PI / 4)))
            dir = Direction.DIR_WEST;

        return dir;
    }

    public bool FinishedRotation()
    {
        float delta_angle = GetDeltaAngle();
        return Mathf.Abs(delta_angle) < rot_margin;
    }

    public int GetCurrentTileX()
    {
        return ((int)GetComponent<Transform>().position.x / (int)tile_size);
    }

    public int GetCurrentTileY()
    {
        return ((int)GetComponent<Transform>().position.z / (int)tile_size);
    }

    public Vector3 GetCurrentVelocity()
    {
        return current_velocity;
    }
    
    public float GetCurrentRotVelocity()
    {
        return current_rot_velocity;
    }

    public float GetMaxAcceleration()
    {
        return max_accel;
    }
    
    public float GetDistanceToTarget()
    {
        return ((GetComponent<Transform>().position) - (GetTargetPosition())).Length;
    }

    public void SetCurrentVelocity(Vector3 vel)
    {
        current_velocity = vel;
    }

    public Vector3 GetCurrentAcceleration()
    {
        return current_acceleration;
    }

    public float GetMaxVelocity()
    {
        return max_vel;
    }

    public float GetMaxRotAcceleration()
    {
        return max_rot_accel;
    }

    public float GetRotMargin()
    {
        return rot_margin;
    }

    public bool NextToPlayer()
    {
        player.GetComponent<MovementController>().GetPlayerPos(out int x, out int y);

        return (Mathf.Abs(y - GetCurrentTileY()) + Mathf.Abs(x - GetCurrentTileX())) <= 1;
    }

    public bool IsWalkable(uint tile_x, uint tile_y)
    {
        return map.GetComponent<Pathfinder>().IsWalkableTile(tile_x, tile_y);
    }

}