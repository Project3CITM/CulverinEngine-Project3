using CulverinEditor;
using CulverinEditor.Debug;

public class Code : CulverinBehaviour
{
    public float speedH = 2.0f;
    public float speedV = 2.0f;

    private float yaw = 0.0f;
    private float pitch = 0.0f;

    void Update()
    {

        if (Input.GetMouseButtonRepeat(2))
        {
            yaw = speedH * Input.GetMouseMoutionX();
            pitch = speedV * Input.GetMouseMoutionY();
            transform.RotateAroundAxis(Vector3.Up, yaw * 5 * Time.DeltaTime());
            transform.RotateAroundAxis(Vector3.Right, pitch * 5 * Time.DeltaTime());
        }
    }
}