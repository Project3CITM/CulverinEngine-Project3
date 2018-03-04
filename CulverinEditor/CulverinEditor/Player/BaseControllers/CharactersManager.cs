using CulverinEditor;
using CulverinEditor.Debug;

public class CharactersManager : CulverinBehaviour
{
    public enum State
    {
        IDLE = 0,
        CHANGING_LEFT,
        CHANGING_RIGHT
    }

    public GameObject current_character;
    public GameObject left_character;
    public GameObject right_character;
    private GameObject temporal_change = null;

    public GameObject player_obj; 
    public GameObject health_obj;
    public GameObject stamina_obj;

    State state = State.IDLE;   // To manage player state

    void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        current_character = GetLinkedObject("current_character");
        left_character = GetLinkedObject("left_character");
        right_character = GetLinkedObject("right_character");

        Debug.Log("Linking objects");
        player_obj = GetLinkedObject("player_obj");
        health_obj = GetLinkedObject("health_obj");
        stamina_obj = GetLinkedObject("stamina_obj");
    }

    void Update()
    {
        switch(state)
        {
            case State.IDLE:
                {
                    if (Input.GetKeyDown(KeyCode.T))
                    {
                        state = State.CHANGING_LEFT;
                        Debug.Log("Pressed T");
                        Debug.Log("Changing Left");
                        //current_character.GetComponent<CharacterController>().SetAnimationTransition("ToOut", true);
                    }
                    else if (Input.GetKeyDown(KeyCode.Y))
                    {
                        state = State.CHANGING_RIGHT;
                        Debug.Log("Pressed Y");
                        Debug.Log("Changing Right");
                        //current_character.GetComponent<CharacterController>().SetAnimationTransition("ToOut", true);
                    }

                    //MANAGE SECONDARY ABILITIES -------------------------------------
                    else if(Input.GetKeyDown(KeyCode.K))
                    {
                        Debug.Log("Left Secondary Ability");
                        left_character.GetComponent<CharacterController>().SecondaryAbility();
                    }
                    else if (Input.GetKeyDown(KeyCode.L))
                    {
                        Debug.Log("Right Secondary Ability");
                        right_character.GetComponent<CharacterController>().SecondaryAbility();
                    }
                    // ---------------------------------------------------------------
                    break;
                }

            case State.CHANGING_LEFT:
                {
                    //if (current_character.GetComponent<CompAnimation>().IsAnimationStopped("Out"))
                    //{
                    //    left_character.GetComponent<CharacterController>().SetAnimationTransition("ToIn", true);
                    //    current_character.GetComponent<CharacterController>().SetPosition(CharacterController.Position.BEHIND);
                    //    left_character.GetComponent<CharacterController>().SetPosition(CharacterController.Position.CURRENT);

                    //    temporal_change = current_character;
                    //    current_character = left_character;
                    //    left_character = temporal_change;

                    //    //Deactivate previous current character HUD
                    //    left_character.GetComponent<CharacterController>().UpdateHUD(false);
                    //    //Activate current character HUD
                    //    current_character.GetComponent<CharacterController>().UpdateHUD(true);

                    //    Debug.Log("current character = " + current_character.GetName());
                    //    state = State.IDLE;
                    //}
                    Debug.Log("Changing Left");

                    ChangeLeft();               

                    Debug.Log("Changed Positions");

                    //Change GameObjects --------------------
                    temporal_change = current_character;
                    current_character = left_character;
                    left_character = temporal_change;

                    Debug.Log("current character = " + current_character.GetName());
                    state = State.IDLE;

                    break;
                }

            case State.CHANGING_RIGHT:
                {
                    //if (current_character.GetComponent<CompAnimation>().IsAnimationStopped("Out"))
                    //{
                    //    right_character.GetComponent<CharacterController>().SetAnimationTransition("ToIn", true);
                    //    current_character.GetComponent<CharacterController>().SetPosition(CharacterController.Position.BEHIND);
                    //    right_character.GetComponent<CharacterController>().SetPosition(CharacterController.Position.CURRENT);

                    //    temporal_change = current_character;
                    //    current_character = right_character;
                    //    right_character = temporal_change;

                    //    //Deactivate previous current character HUD
                    //    right_character.GetComponent<CharacterController>().UpdateHUD(false);
                    //    //Activate current character HUD
                    //    current_character.GetComponent<CharacterController>().UpdateHUD(true);

                    //    Debug.Log("current character = " + current_character.GetName());
                    //    state = State.IDLE;
                    //}

                    Debug.Log("Changing Right");

                    ChangeRight();

                    Debug.Log("Changed Positions");

                    temporal_change = current_character;
                    current_character = right_character;
                    right_character = temporal_change;

                    Debug.Log("current character = " + current_character.GetName());
                    state = State.IDLE;

                    break;
                }
        }
    }

    void ChangeLeft()
    {
        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            Debug.Log("BEHIND JAIME");
            current_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<JaimeController>().UpdateHUD(false);
        }
        else if (current_character.GetName() == "Daenerys")
        {
            Debug.Log("BEHIND DAENERYS");
            current_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<DaenerysController>().UpdateHUD(false);
        }
        else if (current_character.GetName() == "Theon")
        {
            Debug.Log("BEHIND THEON");
            current_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<TheonController>().UpdateHUD(false);
        }

        // LEFT CHARACTER -------------------------------
        if (left_character.GetName() == "Jaime")
        {
            Debug.Log("CURRENT JAIME");
            left_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<JaimeController>().UpdateHUD(true);
        }
        else if (left_character.GetName() == "Daenerys")
        {
            Debug.Log("CURRENT DAENERYS");
            left_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<DaenerysController>().UpdateHUD(true);
        }
        else if (left_character.GetName() == "Theon")
        {
            Debug.Log("CURRENT THEON");
            left_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<TheonController>().UpdateHUD(true);
        }
    }

    void ChangeRight()
    {
        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            current_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<JaimeController>().UpdateHUD(false);
        }
        else if (current_character.GetName() == "Daenerys")
        {
            current_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<DaenerysController>().UpdateHUD(false);
        }
        else if (current_character.GetName() == "Theon")
        {
            current_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<TheonController>().UpdateHUD(false);
        }

        // LEFT CHARACTER -------------------------------
        if (right_character.GetName() == "Jaime")
        {
            right_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<JaimeController>().UpdateHUD(true);
        }
        else if (right_character.GetName() == "Daenerys")
        {
            right_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<DaenerysController>().UpdateHUD(true);
        }
        else if (right_character.GetName() == "Theon")
        {
            right_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<TheonController>().UpdateHUD(true);
        }
    }
}