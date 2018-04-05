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

    PathNode tile = null;

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

    public float max_rot_vel = Mathf.PI;
    public float hurt_max_rot_vel = 1.0f;
    float current_max_rot_vel;

    public float max_rot_accel = 0.05f;
    public float hurt_max_rot_accel = 0.01f;
    float current_max_rot_accel;

    Vector3 current_velocity = new Vector3(Vector3.Zero);
    Vector3 current_acceleration = new Vector3(Vector3.Zero);
    float current_rot_velocity = 0.0f;
    float current_rot_acceleration = 0.0f;

    bool rotation_finished = false;
    bool translation_finished = false;

    public bool chase = false;

    float wait_timer = 0.0f;
    public float wait_time = 1.0f;

    Vector3 look_at_pos = new Vector3(Vector3.Zero);

    public Movement_Action()
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

        //Set Occupied tile in Pathfinder
        tile = new PathNode(0,0);
        tile.SetCoords((int)(GetComponent<Transform>().position.x / tile_size + Mathf.Epsilon), (int)(GetComponent<Transform>().position.z / tile_size + Mathf.Epsilon));
        Debug.Log("Start at Tile: " + tile.GetTileX() + "," + tile.GetTileY());
        map.GetComponent<Pathfinder>().UpdateOccupiedTiles(gameObject.GetName(), tile);
    }

    public override bool ActionStart()
    {
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

        if (path.Count != 0)
        {
            tile = new PathNode(0, 0);
            tile.SetCoords(path[0].GetTileX(), path[0].GetTileY());
            Debug.Log("Moving to: " + tile.GetTileX() + "," + tile.GetTileY());
        }


        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        //Movement
        if (translation_finished == false)
        {
            UpdateTranslation();

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

            if (GetComponent<Align_Steering>().RotationFinished() == true)
            {
                Debug.Log("Sttoping rotation");
                rotation_finished = true;
                current_rot_velocity = 0.0f;
                align.SetEnabled(false);
                look_at_pos.y = GetComponent<Transform>().position.y;
                Vector3 obj_vec = new Vector3(look_at_pos - GetComponent<Transform>().position);
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
        {
            chase = false;
        }

        interupt = false;
        return false;
    }

    private void UpdateRotation()
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
        //Debug.Log("Current Rot Vel: " + current_rot_velocity + "* dt " + Time.deltaTime);
        align.UpdateRotation(current_rot_velocity * Time.deltaTime);

        //Clean
        current_rot_acceleration = 0.0f;
    }

    private void UpdateTranslation()
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
    }

    private void NextTile()
    {
        Vector3 pos = new Vector3(GetComponent<Transform>().position);
        if (path.Count > 0)
        {
            pos.x = path[0].GetTileX() * tile_size;
            pos.z = path[0].GetTileY() * tile_size;
            GetComponent<Transform>().position = pos;

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
                Debug.Log("Moving to Tile: " + path[0].GetTileX() + "," + path[0].GetTileY());
                tile.SetCoords(path[0].GetTileX(), path[0].GetTileY());
                LookAtNextTile();
            }
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

        LookAtNextTile();
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

    public void LookAt(Vector3 target_position)
    {
        look_at_pos = new Vector3(target_position);

        Align_Steering align_scr= GetComponent<Align_Steering>();
        if (align_scr == null) Debug.Log("Align steering null");
        align_scr.SetEnabled(true);

        Vector3 forward = new Vector3(GetComponent<Transform>().GetForwardVector());
        Vector3 pos = new Vector3(GetComponent<Transform>().position);
        Vector3 obj_vec = new Vector3(target_position - pos);

        float delta = Vector3.AngleBetweenXZ(forward, obj_vec);

        if (delta > Mathf.PI)
        {
            delta = delta - 2 * Mathf.PI;
        }
        if (delta < (-Mathf.PI))
        {
            delta = delta + 2 * Mathf.PI;
        }

        if (Mathf.Abs(delta) > align.GetRotMargin())
        {
            rotation_finished = false;
            align.SetEnabled(true);
            align.SetRotation(delta);
        }
    }

    public void LookAtNextTile()
    {
        Vector3 next_tile = new Vector3(GetComponent<Transform>().position);
        if (path != null && path.Count > 0)
        {
            next_tile.x = path[0].GetTileX() * tile_size;
            next_tile.z = path[0].GetTileY() * tile_size;
        }
        LookAt(next_tile);
    }

    public void LookAtPlayer()
    {
        Vector3 target_pos = new Vector3(GetLinkedObject("player").GetComponent<Transform>().position);
        Debug.Log("Look at player pos: " + target_pos);
        LookAt(target_pos);
    }

    public Direction SetDirection()
    {
        Vector3 forward = new Vector3(GetComponent<Transform>().GetForwardVector());
        float delta = Mathf.Atan2(forward.x, forward.z);

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

        Debug.Log("Direction: " + dir);

        return dir;
    }

    public void SetEnemyTile(int x, int y)
    {
        tile.SetCoords(x, y);
    }

    public Direction GetDirection()
    {
        return dir;
    }

    public int GetCurrentTileX()
    {
        return tile.GetTileX();
    }

    public int GetCurrentTileY()
    {
        return tile.GetTileY();
    }

    public PathNode GetCurrentTile()
    {
        return new PathNode(tile.GetTileX(), tile.GetTileY());
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

    public bool NextToPlayer()
    {
        player.GetComponent<MovementController>().GetPlayerPos(out int x, out int y);

        return (Mathf.Abs(y - GetCurrentTileY()) + Mathf.Abs(x - GetCurrentTileX())) <= 1;
    }

    public bool IsWalkable(uint tile_x, uint tile_y)
    {
        return map.GetComponent<Pathfinder>().IsWalkableTile(tile_x, tile_y);
    }

    public void SetInterupt(bool i)
    {
        interupt = i;
    }

    public bool CenteredInTile()
    {
        if (path == null || path.Count == 0) return true;

        if ((Mathf.Abs((GetCurrentTileX() - (path[0].GetTileX() * tile_size))) < arrive.min_distance) && (Mathf.Abs((GetCurrentTileY() - (path[0].GetTileY() * tile_size))) < arrive.min_distance))
            return true;
        
        return false;
    }

}