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



        // W A S D player movement

        if (Input.GetInput_KeyRepeat("LVerticalW", "Player"))
        {
            tempz = go_trans.local_position.z + 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(go_trans.local_position.x, go_trans.local_position.y, tempz));
            Debug.Log("W Vertical Up ");

        }
        else if (Input.GetInput_KeyRepeat("LVerticalS", "Player"))
        {
            tempz = go_trans.local_position.z - 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(go_trans.local_position.x, go_trans.local_position.y, tempz));
            Debug.Log("S Vertical Down");
        }

        if (Input.GetInput_KeyRepeat("LHorizontalD", "Player"))
        {
            tempx = go_trans.local_position.x - 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(tempx, go_trans.local_position.y, go_trans.local_position.z));

            Debug.Log("D Horizontal Left");

        }
        else if (Input.GetInput_KeyRepeat("LHorizontalA", "Player"))
        {
            tempx = go_trans.local_position.x + 1 * Time.DeltaTime() * additive;
            go_trans.SetPosition(new Vector3(tempx, go_trans.local_position.y, go_trans.local_position.z));
            Debug.Log("A Horizontal Left");
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

        //A BUTTON

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

        //Y BUTTON

        if (Input.GetInput_KeyDown("FinalSkill", "Player"))
        {
            Debug.Log("Key Down FinalSkill");
        }
        else if (Input.GetInput_KeyUp("FinalSkill", "Player"))
        {
            Debug.Log("Key Up FinalSkill");
        }
        else if (Input.GetInput_KeyRepeat("FinalSkill", "Player"))
        {
            Debug.Log("Key Repeat FinalSkill");
        }

        //B BUTTON && R

        if (Input.GetInput_KeyDown("TriangleR", "Player"))
        {
            Debug.Log("Key Down TriangleR");
        }
        else if (Input.GetInput_KeyUp("TriangleR", "Player"))
        {
            Debug.Log("Key Up TriangleR");
        }
        else if (Input.GetInput_KeyRepeat("TriangleR", "Player"))
        {
            Debug.Log("Key Repeat TriangleR");
        }

        //X BUTTON && TAB

        if (Input.GetInput_KeyDown("TriangleL", "Player"))
        {
            Debug.Log("Key Down TriangleL");
        }
        else if (Input.GetInput_KeyUp("TriangleL", "Player"))
        {
            Debug.Log("Key Up TriangleL");
        }
        else if (Input.GetInput_KeyRepeat("TriangleL", "Player"))
        {
            Debug.Log("Key Repeat TriangleL");
        }

        //RB BUTTON && 1

        if (Input.GetInput_KeyDown("RAttack", "Player"))
        {
            Debug.Log("Key Down RAttack");
        }
        else if (Input.GetInput_KeyUp("RAttack", "Player"))
        {
            Debug.Log("Key Up RAttack");
        }
        else if (Input.GetInput_KeyRepeat("RAttack", "Player"))
        {
            Debug.Log("Key Repeat RAttack");
        }

        //LB BUTTON && 2

        if (Input.GetInput_KeyDown("LAttack", "Player"))
        {
            Debug.Log("Key Down LAttack");
        }
        else if (Input.GetInput_KeyUp("LAttack", "Player"))
        {
            Debug.Log("Key Up LAttack");
        }
        else if (Input.GetInput_KeyRepeat("LAttack", "Player"))
        {
            Debug.Log("Key Repeat LAttack");
        }

        //RT BUTTON

        variation = Input.GetInput_ControllerAxis("RAllyAttack", "Player");
        if (variation > 0.8)
        {
            Debug.Log("RAllyAttack KeyDown Controller");
        }

        //LT BUTTON

        variation = Input.GetInput_ControllerAxis("LAllyAttack", "Player");
        if (variation > 0.8)
        {
            Debug.Log("LAllyAttack KeyDown Controller");
        }


        // 3-4 are RT and LT but in keyboard

        //4 keyboard----

        if (Input.GetInput_KeyDown("RAllyAttack", "Player"))
        {
            Debug.Log("Key Down RAllyAttack");
        }
        else if (Input.GetInput_KeyUp("RAllyAttack", "Player"))
        {
            Debug.Log("Key Up RAllyAttack");
        }
        else if (Input.GetInput_KeyRepeat("RAllyAttack", "Player"))
        {
            Debug.Log("Key Repeat RAllyAttack");
        }

        //3 keyboard----

        if (Input.GetInput_KeyDown("LAllyAttack", "Player"))
        {
            Debug.Log("Key Down LAllyAttack");
        }
        else if (Input.GetInput_KeyUp("LAllyAttack", "Player"))
        {
            Debug.Log("Key Up LAllyAttack");
        }
        else if (Input.GetInput_KeyRepeat("LAllyAttack", "Player"))
        {
            Debug.Log("Key Repeat LAllyAttack");
        }


        //MOUSE BUTTON LEFT CLICK

        if (Input.GetInput_MouseButtonDown("Interact", "Player"))
        {
            Debug.Log("Mouse button left Interact");
        }



    }

}