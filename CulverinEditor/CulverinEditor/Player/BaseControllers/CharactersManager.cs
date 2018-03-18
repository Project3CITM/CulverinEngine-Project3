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

    public enum Side
    {
        LEFT = 0,
        RIGHT
    }

    //--------------------------------------------------------- THIS IS HARDCODED FOR SCORE SCREEN 
    uint enem_killed;
    public float puzzle_start;
    public bool in_puzzle;
    GameObject bg;
    GameObject coin1;
    GameObject coin2;
    GameObject coin3;

    //SCORE SCREEN END
    public GameObject current_character;
    public GameObject left_character;
    public GameObject right_character;
    private GameObject temporal_change = null;
    //GameObject arrow;
    public GameObject player_obj; 
    public GameObject health_obj;
    public GameObject stamina_obj;
    public GameObject mana_obj;
    public GameObject enemies_obj;

    //To manage secondary abilities buttons
    public GameObject jaime_s_button_obj;
    public GameObject daenerys_s_button_obj;
    public GameObject theon_s_button_obj;

    Shield shield;

    public float puz_max_time;
    //To manage player state
    State state = State.IDLE;   

    //To manage Jaime Secondary Ability
    public bool shield_activated = false;

    void Start()
    {
       
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        current_character = GetLinkedObject("current_character");
        left_character = GetLinkedObject("left_character");
        right_character = GetLinkedObject("right_character");
  
        player_obj = GetLinkedObject("player_obj");
        health_obj = GetLinkedObject("health_obj");
        stamina_obj = GetLinkedObject("stamina_obj");
        mana_obj = GetLinkedObject("mana_obj");
        enemies_obj = GetLinkedObject("enemies_obj");

        jaime_s_button_obj = GetLinkedObject("jaime_s_button_obj");
        daenerys_s_button_obj = GetLinkedObject("daenerys_s_button_obj");
        theon_s_button_obj = GetLinkedObject("theon_s_button_obj");

        SetCurrentPosition();
        //core menu
        //bg = GetLinkedObject("bg");
        //coin1 = GetLinkedObject("coin1");
        //coin2= GetLinkedObject("coin2");
        //coin3= GetLinkedObject("coin3");

        //coin1.SetActive(false);
        //coin2.SetActive(false);
        //coin3.SetActive(false);
        //bg.SetActive(false);
    }

    void Update()
    {
        //if (in_puzzle)
        //{
        //    puzzle_start += Time.deltaTime;
        //}

        switch (state)
        {
            case State.IDLE:
                {
                    //Test Jaime Secondary Ability
                    if (Input.GetKeyDown(KeyCode.Num0))
                    {
                        GetDamage(10);
                    }

                    if(health_obj.GetComponent<Hp>().GetCurrentHealth() <= 0)
                    {
                        if (IsDead(left_character) == false)
                        {
                            state = State.CHANGING_LEFT;
                            CurrentToOut();
                            Debug.Log("Change Left");
                        }
                        else if(IsDead(right_character) == false)
                        {
                            state = State.CHANGING_RIGHT;
                            CurrentToOut();
                            Debug.Log("Change Right");
                        }
                        else
                        {
                            //DEFEAT
                        }
                    }

                    if (Input.GetInput_KeyDown("TriangleR", "Player"))
                    {
                        if (IsIdle() == true)
                        {
                            if (IsDead(right_character) == false)
                            {
                                state = State.CHANGING_RIGHT;
                                CurrentToOut();
                                Debug.Log("Change Right");
                            }
                        }
                    }
                    if (Input.GetInput_KeyDown("TriangleL", "Player"))
                    {
                        if (IsIdle() == true)
                        {
                            if (IsDead(left_character) == false)
                            {
                                state = State.CHANGING_LEFT;
                                CurrentToOut();
                                Debug.Log("Change Left");
                            }
                        }
                    }

                    //MANAGE SECONDARY ABILITIES ------------
                    else if (Input.GetKeyDown(KeyCode.K))
                    {
                        if (IsDead(left_character) == false)
                        {
                            SecondaryAbility(Side.LEFT);
                            Debug.Log("Left S Attack");
                        }
                    }
                    else if (Input.GetKeyDown(KeyCode.L))
                    {
                        if (IsDead(left_character) == false)
                        {
                            SecondaryAbility(Side.RIGHT);
                            Debug.Log("Right S Attack");
                        }
                    }

                    float vari = Input.GetInput_ControllerAxis("LAllyAttack", "Player");

                    if (vari>0.8)
                    {
                        SecondaryAbility(Side.LEFT);
                        
                    }

                    vari = Input.GetInput_ControllerAxis("RAllyAttack", "Player");

                    if (vari>0.8)
                    {
                        SecondaryAbility(Side.RIGHT);
                    }


                    break;
                    // --------------------------------------

                }

            case State.CHANGING_LEFT:
                {
                    if (IsCharacterAnimationStopped(current_character, "Out"))
                    {
                        ChangeLeft();

                        //Change GameObjects --------------------
                        temporal_change = current_character;
                        current_character = left_character;
                        left_character = temporal_change;

                        state = State.IDLE;
                    }

                    break;
                }

            case State.CHANGING_RIGHT:
                {
                    if (IsCharacterAnimationStopped(current_character, "Out"))
                    {
                        ChangeRight();

                        //Change GameObjects --------------------
                        temporal_change = current_character;
                        current_character = right_character;
                        right_character = temporal_change;

                        state = State.IDLE;
                    }
                    break;
                }
        }
    }

    void ChangeLeft()
    {
        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            current_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<JaimeController>().UpdateHUD(false,true);
            current_character.GetComponent<JaimeController>().ToggleMesh(false);
        }
        else if (current_character.GetName() == "Daenerys")
        {
            current_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<DaenerysController>().UpdateHUD(false, true);
            current_character.GetComponent<DaenerysController>().ToggleMesh(false);
        }
        else if (current_character.GetName() == "Theon")
        {
            current_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<TheonController>().UpdateHUD(false, true);
            current_character.GetComponent<TheonController>().ToggleMesh(false);
        }

        // LEFT CHARACTER -------------------------------
        if (left_character.GetName() == "Jaime")
        {
            left_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<JaimeController>().UpdateHUD(true, true);
            left_character.GetComponent<JaimeController>().SetAnimationTransition("ToIn", true);
            left_character.GetComponent<JaimeController>().ToggleMesh(true);
        }
        else if (left_character.GetName() == "Daenerys")
        {
            left_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<DaenerysController>().UpdateHUD(true, true);
            left_character.GetComponent<DaenerysController>().SetAnimationTransition("ToIn", true);
            left_character.GetComponent<DaenerysController>().ToggleMesh(true);
        }
        else if (left_character.GetName() == "Theon")
        {
            left_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<TheonController>().UpdateHUD(true, true);
            left_character.GetComponent<TheonController>().ToggleMesh(true);
            left_character.GetComponent<TheonController>().SetAnimationTransition("ToIn", true);
            
        }
    }

    void ChangeRight()
    {
        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            current_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<JaimeController>().UpdateHUD(false, false);
            current_character.GetComponent<JaimeController>().ToggleMesh(false);
        }
        else if (current_character.GetName() == "Daenerys")
        {
            current_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<DaenerysController>().UpdateHUD(false, false);
            current_character.GetComponent<DaenerysController>().ToggleMesh(false);
        }
        else if (current_character.GetName() == "Theon")
        {
            current_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.BEHIND);
            current_character.GetComponent<TheonController>().UpdateHUD(false, false);
            current_character.GetComponent<TheonController>().ToggleMesh(false);
        }

        // LEFT CHARACTER -------------------------------
        if (right_character.GetName() == "Jaime")
        {
            right_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<JaimeController>().UpdateHUD(true, false);
            right_character.GetComponent<JaimeController>().SetAnimationTransition("ToIn", true);
            right_character.GetComponent<JaimeController>().ToggleMesh(true);
        }
        else if (right_character.GetName() == "Daenerys")
        {
            right_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<DaenerysController>().UpdateHUD(true, false);
            right_character.GetComponent<DaenerysController>().SetAnimationTransition("ToIn", true);
            right_character.GetComponent<DaenerysController>().ToggleMesh(true);
        }
        else if (right_character.GetName() == "Theon")
        {
            right_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<TheonController>().UpdateHUD(true, false);
            right_character.GetComponent<TheonController>().SetAnimationTransition("ToIn", true);
            right_character.GetComponent<TheonController>().ToggleMesh(true);
        }
    }

    void CurrentToOut()
    {
        Vector3 pos = transform.local_position;
        Vector3 vfront = transform.forward;
        Mathf.Round(vfront.x);
        Mathf.Round(vfront.y);
        Mathf.Round(vfront.z);

        JaimeController jcontroller;
        DaenerysController dcontroller;
        TheonController tcontroller;

        if (current_character.GetName() == "Jaime")
        {
            jcontroller = current_character.GetComponent<JaimeController>();
            jcontroller.SetAnimationTransition("ToOut", true);

            if (right_character.GetName() == "Daenerys")
            {
                dcontroller = right_character.GetComponent<DaenerysController>();
                tcontroller = left_character.GetComponent<TheonController>();

            }
            else
            {
                dcontroller = left_character.GetComponent<DaenerysController>();
                tcontroller = right_character.GetComponent<TheonController>();
            }
            jcontroller.curr_position = pos;
            jcontroller.curr_forward = vfront;
            dcontroller.curr_position = pos;
            dcontroller.curr_forward = vfront;
            tcontroller.curr_position = pos;
            tcontroller.curr_forward = vfront;
        }
        else if (current_character.GetName() == "Daenerys")
        {
            dcontroller = current_character.GetComponent<DaenerysController>();
            dcontroller.SetAnimationTransition("ToOut", true);
      
            if (right_character.GetName() == "Jaime")
            {
                jcontroller = right_character.GetComponent<JaimeController>();
                tcontroller = left_character.GetComponent<TheonController>();
            }
            else
            {
                jcontroller = left_character.GetComponent<JaimeController>();
                tcontroller = right_character.GetComponent<TheonController>();
            }
            jcontroller.curr_position = pos;
            jcontroller.curr_forward = vfront;
            dcontroller.curr_position = pos;
            dcontroller.curr_forward = vfront;
            tcontroller.curr_position = pos;
            tcontroller.curr_forward = vfront;
        }
        else if (current_character.GetName() == "Theon")
        {
            tcontroller = current_character.GetComponent<TheonController>();
            tcontroller.SetAnimationTransition("ToOut", true);

            if (right_character.GetName() == "Jaime")
            {
                jcontroller = right_character.GetComponent<JaimeController>();
                dcontroller = left_character.GetComponent<DaenerysController>();
            }
            else
            {
                jcontroller = left_character.GetComponent<JaimeController>();
                dcontroller = right_character.GetComponent<DaenerysController>();

            }
            jcontroller.curr_position = pos;
            jcontroller.curr_forward = vfront;
            dcontroller.curr_position = pos;
            dcontroller.curr_forward = vfront;
            tcontroller.curr_position = pos;
            tcontroller.curr_forward = vfront;
        }
    }

    bool IsCharacterAnimationStopped(GameObject characterGO, string name)
    {
        if (characterGO.GetName() == "Jaime")
        {
            return characterGO.GetComponent<JaimeController>().IsAnimationStopped(name);
        }
        else if (characterGO.GetName() == "Daenerys")
        {
            return characterGO.GetComponent<DaenerysController>().IsAnimationStopped(name);
        }
        else if (characterGO.GetName() == "Theon")
        {
            return characterGO.GetComponent<TheonController>().IsAnimationStopped(name);
        }
        else
        {
            return false;
        }
    }

    void ControlCurrentCharacter()
    {
        if (current_character.GetName() == "Jaime")
        {
            current_character.GetComponent<JaimeController>().ControlCharacter();
        }
        else if (current_character.GetName() == "Daenerys")
        {
            current_character.GetComponent<DaenerysController>().ControlCharacter();
        }
        else if (current_character.GetName() == "Theon")
        {
            current_character.GetComponent<TheonController>().ControlCharacter();
        }
    }

    void SecondaryAbility(Side side)
    {
        if (side == Side.LEFT)
        {
            if (left_character.GetName() == "Jaime")
            {
                GetLinkedObject("jaime_s_button_obj").GetComponent<CompButton>().Clicked();
            }
            else if (left_character.GetName() == "Daenerys")
            {
                GetLinkedObject("daenerys_s_button_obj").GetComponent<CompButton>().Clicked();

            }
            else if (left_character.GetName() == "Theon")
            {
                GetLinkedObject("theon_s_button_obj").GetComponent<CompButton>().Clicked();

            }
        }
        else if (side == Side.RIGHT)
        {
            if (right_character.GetName() == "Jaime")
            {
                GetLinkedObject("jaime_s_button_obj").GetComponent<CompButton>().Clicked();
            }
            else if (right_character.GetName() == "Daenerys")
            {
                GetLinkedObject("daenerys_s_button_obj").GetComponent<CompButton>().Clicked();
            }
            else if (right_character.GetName() == "Theon")
            {
                GetLinkedObject("theon_s_button_obj").GetComponent<CompButton>().Clicked();

            }
        }
    }

    //Call thius function to deal damage to the current character
    public void GetDamage(float dmg)
    {
        // Shield Ability Consumable
        if (player_obj.GetComponent<Shield>().IsActive())
        {
            player_obj.GetComponent<Shield>().Break();
        }

        else
        {
            // CURRENT CHARACTER -------------------------------
            if (current_character.GetName() == "Jaime")
            {
                current_character.GetComponent<JaimeController>().GetDamage(dmg);
                if(health_obj.GetComponent<Hp>().GetCurrentHealth() <= 0)
                {
                    current_character.GetComponent<JaimeController>().SetState(CharacterController.State.DEAD);
                    current_character.GetComponent<JaimeController>().PlayFx("JaimeDead");
                }
            }
            else if (current_character.GetName() == "Daenerys")
            {
                current_character.GetComponent<DaenerysController>().GetDamage(dmg);
            }
            else if (current_character.GetName() == "Theon")
            {
                current_character.GetComponent<TheonController>().GetDamage(dmg);
                if (health_obj.GetComponent<Hp>().GetCurrentHealth() <= 0)
                {
                    current_character.GetComponent<TheonController>().SetState(CharacterController.State.DEAD);
                    current_character.GetComponent<TheonController>().PlayFx("TheonDead");
                }
            }
        }
    }

    public bool IsIdle()
    {
        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            if(current_character.GetComponent<JaimeController>().GetState() == (int)CharacterController.State.IDLE)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (current_character.GetName() == "Daenerys")
        {
            if (current_character.GetComponent<DaenerysController>().GetState() == (int)CharacterController.State.IDLE)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (current_character.GetName() == "Theon")
        {
            if (current_character.GetComponent<TheonController>().GetState() == (int)CharacterController.State.IDLE)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    public bool IsDead(GameObject character)
    {
        if (character.GetName() == "Jaime")
        {
            if (character.GetComponent<JaimeController>().GetState() == (int)CharacterController.State.DEAD)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (character.GetName() == "Daenerys")
        {
            if (character.GetComponent<DaenerysController>().GetState() == (int)CharacterController.State.DEAD)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (character.GetName() == "Theon")
        {
            if (character.GetComponent<TheonController>().GetState() == (int)CharacterController.State.DEAD)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }


        //--------------------------------------------------------- THIS IS HARDCODED FOR SCORE SCREEN 
    void EnemyKilled(uint number = 1)
    {
        enem_killed += number;
    }

    float PuzzleTime()
    {
        return Time.timePlay;
    }

    public void StartPuzzle()
    {
        puzzle_start = 0;
        in_puzzle = true;
    }

    int GetStars()
    {
        int stars = 0;
        if (Time.timePlay <= 300)
        {
            stars++;
        }
        if (enem_killed == 3)
        {
            stars++;
        }
        if (PuzzleTime() < puz_max_time)
        {
            stars++;
        }
        return stars;
    }

    void CloseScoreMenu()
    {
        Time.timePlay = 0;
        enem_killed = 0;
        puzzle_start = 0;
        in_puzzle = false;
        bg.SetActive(false);
        coin1.SetActive(false);
        coin2.SetActive(false);
        coin3.SetActive(false);
    }

    void ShowVictoryMenu()
    {
        int a = GetStars();
        bg.SetActive(true);
        switch (a)
        {
            case 1:
                coin1.SetActive(true);
                break;
            case 2:
                coin1.SetActive(true);
                coin2.SetActive(true);
                break;
            case 3:
                coin1.SetActive(true);
                coin2.SetActive(true);
                coin3.SetActive(true);
                break;
        }
    }
    //--------------------------------------------------------- THIS IS HARDCODED FOR SCORE SCREEN 

    public void SetCurrentPosition()
    {
        Vector3 pos = transform.local_position;
        Vector3 vfront = transform.forward;
        Mathf.Round(vfront.x);
        Mathf.Round(vfront.y);
        Mathf.Round(vfront.z);

        JaimeController jcontroller;
        DaenerysController dcontroller;
        TheonController tcontroller;

        if (current_character.GetName() == "Jaime")
        {
            jcontroller = current_character.GetComponent<JaimeController>();

            if (right_character.GetName() == "Daenerys")
            {
                dcontroller = right_character.GetComponent<DaenerysController>();
                tcontroller = left_character.GetComponent<TheonController>();

            }
            else
            {
                dcontroller = left_character.GetComponent<DaenerysController>();
                tcontroller = right_character.GetComponent<TheonController>();
            }
            jcontroller.curr_position = pos;
            jcontroller.curr_forward = vfront;
            dcontroller.curr_position = pos;
            dcontroller.curr_forward = vfront;
            tcontroller.curr_position = pos;
            tcontroller.curr_forward = vfront;
        }
        else if (current_character.GetName() == "Daenerys")
        {
            dcontroller = current_character.GetComponent<DaenerysController>();

            if (right_character.GetName() == "Jaime")
            {
                jcontroller = right_character.GetComponent<JaimeController>();
                tcontroller = left_character.GetComponent<TheonController>();
            }
            else
            {
                jcontroller = left_character.GetComponent<JaimeController>();
                tcontroller = right_character.GetComponent<TheonController>();
            }
            jcontroller.curr_position = pos;
            jcontroller.curr_forward = vfront;
            dcontroller.curr_position = pos;
            dcontroller.curr_forward = vfront;
            tcontroller.curr_position = pos;
            tcontroller.curr_forward = vfront;
        }
        else if (current_character.GetName() == "Theon")
        {
            tcontroller = current_character.GetComponent<TheonController>();

            if (right_character.GetName() == "Jaime")
            {
                jcontroller = right_character.GetComponent<JaimeController>();
                dcontroller = left_character.GetComponent<DaenerysController>();
            }
            else
            {
                jcontroller = left_character.GetComponent<JaimeController>();
                dcontroller = right_character.GetComponent<DaenerysController>();

            }
            jcontroller.curr_position = pos;
            jcontroller.curr_forward = vfront;
            dcontroller.curr_position = pos;
            dcontroller.curr_forward = vfront;
            tcontroller.curr_position = pos;
            tcontroller.curr_forward = vfront;
        }
    }
}

