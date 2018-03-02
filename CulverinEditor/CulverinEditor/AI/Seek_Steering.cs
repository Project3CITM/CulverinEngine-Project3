using CulverinEditor;
using CulverinEditor.Debug;

public class Seek_Steering : CulverinBehaviour
{
    Movement_Action move;
    public GameObject myself;

    void Start()
    {
        move = GetComponent<Movement_Action>();
        Debug.Log("Seek Start");
        SetEnabled(false);
    }

    void Update()
    {
        Vector3 acceleration = GetComponent<Movement_Action>().GetTargetPosition();// - GetComponent<Transform>().position;

        Debug.Log(acceleration.ToString());
        Debug.Log(GetComponent<Transform>().GetPosition().ToString());
        Debug.Log(GetComponent<Transform>().local_position.ToString());
        Debug.Log(GetLinkedObject("myself").GetComponent<Transform>().GetGlobalPosition().ToString());
        
        acceleration = acceleration.Normalized * move.GetMaxAcceleration();

        

        move.Accelerate(acceleration);
    }
}

