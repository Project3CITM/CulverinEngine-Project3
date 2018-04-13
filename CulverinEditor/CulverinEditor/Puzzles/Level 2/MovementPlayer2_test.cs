using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

//Attach this script to the tank parent object if you want to see it rotate
public class MovementPlayer2_test : CulverinBehaviour
{

    void Start()
    {

    }

    void Update()
    {
        if (Input.GetKeyRepeat(KeyCode.A)) //Left
        {
            Vector3 temp = transform.position;
            temp.z -= 1;
            transform.SetPosition(temp);
        }
        else if (Input.GetKeyRepeat(KeyCode.D)) //Right
        {
            Vector3 temp = transform.position;
            temp.z += 1;
            transform.SetPosition(temp);
        }
        else if (Input.GetKeyRepeat(KeyCode.W)) //Up
        {
            Vector3 temp = transform.position;
            temp.x += 1;
            transform.SetPosition(temp);
        }
        else if (Input.GetKeyRepeat(KeyCode.S)) //Down
        {
            Vector3 temp = transform.position;
            temp.x -= 1;
            transform.SetPosition(temp);
        }
    }
}