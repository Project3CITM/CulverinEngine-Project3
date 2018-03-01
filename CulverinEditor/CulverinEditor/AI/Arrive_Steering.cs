using CulverinEditor;
using System.Collections;
using CulverinEditor.Debug;

public class Arrive_Steering : CulverinBehaviour
{
    public float min_distance = 0.1f;
    public float slow_distance = 5.0f;
    public float stopping_time = 1.0f;
    Vector3 acceleration;
    Movement_Action move;
    bool in_distance = false;

    // Use this for initialization
    void Start()
    {
        move = GetComponent<Movement_Action>();
        Debug.Log("Arrive Start");
        SetEnabled(false);
    }

    public override void OnDisable()
    {
        in_distance = false;
    }

    // Update is called once per frame
    void Update()
    {
        if(GetComponent<Movement_Action>().GetDistanceToTarget() <= slow_distance)
        {
            in_distance = true;
            float acceleration_magnitude = 2 * (slow_distance / (stopping_time * stopping_time) - GetComponent<Movement_Action>().GetCurrentVelocity().Length / stopping_time);
            acceleration = GetComponent<Movement_Action>().GetCurrentVelocity().Normalized * -(GetComponent<Movement_Action>().GetMaxAcceleration() + acceleration_magnitude);
        }

        if (in_distance)
        {
            GetComponent<Movement_Action>().Accelerate(acceleration);
        }
    }
}
