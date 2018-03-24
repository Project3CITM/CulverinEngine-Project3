using CulverinEditor;
using System.Collections;
using CulverinEditor.Debug;

public class Arrive_Steering : CulverinBehaviour
{
    public float min_distance = 0.05f;
    public float slow_distance = 5f;
    public float stopping_time = 1.5f;
    Movement_Action move;
    bool in_range = false;

    // Use this for initialization
    void Start()
    {
        move = GetComponent<Movement_Action>();
        Debug.Log("Arrive Start");
        in_range = false;
        SetEnabled(false);
    }

    void Update()
    {
        Vector3 ideal_velocity = new Vector3(Vector3.Zero);
        Vector3 distance_to_target = new Vector3(GetComponent<Movement_Action>().GetTargetPosition() - transform.GetPosition());
        distance_to_target.y = 0;

        //Out of zone case
        if (distance_to_target.Length > slow_distance)
        {
            if (in_range == true)
                in_range = false;
            return;
        }
        //On slow zone case
        else
        {
            if (in_range == false)
            {
                in_range = true;
                if(move.chase == false)
                    GetComponent<CompAnimation>().SetTransition("ToIdle");
                else
                    GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
                //set patrol/chase blend
            }
                
            ideal_velocity = ((distance_to_target.Normalized * GetComponent<Movement_Action>().GetMaxVelocity() * distance_to_target.Length)) / slow_distance;
        }

        Vector3 deceleration = new Vector3((ideal_velocity - GetComponent<Movement_Action>().GetCurrentVelocity()) * Time.deltaTime);
    
        GetComponent<Movement_Action>().Accelerate(deceleration);
    }
}
   

