using CulverinEditor;
using CulverinEditor.Debug;

//Attach this script to the camera and set target the tank parent GameObject
//To follow the tank with the camera
public class CameraController
{
    public GameObject target;

    void Start()
    {
        GameObject.gameObject.GetComponent<Transform>().RotateAroundAxis(30 * Vector3.Right);
    }

    void Update()
    {
        FollowTank();
    }

    public void FollowTank()
    {
        Vector3 cam_pos = target.GetComponent<Transform>().Position;
        cam_pos.z -= 10;
        cam_pos.y += 10;
        GameObject.gameObject.GetComponent<Transform>().Position = cam_pos;
    }
}