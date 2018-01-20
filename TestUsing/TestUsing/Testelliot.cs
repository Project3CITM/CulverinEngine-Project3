using CulverinEditor;
using CulverinEditor.Debug;

//Attach this script to the tank parent object if you want to see it rotate
public class myTank
{
    public float movSpeed = 0.0f;
    public float rotSpeed = 0.0f;
    public Vector3 final_mov;
    public Vector3 final_rot;

    void Update()
    {
        if (Input.KeyRepeat("W"))
        {
            final_mov = (movSpeed * Time.DeltaTime()) * Vector3.Forward;
            GameObject.gameObject.GetComponent<Transform>().Position += final_mov;
        }
        if (Input.KeyRepeat("A"))
        {
            final_mov = (movSpeed * Time.DeltaTime()) * Vector3.Left;
            GameObject.gameObject.GetComponent<Transform>().Position -= final_mov;

            final_rot = (rotSpeed * Time.DeltaTime()) * Vector3.Up;
            GameObject.gameObject.GetComponent<Transform>().RotateAroundAxis(final_rot);
        }
        if (Input.KeyRepeat("S"))
        {
            final_mov = (movSpeed * Time.DeltaTime()) * Vector3.Backward;
            GameObject.gameObject.GetComponent<Transform>().Position += final_mov;
        }
        if (Input.KeyRepeat("D"))
        {
            final_mov = (movSpeed * Time.DeltaTime()) * Vector3.Right;
            GameObject.gameObject.GetComponent<Transform>().Position -= final_mov;

            final_rot = (rotSpeed * Time.DeltaTime()) * Vector3.Down;
            GameObject.gameObject.GetComponent<Transform>().RotateAroundAxis(final_rot);
        }
    }
}