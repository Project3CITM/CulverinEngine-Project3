using CulverinEditor;
using System.Collections;
using CulverinEditor.Debug;

public class Arrive_Steering : CulverinBehaviour
{
    public float min_distance = 0.25f;
    public float slow_distance = 5f;
    public float stopping_time = 1.5f;
    bool in_range = false;

    // Use this for initialization
    void Start()
    {
        Debug.Log("Arrive Start");
        in_range = false;
        SetEnabled(false);
    }

    void Update()
    {
        Vector3 ideal_velocity = new Vector3(Vector3.Zero);
        Vector3 my_pos = GetComponent<Transform>().position;
        Vector3 tile_pos = GetComponent<Movement_Action>().GetTargetPosition();
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
        else if(distance_to_target.Length < slow_distance && distance_to_target.Length > min_distance)
        {
            if (in_range == false)
            {
                in_range = true;
                if (GetComponent<Movement_Action>().chase == false)
                {
                    GetComponent<CompAnimation>().SetTransition("ToIdle");
                    Debug.Log("Animation To Idle - Arrive");
                }
                else
                {
                    GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
                    Debug.Log("Animation To IdleAttack - Arrive");
                }
                //set patrol/chase blend
            }
                
            ideal_velocity = ((distance_to_target.Normalized * GetComponent<Movement_Action>().GetMaxVelocity() * distance_to_target.Length)) / slow_distance;
        }

        Vector3 deceleration = new Vector3(ideal_velocity - GetComponent<Movement_Action>().GetCurrentVelocity());
    
        GetComponent<Movement_Action>().Accelerate(deceleration);
    }

    public bool ReachedTile()
    {
        Vector3 ideal_velocity = new Vector3(Vector3.Zero);
        Vector3 my_pos = GetComponent<Transform>().position;
        Vector3 tile_pos = GetComponent<Movement_Action>().GetTargetPosition();
        Vector3 distance_to_target = new Vector3(Vector3.Zero);
        distance_to_target.x = tile_pos.x - my_pos.x;
        distance_to_target.z = tile_pos.z - my_pos.z;
        return distance_to_target.Length <= min_distance;        
    }
}
   

