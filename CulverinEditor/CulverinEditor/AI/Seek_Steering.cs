using CulverinEditor;
using CulverinEditor.Debug;

public class Seek_Steering : CulverinBehaviour
{
    Movement_Action move;

    void Start()
    {
        move = GetComponent<Movement_Action>();
        Debug.Log("Seek Start");
        SetEnabled(false);
    }

    void Update()
    {
        Vector3 acceleration = GetComponent<Movement_Action>().GetTargetPosition() - GetComponent<Transform>().position;

        acceleration = acceleration.Normalized * move.max_accel;

        move.Accelerate(acceleration);
    }
}

