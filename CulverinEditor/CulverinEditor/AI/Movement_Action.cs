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

    
    public float max_vel = 1.0f;
    public float max_accel = 1.0f;
    public float max_rot_vel = 1.0f;
    public float max_rot_accel = 1.0f;
    Vector3 current_velocity = Vector3.Zero;
    Vector3 current_acceleration = Vector3.Zero;
    float current_rot_velocity = 0.0f;
    float current_rot_acceleration = 0.0f;
    float arrive_distance = 0.0f;


    public override bool ActionStart()
    {
        Debug.Log("MOVE_START");
        if (path == null) return false;
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        //Velocity calculation
        if(current_acceleration.Length > max_accel)
        {
            current_acceleration = current_acceleration.Normalized * max_accel;
        }
        current_velocity += current_acceleration;

        if(current_velocity.Length > max_vel)
        {
            current_velocity = current_velocity.Normalized * max_vel;
        }

        //Acceleration calculation
        if (Mathf.Abs(current_rot_acceleration) > max_rot_accel)
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
        }

        current_acceleration = Vector3.Zero;
        current_rot_acceleration = 0.0f;

        if(ReachedTile())
        {
            GetComponent<Transform>().SetPosition(new Vector3(path[0].GetTileX() * tile_size, GetLinkedObject("this_obj_enemy").GetComponent<Transform>().position.y, path[0].GetTileY() * tile_size));

            path.Remove(path[0]);

            if (path.Count == 0)
            {
                GetComponent<Arrive_Steering>().SetEnabled(false);
                GetComponent<Seek_Steering>().SetEnabled(false);
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
        GameObject target = GetLinkedObject("myself");
        Debug.Log("A");
        target.GetComponent<Arrive_Steering>();
        Debug.Log("B");
        GetLinkedObject("myself").GetComponent<Arrive_Steering>().GetEnabled();
        Debug.Log("C");
        Debug.Log(GetLinkedObject("myself").GetComponent<Arrive_Steering>().GetEnabled().ToString());

        GetComponent<Arrive_Steering>().SetEnabled(true);
        Debug.Log(GetComponent<Arrive_Steering>().GetEnabled().ToString());
        GetComponent<Seek_Steering>().SetEnabled(true);
        Debug.Log("path_size:" + path.Count.ToString());
    }

    public void Accelerate(Vector3 acceleration)
    {
        current_acceleration += acceleration;
    }

    public void Rotate(float rotation)
    {
        current_rot_acceleration += rotation;
    }

    public bool ReachedTile()
    {
        Vector3 my_pos = GetComponent<Transform>().local_position;

        Vector3 tile_pos = GetTargetPosition();

        float distance_to_target = (my_pos - tile_pos).Length;

        return (distance_to_target < arrive_distance);
    }

    public Vector3 GetTargetPosition()
    {
        return new Vector3(path[0].GetTileX() * tile_size, GetComponent<Transform>().local_position.y, path[0].GetTileY() * tile_size);
    }

    public void GetCurrentTile(out int x, out int y)
    {
        x = (int)((float)GetComponent<Transform>().local_position.x / tile_size);
        y = (int)((float)GetComponent<Transform>().local_position.z / tile_size);
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

