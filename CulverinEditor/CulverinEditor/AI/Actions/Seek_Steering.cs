using CulverinEditor;
using CulverinEditor.Debug;

public class Seek_Steering : CulverinBehaviour
{

    Movement_Action move;
    Transform trans;
    void Start()
    {
        SetEnabled(false);
        move = GetComponent<Movement_Action>();
        trans = GetComponent<Transform>();
    }

    void Update()
    {
        Vector3 acceleration = new Vector3(Vector3.Zero);
        Vector3 target_pos = move.GetTargetPosition();
        Vector3 pos = trans.position;

        acceleration.x = target_pos.x - pos.x;
        acceleration.z = target_pos.z - pos.z;

        acceleration = acceleration.Normalized * move.GetMaxAcceleration();

        move.Accelerate(acceleration);
    }
}

