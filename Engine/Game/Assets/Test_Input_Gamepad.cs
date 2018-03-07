using CulverinEditor;
using CulverinEditor.Debug;


public class Test_Input_Gamepad : CulverinBehaviour
{

    Transform go_trans;
    public float additive=1.0f;
    void Start()
    {
        Debug.Log("Start Gamepad");
        go_trans = GetComponent<Transform>();
    }

    void Update()
    {
        go_trans = GetComponent<Transform>();

        float tempx;
        float variation = 0.0f;
        variation = Input.GetInput_ControllerAxis("LHorizontal", "Player");
       if (variation > 0.8)
        {
            tempx = go_trans.local_position.x - 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(tempx, go_trans.local_position.y, go_trans.local_position.z));

           // Debug.Log("Left Joystick Horizontal Right");
            
        }
        else if (variation < -0.8)
        {
            tempx = go_trans.local_position.x + 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(tempx, go_trans.local_position.y, go_trans.local_position.z));
            Debug.Log("Left Joystick Horizontal Left");
        }


        variation = Input.GetInput_ControllerAxis("RHorizontal", "Player");
          // Debug.Log("Right Joystick Horizontal Right");
        if (variation > 0.8)
        {
            Debug.Log("Right Joystick Horizontal Right");
        }
        else if (variation < -0.8)
        {
            Debug.Log("Right Joystick Horizontal Left");
        }

        // Joystick Vertical

        float tempz=0;
        variation = Input.GetInput_ControllerAxis("LVertical", "Player");
        if (variation < -0.8)
        {
            tempz = go_trans.local_position.z + 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(go_trans.local_position.x, go_trans.local_position.y, tempz));
            Debug.Log("Left Joystick Vertical Up ");
        }
        else if (variation > 0.8)
        {
            tempz = go_trans.local_position.z - 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(go_trans.local_position.x, go_trans.local_position.y, tempz));
            Debug.Log("Left Joystick Vertical Down");
        }



        variation = Input.GetInput_ControllerAxis("RVertical", "Player");
        if (variation < -0.8)
        {
            //go_trans.local_position += (go_trans.local_position * variation * additive);
            Debug.Log("Right Joystick Vertical Up ");
        }
        else if (variation > 0.8)
        {
            //go_trans.local_position += (go_trans.local_position * variation * additive);
            Debug.Log("Right Joystick Vertical Down");
        }


        if (Input.GetInput_KeyDown("Interact","Player"))
        {
            Debug.Log("Key Down interact");
        }
        else if(Input.GetInput_KeyUp("Interact", "Player"))
        {
            Debug.Log("Key Up interact");
        }
        else if (Input.GetInput_KeyRepeat("Interact", "Player"))
        {
            Debug.Log("Key Repeat interact");
        }

    }

}