using CulverinEditor;


public class Seek_Steering : CulverinBehaviour
{
    Movement_Action move;

    void Update()
    {
        Vector3 acceleration = move.GetTargetPosition() - GetComponent<Transform>().position;
        if (acceleration.Length > move.max_accel)
        {
            acceleration = acceleration.Normalized * move.max_accel;
        }

        move.Accelerate(acceleration);
    }
}

