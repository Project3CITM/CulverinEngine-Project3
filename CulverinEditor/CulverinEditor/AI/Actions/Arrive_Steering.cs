using CulverinEditor;
using System.Collections;
using CulverinEditor.Debug;

public class Arrive_Steering : CulverinBehaviour
{
    public float min_distance = 0.25f;
    public float slow_distance = 5f;
    public float stopping_time = 1.5f;
    bool in_range = false;
    Movement_Action move;
    Transform transform;

    // Use this for initialization
    void Start()
    {
        in_range = false;
        SetEnabled(false);
        move = GetComponent<Movement_Action>();
        transform = GetComponent<Transform>();
    }

    void Update()
    {
        Vector3 my_pos = transform.position;
        Vector3 tile_pos = move.GetTargetPosition();
        Vector3 distance_to_target = new Vector3(Vector3.Zero);
        distance_to_target.x = tile_pos.x - my_pos.x;
        distance_to_target.z = tile_pos.z - my_pos.z;

        //Out of zone case
        if (distance_to_target.Length > slow_distance)
        {
            if (in_range == true)
                in_range = false;
            return;
        }
        //On slow zone case
        else if (distance_to_target.Length < slow_distance && distance_to_target.Length > min_distance)
        {
            Vector3 ideal_velocity = distance_to_target.Normalized * move.GetMaxVelocity() * (distance_to_target.Length / slow_distance);
            Vector3 deceleration = (ideal_velocity - move.GetCurrentVelocity());

            if (deceleration.Length > move.GetMaxAcceleration())
                deceleration = deceleration.Normalized * move.GetMaxAcceleration();

            deceleration = deceleration + distance_to_target.Normalized * move.GetMaxAcceleration() * (-1);

            move.Accelerate(deceleration);
        }
    }

    public bool ReachedTile()
    {
        Vector3 ideal_velocity = new Vector3(Vector3.Zero);
        Vector3 my_pos = transform.position;
        Vector3 tile_pos = move.GetTargetPosition();
        Vector3 distance_to_target = new Vector3(Vector3.Zero);
        distance_to_target.x = tile_pos.x - my_pos.x;
        distance_to_target.z = tile_pos.z - my_pos.z;
        return distance_to_target.Length <= min_distance;        
    }
}
   

