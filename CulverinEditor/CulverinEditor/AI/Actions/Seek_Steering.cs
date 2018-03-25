using CulverinEditor;
using CulverinEditor.Debug;

public class Seek_Steering : CulverinBehaviour
{
    void Start()
    {
        Debug.Log("Seek Start");
        SetEnabled(false);
    }

    void Update()
    {
        Vector3 acceleration = new Vector3(Vector3.Zero);
        Vector3 target_pos = GetComponent<Movement_Action>().GetTargetPosition();
        Vector3 pos = GetComponent<Transform>().position;

        acceleration.x = target_pos.x - pos.x;
        acceleration.z = target_pos.z - pos.z;

        acceleration = acceleration.Normalized * GetComponent<Movement_Action>().GetMaxAcceleration();

        GetComponent<Movement_Action>().Accelerate(acceleration);
    }
}

