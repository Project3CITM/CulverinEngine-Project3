using CulverinEditor;
using System.Collections;
using CulverinEditor.Debug;

public class Arrive_Steering : CulverinBehaviour
{
    public float min_distance = 0.05f;
    public float slow_distance = 0.9f;
    public float stopping_time = 0.3f;
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
        Vector3 acceleration = Vector3.Zero;
        Vector3 ideal_velocity = new Vector3(Vector3.Zero);
        Vector3 distance_to_target = new Vector3(GetComponent<Movement_Action>().GetTargetPosition() - transform.GetPosition());
        distance_to_target.y = 0;
        
        //On stop zone case
        if (distance_to_target.Length <= min_distance)
        {
            GetComponent<Movement_Action>().SetCurrentVelocity(Vector3.Zero);
            GetComponent<Movement_Action>().Accelerate(new Vector3(GetComponent<Movement_Action>().GetCurrentAcceleration() * -1));
            return;
        }

        //Out of zone case
        else if (distance_to_target.Length > slow_distance)
        {
            GetComponent<Movement_Action>().Accelerate(acceleration);
            return;
        }
        //On slow zone case
        else
        {
            ideal_velocity = ((distance_to_target.Normalized * GetComponent<Movement_Action>().GetMaxVelocity() * distance_to_target.Length)) / slow_distance;
        }

        Vector3 deceleration = new Vector3(ideal_velocity - GetComponent<Movement_Action>().GetCurrentVelocity());
    
        GetComponent<Movement_Action>().Accelerate(deceleration);
    }
}
   

