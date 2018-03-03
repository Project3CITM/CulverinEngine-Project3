using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;
using System.Collections.Generic;

public class Movement_Action : Action
{
    public GameObject   map;
    public GameObject   myself;
    public float        tile_size = 0.0f;
    List<PathNode>      path = null;

    enum Motion_State
    {
        MS_NO_STATE,
        MS_MOVE,
        MS_ROTATE
    }

    Motion_State state = Motion_State.MS_NO_STATE;

    public float max_vel = 3.0f;
    public float max_accel = 1.0f;
    public float max_rot_vel = 1.0f;
    public float max_rot_accel = 1.0f;
    Vector3 current_velocity = new Vector3(Vector3.Zero);
    Vector3 current_acceleration = new Vector3(Vector3.Zero);
    float current_rot_velocity = 0.0f;
    float current_rot_acceleration = 0.0f;
    float arrive_distance = 0.1f;
    float rot_margin = 0.5f;

    void Start()
    {
        map = GetLinkedObject("map");
        path = new List<PathNode>();
    }

    public Movement_Action()
    {
        action_type = ACTION_TYPE.MOVE_ACTION;
    }

    public override bool ActionStart()
    {
        if (path == null)
        {
            Debug.Log("Move: Path == null");
            return false;
        }
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        switch (state)
        {
            case Motion_State.MS_MOVE:
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

                Vector3 local_pos = GetComponent<Transform>().local_position;
                float dt = Time.DeltaTime();
                local_pos.x = local_pos.x + (current_velocity.x * dt);
                local_pos.z = local_pos.z + (current_velocity.z * dt);
                GetComponent<Transform>().local_position = local_pos;

                current_acceleration = new Vector3(Vector3.Zero);
                current_rot_acceleration = 0.0f;

                if (ReachedTile())
                {
                    local_pos.x = path[0].GetTileX() * tile_size;
                    local_pos.z = path[0].GetTileY() * tile_size;
                    GetComponent<Transform>().local_position = local_pos;
                    path.Remove(path[0]);

                    if (path.Count == 0)
                    {
                        GetComponent<Arrive_Steering>().SetScriptEnabled(false);
                        GetComponent<Seek_Steering>().SetScriptEnabled(false);
                        SetState();
                    }
                }
                break;

            case Motion_State.MS_ROTATE:
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

                GetComponent<Transform>().RotateAroundAxis(Vector3.Up, current_rot_velocity);

                current_rot_acceleration = 0.0f;

                if(FinishedRotation())
                {
                    GetComponent<Align_Steering>().SetScriptEnabled(false);

                    Vector3 obj_vec = GetComponent<Transform>().position - GetTargetPosition();
                    GetComponent<Transform>().forward = (obj_vec / obj_vec.Length) * GetComponent<Transform>().forward.Length;
                    SetState();
                }

                break;
        }

        if(state == Motion_State.MS_NO_STATE)
            return ACTION_RESULT.AR_SUCCESS;

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {  
        return false;
    }

    public void GoTo(int cur_x, int cur_y, int obj_x, int obj_y)
    {
        path = map.GetComponent<Pathfinder>().CalculatePath(new PathNode(cur_x, cur_y), new PathNode(obj_x, obj_y));
        SetState();
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

        Vector3 result = new Vector3 (Vector3.Zero);
        result.x = tile_pos.x - my_pos.x;
        result.z = tile_pos.z - my_pos.z;

        float distance_to_target = result.Length;

        return (distance_to_target < arrive_distance);
    }

    public Vector3 GetTargetPosition()
    {
        Vector3 result = new Vector3(Vector3.Zero);
        result.x = path[0].GetTileX() * tile_size;
        result.y = GetComponent<Transform>().local_position.y;
        result.z = path[0].GetTileY() * tile_size;
        return result;
    }

    void SetState()
    {
        if (!FinishedRotation())
        {
            state = Motion_State.MS_ROTATE;
            GetComponent<Align_Steering>().SetScriptEnabled(true);
            Debug.Log("State: ROTATE");
            return;
        }
        if (!ReachedTile())
        {
            state = Motion_State.MS_MOVE;
            GetComponent<Arrive_Steering>().SetScriptEnabled(true);
            GetComponent<Seek_Steering>().SetScriptEnabled(true);
            Debug.Log("State: MOVE");
            return;
        }
        Debug.Log("State: NO_STATE");
        state = Motion_State.MS_NO_STATE;
    }

    public float GetDeltaAngle()
    {
        Vector3 forward = GetComponent<Transform>().forward;
        Vector3 obj_vec = GetComponent<Transform>().position - GetTargetPosition();

        float own_angle = Mathf.Atan2(forward.x, forward.z);
        float obj_angle = Mathf.Atan2(obj_vec.x, obj_vec.z);

        float delta = obj_angle - own_angle;

        if (delta > Mathf.PI)
            delta -= 2 * Mathf.PI;

        Debug.Log("Delta Angle: " + delta.ToString());

        return delta;
    }

    bool FinishedRotation()
    {
        float delta_angle = GetDeltaAngle();
        return delta_angle < rot_margin;
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
        return ((GetComponent<Transform>().local_position) - (GetTargetPosition())).Length;
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
}

