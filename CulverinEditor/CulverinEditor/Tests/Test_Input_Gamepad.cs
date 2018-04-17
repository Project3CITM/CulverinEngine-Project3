using CulverinEditor;
using CulverinEditor.Debug;


public class Test_Input_Gamepad : CulverinBehaviour
{

    Transform go_trans;
    public float additive=1.0f;
    void Start()
    {
        go_trans = GetComponent<Transform>();
    }

    void Update()
    {
        go_trans = GetComponent<Transform>();


        /*
         
        -------------IMPORTANT!!!!!!!!!------------------
        This is a test for all gameplay programers to call input inside scripts
         
         
        We have to change one workflow of the input.
        But for now we will have to make two different checks for
        player movement if the player is playing with keyboard or gamepad.
        GetInput_ControllerAxis -> For gamecontroller axis player movement
        GetInput_KeyDown -> For keyboard WASD player movement

        And also two checks for RB and LB sticks of the gamecontroller that the
        function is -> GetInput_ControllerAxis
        And for keyboard are keys so the function is -> GetInput_KeyDown

        */

        float tempx;
        float variation = 0.0f;
        variation = Input.GetInput_ControllerAxis("LHorizontal", "Player");
       if (variation > 0.8)
        {
            tempx = go_trans.local_position.x - 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(tempx, go_trans.local_position.y, go_trans.local_position.z));            
        }
        else if (variation < -0.8)
        {
            tempx = go_trans.local_position.x + 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(tempx, go_trans.local_position.y, go_trans.local_position.z));
        }


        variation = Input.GetInput_ControllerAxis("RHorizontal", "Player");

        // Joystick Vertical

        float tempz=0;
        variation = Input.GetInput_ControllerAxis("LVertical", "Player");
        if (variation < -0.8)
        {
            tempz = go_trans.local_position.z + 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(go_trans.local_position.x, go_trans.local_position.y, tempz));
        }
        else if (variation > 0.8)
        {
            tempz = go_trans.local_position.z - 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(go_trans.local_position.x, go_trans.local_position.y, tempz));
        }

        // W A S D player movement

        if (Input.GetInput_KeyRepeat("LVerticalW", "Player"))
        {
            tempz = go_trans.local_position.z + 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(go_trans.local_position.x, go_trans.local_position.y, tempz));
        }
        else if (Input.GetInput_KeyRepeat("LVerticalS", "Player"))
        {
            tempz = go_trans.local_position.z - 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(go_trans.local_position.x, go_trans.local_position.y, tempz));
        }

        if (Input.GetInput_KeyRepeat("LHorizontalD", "Player"))
        {
            tempx = go_trans.local_position.x - 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(tempx, go_trans.local_position.y, go_trans.local_position.z));
        }
        else if (Input.GetInput_KeyRepeat("LHorizontalA", "Player"))
        {
            tempx = go_trans.local_position.x + 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(tempx, go_trans.local_position.y, go_trans.local_position.z));
        }
    }

}