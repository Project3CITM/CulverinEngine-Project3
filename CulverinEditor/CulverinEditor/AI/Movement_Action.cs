using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;
using System.Collections.Generic;

public class Movement_Action : Action
{
    public GameObject   map;
    public GameObject myself;
    public float        tile_size = 0.0f;
    List<PathNode>      path = null;

    public float max_vel = 3.0f;
    public float max_accel = 1.0f;
    public float max_rot_vel = 1.0f;
    public float max_rot_accel = 1.0f;
    Vector3 current_velocity = Vector3.Zero;
    Vector3 current_acceleration = Vector3.Zero;
    float current_rot_velocity = 0.0f;
    float current_rot_acceleration = 0.0f;
    float arrive_distance = 0.1f;

    void Start()
    {
        path = new List<PathNode>();
    }

    public Movement_Action()
    {
        action_type = ACTION_TYPE.MOVE_ACTION;
        Debug.Log("Move Act");
    }

    public override bool ActionStart()
    {
        Debug.Log("MOVE_START");
        if (path == null)
        {
            Debug.Log("Move: Path == null");
            return false;
        }
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        //Debug.Log("MOVE_UPDATE");

        //Velocity calculation
        if (current_acceleration.Length > max_accel)
        {
            current_acceleration = current_acceleration.Normalized * max_accel;
        }
        current_velocity.x = current_velocity.x + current_acceleration.x;
        current_velocity.z = current_velocity.z + current_acceleration.z;

        if (current_velocity.Length > max_vel)
        {
            current_velocity = current_velocity.Normalized * max_vel;
        }

        //Acceleration calculation
        /*if (Mathf.Abs(current_rot_acceleration) > max_rot_accel)
        {
            if (current_rot_acceleration > 0)
            {
                current_rot_acceleration = max_rot_accel;
            }
            else current_rot_acceleration = -max_rot_accel;
        }
        current_rot_velocity += current_rot_acceleration;

        if (Mathf.Abs(current_rot_velocity) > max_rot_vel)
        {
            if (current_rot_velocity > 0)
            {
                current_rot_velocity = max_rot_vel;
            }
            else current_rot_velocity = -max_rot_vel;
        }*/

        Vector3 local_pos = GetComponent<Transform>().local_position;
        float dt = Time.DeltaTime();
        local_pos.x = local_pos.x + (current_velocity.x * dt);
        local_pos.z = local_pos.z + (current_velocity.z * dt);
        GetComponent<Transform>().local_position = local_pos;
        //GetComponent<Transform>().RotateAroundAxis(Vector3.Up, current_rot_velocity);

        current_acceleration = Vector3.Zero;
        current_rot_acceleration = 0.0f;

        if(ReachedTile())
        {
            local_pos.x = path[0].GetTileX() * tile_size;
            local_pos.z = path[0].GetTileY() * tile_size;
            GetComponent<Transform>().local_position = local_pos;
            path.Remove(path[0]);

            if (path.Count == 0)
            {
                GetComponent<Arrive_Steering>().SetScriptEnabled(false);
                GetComponent<Seek_Steering>().SetScriptEnabled(false);
                return ACTION_RESULT.AR_SUCCESS;
            }
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {  
        return false;
    }

    public void GoTo(int cur_x, int cur_y, int obj_x, int obj_y)
    {
        path = GetLinkedObject("map").GetComponent<Pathfinder>().CalculatePath(new PathNode(cur_x, cur_y), new PathNode(obj_x, obj_y));
        Debug.Log("path_size:" + path.Count.ToString());
        GetLinkedObject("myself").GetComponent<Arrive_Steering>().SetScriptEnabled(true);
        GetLinkedObject("myself").GetComponent<Seek_Steering>().SetScriptEnabled(true);
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

    public bool ReachedTile()
    {
        Vector3 my_pos = GetComponent<Transform>().local_position;
        Vector3 tile_pos = GetTargetPosition();

        Vector3 result = Vector3.Zero;
        result.x = tile_pos.x - my_pos.x;
        result.z = tile_pos.z - my_pos.z;

        float distance_to_target = result.Length;

        return (distance_to_target < arrive_distance);
    }

    public Vector3 GetTargetPosition()
    {
        Vector3 result = Vector3.Zero;
        result.x = path[0].GetTileX() * tile_size;
        result.y = GetComponent<Transform>().local_position.y;
        result.z = path[0].GetTileY() * tile_size;
        return result;
    }

    public void GetCurrentTile(out int x, out int y)
    {
        Debug.Log("Local pos X: " + GetComponent<Transform>().local_position.x.ToString() + " Y: " + GetComponent<Transform>().local_position.y.ToString());
        x = (int)((float)GetComponent<Transform>().local_position.x / tile_size);
        y = (int)((float)GetComponent<Transform>().local_position.z / tile_size);
    }

    public int GetCurrentTileX()
    {
        return (int)((float)GetComponent<Transform>().local_position.x / tile_size);
    }

    public int GetCurrentTileY()
    {
        return (int)((float)GetComponent<Transform>().local_position.z / tile_size);
    }

    public Vector3 GetCurrentVelocity()
    {
        return current_velocity;
    }

    public float GetMaxAcceleration()
    {
        return max_accel;
    }

    public float GetDistanceToTarget()
    {
        return ((GetComponent<Transform>().local_position) - (GetTargetPosition())).Length;
    }
}

