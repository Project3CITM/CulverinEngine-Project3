using CulverinEditor;
using System.Collections;
using CulverinEditor.Debug;

public class Arrive_Steering : CulverinBehaviour
{
    public float min_distance = 0.05f;
    public float slow_distance = 5f;
    public float stopping_time = 1.5f;
    Movement_Action move;

    // Use this for initialization
    void Start()
    {
        move = GetComponent<Movement_Action>();
        Debug.Log("Arrive Start");
        SetEnabled(false);
    }

    void Update()
    {
        Vector3 ideal_velocity = new Vector3(Vector3.Zero);
        Vector3 distance_to_target = new Vector3(GetComponent<Movement_Action>().GetTargetPosition() - transform.GetPosition());
        distance_to_target.y = 0;
        
        //On stop zone case
        if (distance_to_target.Length <= min_distance)
        {
            GetComponent<Movement_Action>().SetCurrentVelocity(new Vector3(Vector3.Zero));
            GetComponent<Movement_Action>().Accelerate(new Vector3(GetComponent<Movement_Action>().GetCurrentAcceleration() * -1));
            return;
        }

        //Out of zone case
        else if (distance_to_target.Length > slow_distance)
            return;

        //On slow zone case
        else
        {
            ideal_velocity = ((distance_to_target.Normalized * GetComponent<Movement_Action>().GetMaxVelocity() * distance_to_target.Length)) / slow_distance;
        }

        Vector3 deceleration = new Vector3(ideal_velocity - GetComponent<Movement_Action>().GetCurrentVelocity()) * Time.deltaTime;
    
        GetComponent<Movement_Action>().Accelerate(deceleration);
    }
}
   

