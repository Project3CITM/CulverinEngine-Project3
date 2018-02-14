using CulverinEditor;
using CulverinEditor.Debug;

//Attach this script to the turret parent object if you want to see it rotate
public class myTurret
{
    public float rotSpeed = 50.0f;
    public Vector3 final_rot;

    void Update()
    {
        //Rotate LEFT
        if (Input.MouseButtonRepeat(1))
        {
            final_rot = (rotSpeed * Time.DeltaTime()) * Vector3.Up;
            GameObject.gameObject.GetComponent<Transform>().RotateAroundAxis(final_rot);
        }

        //Rotate RIGHT
        if (Input.MouseButtonRepeat(3))
        {
            final_rot = (rotSpeed * Time.DeltaTime()) * Vector3.Down;
            GameObject.gameObject.GetComponent<Transform>().RotateAroundAxis(final_rot);
        }

        //Shoot!
        if (Input.KeyDown("Space"))
        {
            Debug.Log("BOOOOM!!!");
        }
    }
}
