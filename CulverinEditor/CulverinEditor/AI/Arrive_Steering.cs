using CulverinEditor;
using System.Collections;
using CulverinEditor.Debug;

public class Arrive_Steering : CulverinBehaviour
{
    public float min_distance = 0.1f;
    public float slow_distance = 0.5f;
    public float stopping_time = 1.0f;
    Movement_Action move;
    bool in_distance = false;

    // Use this for initialization
    void Start()
    {
        move = GetComponent<Movement_Action>();
        Debug.Log("Arrive Start");
        SetScriptEnabled(false);
    }

    public override void OnDisable()
    {
        in_distance = false;
    }

    // Update is called once per frame
    void Update()
    {
        /*Vector3 acceleration = Vector3.Zero;

        if(GetComponent<Movement_Action>().GetDistanceToTarget() <= slow_distance && !in_distance)
        {
            in_distance = true;
            Vector3 current_velocity = GetComponent<Movement_Action>().GetCurrentVelocity();
            float acceleration_magnitude = 2 * (slow_distance / (stopping_time * stopping_time) - current_velocity.Length / stopping_time);
            acceleration = current_velocity.Normalized * (-1) * (GetComponent<Movement_Action>().GetMaxAcceleration() + acceleration_magnitude);
        }

        if (in_distance)
        {
            GetComponent<Movement_Action>().Accelerate(acceleration);
        }*/
    }
}
