using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.SceneManagement;
using CulverinEditor.Pathfinding;
using System.Collections.Generic;

//CUTSCENE MANAGER -------------------------------------
public class Step
{
    public enum State
    {
        WAIT = 0,
        IN_PROGRESS,
        FINISHED
    }

    public State state = State.WAIT;
    public Step next_step = null;
    public string name = "Step";
    public float wait_time = 0.0f;

    public Step(string step_name, float wait)
    {
        name = step_name;
        wait_time = wait;
    }

    //Link to the next step
    public virtual void SetNextStep(Step step)
    {
        next_step = step;
    }

    public virtual void StartStep()
    {
        state = Step.State.IN_PROGRESS;

        Debug.Log("STEP START", Department.PLAYER, Color.BLUE);
    }

    public virtual void FinishStep()
    {
        if (WaitFinished())
        {
            state = Step.State.FINISHED;
            Debug.Log("STEP FINISH", Department.PLAYER, Color.RED);
        }
        else
        {
            Debug.Log("STEP WAITING FINISH", Department.PLAYER, Color.ORANGE);
        }
    }

    //Virtual method to override functions of the child steps
    public virtual void StepUpdate()
    {
    }

    public bool WaitFinished()
    {
        //Wait X sec until pass to the next step
        if (wait_time <= 0.0f)
        {
            return true;
        }
        else
        {
            wait_time -= Time.deltaTime;
            return false;
        }
    }

    public bool isFinished()
    {
        if (state == Step.State.FINISHED)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

//Wait X secs until the next step is reproduced
public class WaitStep : Step
{
    public WaitStep(string step_name, float wait) : base(step_name, wait)
    {
    }

    public override void StepUpdate()
    {
        wait_time -= Time.deltaTime;

        Debug.Log(wait_time, Department.PLAYER, Color.YELLOW);

        if (wait_time <= 0.0f)
        {
            FinishStep();
        }
    }
}

//Move the player to the desired direction
public class MoveStep : Step
{
    public enum MoveDirection
    {
        NONE = -1,
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
    }

    private MoveDirection direction = MoveDirection.NONE;
    private MovementController movement = null;

    //CONSTRUCTOR
    public MoveStep(MoveDirection dir, MovementController mov_player, string step_name, float wait = 0.0f) : base(step_name, wait)
    {
        direction = dir;
        movement = mov_player;
    }

    public override void StartStep()
    {
        base.StartStep();

        switch (direction)
        {
            case MoveDirection.FORWARD:
                {
                    //Tell the player to rotate left
                    movement.MoveForward();
                    Debug.Log("MOVE FORWARD", Department.PLAYER, Color.YELLOW);
                    break;
                }
            case MoveDirection.BACKWARD:
                {
                    //Tell the player to rotate left
                    movement.MoveBackward();
                    Debug.Log("MOVE BACKWARD", Department.PLAYER, Color.YELLOW);
                    break;
                }
            case MoveDirection.LEFT:
                {
                    //Tell the player to rotate left
                    movement.MoveLeft();
                    Debug.Log("MOVE LEFT", Department.PLAYER, Color.YELLOW);
                    break;
                }
            case MoveDirection.RIGHT:
                {
                    //Tell the player to rotate left
                    movement.MoveRight();
                    Debug.Log("MOVE RIGHT", Department.PLAYER, Color.YELLOW);
                    break;
                }
            default:
                {
                    break;
                }
        }
    }

    public override void StepUpdate()
    {
        //Check if player finished the movement
        if (movement.IsMoving() == false)
        {
            FinishStep();
        }
    }
}

//Rotate the player to the desired direction
public class RotateStep : Step
{
    public enum RotateDirection
    {
        NONE = -1,
        LEFT,
        RIGHT,
    }

    private RotateDirection direction = RotateDirection.NONE;
    private MovementController movement = null;

    //CONSTRUCTOR
    public RotateStep(RotateDirection dir, MovementController mov_player, string step_name, float wait = 0.0f) : base(step_name, wait)
    {
        direction = dir;
        movement = mov_player;

    }

    public override void StartStep()
    {
        base.StartStep();

        switch (direction)
        {
            case RotateDirection.LEFT:
                {
                    //Tell the player to rotate left
                    movement.RotateLeft();
                    Debug.Log("ROTATE LEFT", Department.PLAYER, Color.YELLOW);
                    break;
                }
            case RotateDirection.RIGHT:
                {
                    //Tell the player to rotate left
                    movement.RotateRight();
                    Debug.Log("ROTATE RIGHT", Department.PLAYER, Color.YELLOW);
                    break;
                }
            default:
                {
                    break;
                }
        }
    }

    public override void StepUpdate()
    {
        //Check if player finished the rotation
        if (movement.IsMoving() == false)
        {
            FinishStep();
        }
    }
}


public class Cutscene : CulverinBehaviour
{
    private Step curr_step;

    //Steps in this cutscene ----
    WaitStep wait_step;
    RotateStep rotate_left_step;
    RotateStep rotate_left_step2;
    MoveStep move_step1;
    RotateStep rotate_right_step;
    RotateStep rotate_right_step2;
    MoveStep move_step2;
    MoveStep move_step3;
    MoveStep move_step4;
    MoveStep move_step5;
    MoveStep move_step6;

    //Player position
    MovementController player_position;
    // --------------------------

    public bool start_cutscene = false;
    public bool cutscene_finished = false;

    //Boss presentation animation
    public void CutsceneInit(MovementController mov)
    {

        player_position = GetComponent<MovementController>();

        //Create all the steps
        wait_step = new WaitStep("wait_1", 2.0f);
        rotate_left_step = new RotateStep(RotateStep.RotateDirection.LEFT, mov, "rotate_left_1", 0.5f);
        move_step1 = new MoveStep(MoveStep.MoveDirection.FORWARD, mov, "move_forward_1", 0.5f);
        rotate_right_step = new RotateStep(RotateStep.RotateDirection.RIGHT, mov, "rotate_right_1", 1.5f);
        move_step2 = new MoveStep(MoveStep.MoveDirection.FORWARD, mov, "move_forward_2", 0.5f);
        move_step3 = new MoveStep(MoveStep.MoveDirection.FORWARD, mov, "move_forward_3", 0.5f);
        move_step4 = new MoveStep(MoveStep.MoveDirection.FORWARD, mov, "move_forward_4", 0.5f);
        move_step5 = new MoveStep(MoveStep.MoveDirection.FORWARD, mov, "move_forward_5", 0.5f);
        move_step6 = new MoveStep(MoveStep.MoveDirection.FORWARD, mov, "move_forward_6");
        //move_step2 = new MoveStep(MoveStep.MoveDirection.BACKWARD, mov, "move_backward_2");

        //If player triggers lever looking in other directions
        rotate_left_step2 = new RotateStep(RotateStep.RotateDirection.LEFT, mov, "rotate_left_2", 0.5f);
        rotate_right_step2 = new RotateStep(RotateStep.RotateDirection.RIGHT, mov, "rotate_right_1", 0.5f);

        //Link the steps
        wait_step.SetNextStep(rotate_left_step);
        rotate_left_step.SetNextStep(move_step1);
        move_step1.SetNextStep(rotate_right_step);
        rotate_right_step.SetNextStep(move_step2);
        move_step2.SetNextStep(move_step3);
        move_step3.SetNextStep(move_step4);
        move_step4.SetNextStep(move_step5);
        move_step5.SetNextStep(move_step6);

        //Start the first step
        curr_step = wait_step;
        curr_step.StartStep();
    }

    public void StartCutscene()
    {
        switch (player_position.curr_dir)
        {
            case MovementController.Direction.NORTH:
                {
                    wait_step.SetNextStep(move_step1);
                    break;
                }
            case MovementController.Direction.SOUTH:
                {
                    wait_step.SetNextStep(rotate_left_step2);
                    rotate_left_step2.SetNextStep(rotate_left_step);
                    break;
                }
            case MovementController.Direction.WEST:
                {
                    wait_step.SetNextStep(rotate_right_step2);
                    rotate_right_step2.SetNextStep(move_step1);
                    break;
                }
            default:
                {
                    break;
                }

        }


        start_cutscene = true;
        cutscene_finished = false;

        Debug.Log("Cutscene STARTED", Department.PLAYER);
    }

    public void CutsceneUpdate()
    {
        // CUTSCENE IN PROGRESS ----------------------
        if (curr_step != null)
        {
            if (curr_step.state == Step.State.IN_PROGRESS)
            {
                //Update the current step while it's in progress
                curr_step.StepUpdate();
            }

            //If the step is finished, init the next one
            if (curr_step.isFinished())
            {
                if (curr_step.next_step != null)
                {
                    //Set next step the current one
                    curr_step = curr_step.next_step;
                    curr_step.StartStep();

                    Debug.Log("NEXT STEP", Department.PLAYER, Color.PINK);
                }

                //Finish the cutscene by setting the current step to null
                else
                {
                    curr_step = null;
                }
            }
        }

        // CUTSCENE FINISHED ------------------------
        else
        {
            FinishCutscene();
        }
    }

    public void FinishCutscene()
    {
        start_cutscene = false;
        cutscene_finished = true;

        Debug.Log("Cutscene FINISHED", Department.PLAYER, Color.GREEN);
    }

    public bool isFinished()
    {
        return cutscene_finished;
    }
}
// -----------------------------------------------------

public class CharactersManager : CulverinBehaviour
{
    public enum State
    {
        IDLE = 0,
        CHANGING_LEFT,
        CHANGING_RIGHT,
        DROWNING,
        DYING,
        CUTSCENE
    }

    public enum Side
    {
        LEFT = 0,
        RIGHT
    }

    // THIS IS HARDCODED FOR SCORE SCREEN --
    uint enem_killed;
    public float puzzle_start;
    public bool in_puzzle;
    GameObject bg;
    GameObject coin1;
    GameObject coin2;
    GameObject coin3;
    // -------------------------------------

    public GameObject current_character;
    public GameObject left_character;
    public GameObject right_character;
    private GameObject temporal_change = null;

    public GameObject player_obj;
    public GameObject health_obj;
    Hp health;
    public GameObject stamina_obj;
    public GameObject leftamina_bar;
    public GameObject mana_obj;
    public GameObject leftmana_bar;

    //To manage secondary abilities buttons
    public GameObject jaime_s_button_obj;
    CompButton jaime_s_button;
    JaimeCD_Secondary jaime_s_script;
    public GameObject daenerys_s_button_obj;
    CompButton daenerys_s_button;
    DaenerysCD_Secondary daenerys_s_script;
    public GameObject theon_s_button_obj;
    CompButton theon_s_button;
    TheonCD_Secondary theon_s_script;

    public GameObject jaime_s_button_obj_idle;
    public GameObject daenerys_s_button_obj_idle;
    public GameObject theon_s_button_obj_idle;

    public float puz_max_time;
    public float drown_dmg = 20;
    public int puz_respawn_x = 0;
    public int puz_respawn_y = 0;

    //To manage player state
    State state = State.IDLE;
    public bool changing = false;
    public bool dying = false;

    //To manage Jaime Secondary Ability
    public bool shield_activated = false;

    public GameObject camera;
    CompAudio audio;

    private bool is_healing = false;
    private float heal_destination = 0.0f;

    //GOD MODE -----------------------------
    public GameObject god_mode_sprite;
    CompImage god_mode_image;
    public GameObject no_cds_text;
    public GameObject no_energy_text;
    public GameObject no_damage_text;
    /*GOD MODE: 
        - No damage taken.
        - 0 cost of abilities.
        - 1 sec cooldown for secondary abilities.
    */
    public bool god_mode = false;

    /*NO DAMAGE MODE: 
       - No damage taken.
   */
    public bool no_damage = false;

    /*NO CDS MODE: 
       - 1 sec for secondary abilities.  
    */
    public bool no_cds = false;

    /*NO ENERGY MODE: 
     - 0 cost for all abilities.  
    */
    public bool no_energy = false;

    //AUDIO SOUNDS CONTROLLER -------
    public bool jaime_tired = false;
    public float jaime_tired_time = 0.0f;
    public bool daenerys_tired = false;
    public float daenerys_tired_time = 0.0f;
    public bool theon_tired = false;
    public float theon_tired_time = 0.0f;
    // ------------------------------


    //CUTSCENE CONTROLLER -----
    public Cutscene cutscene;
    public GameObject hud_obj = null;
    private MovementController movement_controller = null;
    public GameObject black_bars = null;
    // ------------------------

    //Call this function to start the player cutscene to enter the boss room
    public void StartPlayerCutscene()
    {
        if (hud_obj != null)
        {
            //Disable In-Game HUD
            hud_obj.SetActive(false);

            // Black Bars
            black_bars.GetComponent<BlackBarsCutscene>().ActivateBlackBar(true);
        }

        //Start 
        cutscene.StartCutscene();

        //Change player state to block all inputs
        state = State.CUTSCENE;
    }

    void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        current_character = GetLinkedObject("current_character");
        left_character = GetLinkedObject("left_character");
        right_character = GetLinkedObject("right_character");

        player_obj = GetLinkedObject("player_obj");
        health_obj = GetLinkedObject("health_obj");
        health = health_obj.GetComponent<Hp>();
        stamina_obj = GetLinkedObject("stamina_obj");
        leftamina_bar = GetLinkedObject("leftamina_bar");
        mana_obj = GetLinkedObject("mana_obj");
        leftmana_bar = GetLinkedObject("leftmana_bar");

        jaime_s_button_obj = GetLinkedObject("jaime_s_button_obj");
        jaime_s_button_obj_idle = GetLinkedObject("jaime_s_button_obj_idle");
        jaime_s_button = jaime_s_button_obj.GetComponent<CompButton>();
        jaime_s_script = jaime_s_button_obj.GetComponent<JaimeCD_Secondary>();

        daenerys_s_button_obj = GetLinkedObject("daenerys_s_button_obj");
        daenerys_s_button = daenerys_s_button_obj.GetComponent<CompButton>();
        daenerys_s_button_obj_idle = GetLinkedObject("daenerys_s_button_obj_idle");
        daenerys_s_script = daenerys_s_button_obj.GetComponent<DaenerysCD_Secondary>();

        theon_s_button_obj = GetLinkedObject("theon_s_button_obj");
        theon_s_button = theon_s_button_obj.GetComponent<CompButton>();
        theon_s_button_obj_idle = GetLinkedObject("theon_s_button_obj_idle");
        theon_s_script = theon_s_button_obj.GetComponent<TheonCD_Secondary>();

        camera = GetLinkedObject("camera");

        hud_obj = GetLinkedObject("hud_obj");

        //GOD MODE VARIABLES ----------------------------------------
        god_mode_sprite = GetLinkedObject("god_mode_sprite");
        god_mode_image = god_mode_sprite.GetComponent<CompImage>();

        no_cds_text = GetLinkedObject("no_cds_text");
        no_energy_text = GetLinkedObject("no_energy_text");
        no_damage_text = GetLinkedObject("no_damage_text");

        god_mode_image.SetRender(false);

        no_cds_text.GetComponent<CompText>().SetText("NO COOLDOWNS MODE");
        no_cds_text.SetActive(false);

        no_energy_text.GetComponent<CompText>().SetText("NO COSTS MODE");
        no_energy_text.SetActive(false);

        no_damage_text.GetComponent<CompText>().SetText("NO DAMAGE MODE");
        no_damage_text.SetActive(false);

        SetCurrentPosition();

        audio = GetComponent<CompAudio>();
        leftmana_bar.GetComponent<CompImage>().SetEnabled(false);


        changing = false;
        dying = false;
        is_healing = false;
        god_mode = false;

        jaime_tired = false;
        jaime_tired_time = 0.0f;
        daenerys_tired = false;
        daenerys_tired_time = 0.0f;
        theon_tired = false;
        theon_tired_time = 0.0f;


        //CUTSCENE MANAGER ----------
        movement_controller = GetComponent<MovementController>();

        //Init cutscene only in the final level, at the boss floor
        if (hud_obj != null)
        {
            cutscene = new Cutscene();
            cutscene.CutsceneInit(movement_controller);

            // Black Bars
            black_bars = GetLinkedObject("black_bars");
        }
        // --------------------------
    }

    void Update()
    {
        // CUTSCENE MANAGER ------------------------------------------------------
        if (cutscene != null)
        {
            if (cutscene.start_cutscene == true && cutscene.cutscene_finished == false)
            {
                //Update cutscene while any of its states is in progress
                cutscene.CutsceneUpdate();

                //Check if cutscene is finished
                if (cutscene.isFinished())
                {
                    if (hud_obj != null)
                    {
                        //Enable In-Game HUD
                        hud_obj.SetActive(true);

                        // Black Bars
                        black_bars.GetComponent<BlackBarsCutscene>().ActivateBlackBar(false);
                    }

                    //Return the control to the player
                    state = State.IDLE;
                }
            }
        }
        // ----------------------------------------------------------------


        //MANAGE AUDIO CONTROLLER VARIABLES --
        if (jaime_tired)
        {
            jaime_tired_time += Time.deltaTime;
            if (jaime_tired_time >= 3.0f)
            {
                jaime_tired = false;
                jaime_tired_time = 0.0f;
            }
        }
        if (daenerys_tired)
        {
            daenerys_tired_time += Time.deltaTime;
            if (daenerys_tired_time >= 3.0f)
            {
                daenerys_tired = false;
                daenerys_tired_time = 0.0f;
            }
        }
        if (theon_tired)
        {
            theon_tired_time += Time.deltaTime;
            if (theon_tired_time >= 3.0f)
            {
                theon_tired = false;
                theon_tired_time = 0.0f;
            }
        }
        //------------------------------------

        //MANAGE GOD MODE
        CheckGodMode();

        //CONTROL LEFT HP BAR
        if (is_healing)
        {
            Debug.Log("Healing", Department.PLAYER, Color.PINK);
            if (current_character.GetName() == "Theon")
            {
                float curr_hp = health.GetCurrentHealth();
                float max_hp = current_character.GetComponent<TheonController>().max_hp;
                if (curr_hp < heal_destination)
                {
                    health.SetHP(curr_hp + 0.9f, max_hp);
                    current_character.GetComponent<TheonController>().curr_hp = curr_hp + 0.9f;
                }
                else
                {
                    current_character.GetComponent<TheonController>().play_breathing_audio = true;
                    health.SetHP(heal_destination, max_hp);
                    is_healing = false;
                }
            }
            else if (current_character.GetName() == "Jaime")
            {
                float curr_hp = health.GetCurrentHealth();
                float max_hp = current_character.GetComponent<JaimeController>().max_hp;
                if (curr_hp < heal_destination)
                {
                    health.SetHP(curr_hp + 0.9f, max_hp);
                    current_character.GetComponent<JaimeController>().curr_hp = curr_hp + 0.9f;
                }
                else
                {
                    current_character.GetComponent<JaimeController>().play_breathing_audio = true;
                    health.SetHP(heal_destination, max_hp);
                    is_healing = false;
                }
            }
            else if (current_character.GetName() == "Daenerys")
            {
                float curr_hp = health.GetCurrentHealth();
                float max_hp = current_character.GetComponent<DaenerysController>().max_hp;
                if (curr_hp < heal_destination)
                {
                    health.SetHP(curr_hp + 0.9f, max_hp);
                    current_character.GetComponent<DaenerysController>().curr_hp = curr_hp + 0.9f;
                }
                else
                {
                    current_character.GetComponent<DaenerysController>().play_breathing_audio = true;
                    health.SetHP(heal_destination, max_hp);
                    is_healing = false;
                }
            }
        }

        //MANAGE CHARACTERS MANAGER STATES (IDLE/CHANGING LEFT & RIGHT)
        switch (state)
        {
            case State.IDLE:
                {
                    //Check when finished In animation
                    if (changing && IsCharacterAnimationRunning(current_character, "Idle"))
                    {
                        changing = false;
                    }

                    //Test Jaime Secondary Ability
                    if (Input.GetKeyDown(KeyCode.Num0))
                    {
                        GetDamage(10);
                    }

                    if (health.GetCurrentHealth() <= 0)
                    {
                        Debug.Log("CHARACTER DIYNG", Department.PLAYER, Color.GREEN);
                        if (IsDead(left_character) == false)
                        {
                            Debug.Log("left", Department.PLAYER, Color.GREEN);
                            GetLinkedObject("player_obj").GetComponent<DamageFeedback>().CharacterDie();
                            state = State.CHANGING_LEFT;
                        }
                        else if (IsDead(right_character) == false)
                        {
                            Debug.Log("right", Department.PLAYER, Color.GREEN);
                            GetLinkedObject("player_obj").GetComponent<DamageFeedback>().CharacterDie();
                            state = State.CHANGING_RIGHT;
                        }
                        else
                        {
                            Debug.Log("U FUCKED", Department.PLAYER, Color.GREEN);
                            GetLinkedObject("player_obj").GetComponent<DamageFeedback>().TotalDefeat();
                            if (current_character.GetName() == "Theon")
                            {
                                TheonController theon = current_character.GetComponent<TheonController>();
                                theon.StopBreathing("TheonBreathing");
                            }
                            else if (current_character.GetName() == "Jaime")
                            {
                                JaimeController jaime = current_character.GetComponent<JaimeController>();
                                jaime.StopBreathing("JaimeBreathing");
                            }
                            else if (current_character.GetName() == "Daenerys")
                            {
                                DaenerysController daenerys = current_character.GetComponent<DaenerysController>();
                                daenerys.StopBreathing("DaenerysBreathing");
                            }
                        }
                    }

                    if (Input.GetKeyDown(KeyCode.M))
                    {
                        SceneManager.LoadScene("3D Menu intro");
                    }

                    if (Input.GetInput_KeyDown("TriangleR", "Player"))
                    {
                        if (IsIdle() == true)
                        {
                            if (IsDead(right_character) == false)
                            {
                                state = State.CHANGING_RIGHT;
                                CurrentToOut();
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
                            }
                        }
                    }

                    float vari = Input.GetInput_ControllerAxis("LAllyAttack", "Player");

                    if (vari > 0.8)
                    {
                        SecondaryAbility(Side.LEFT);
                    }

                    vari = Input.GetInput_ControllerAxis("RAllyAttack", "Player");

                    if (vari > 0.8)
                    {
                        SecondaryAbility(Side.RIGHT);
                    }


                    break;
                    // --------------------------------------

                }

            case State.CHANGING_LEFT:
                {
                    if (IsDead(current_character) == false)
                    {
                        if (IsCharacterAnimationStopped(current_character, "Out"))
                        {
                            ChangeLeft();

                            temporal_change = current_character;
                            current_character = left_character;
                            left_character = temporal_change;

                            state = State.IDLE;
                        }
                    }

                    else
                    {
                        if (IsCharacterAnimationStopped(current_character, "Death"))
                        {
                            ChangeLeft();

                            temporal_change = current_character;
                            current_character = left_character;
                            left_character = temporal_change;

                            state = State.IDLE;
                        }
                    }

                    break;
                }

            case State.CHANGING_RIGHT:
                {
                    if (IsDead(current_character) == false)
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
                    }

                    else
                    {
                        if (IsCharacterAnimationStopped(current_character, "Death"))
                        {
                            ChangeRight();

                            //Change GameObjects --------------------
                            temporal_change = current_character;
                            current_character = right_character;
                            right_character = temporal_change;

                            state = State.IDLE;
                        }
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
            current_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.BEHIND_LEFT);
            current_character.GetComponent<JaimeController>().UpdateHUD(false, true);
            current_character.GetComponent<JaimeController>().ToggleMesh(false);
            current_character.GetComponent<JaimeController>().jaime_sword_obj.GetComponent<CompCollider>().CollisionActive(false);
        }
        else if (current_character.GetName() == "Daenerys")
        {
            current_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.BEHIND_LEFT);
            current_character.GetComponent<DaenerysController>().UpdateHUD(false, true);
            current_character.GetComponent<DaenerysController>().ToggleMesh(false);
        }
        else if (current_character.GetName() == "Theon")
        {
            current_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.BEHIND_LEFT);
            current_character.GetComponent<TheonController>().UpdateHUD(false, true);
            current_character.GetComponent<TheonController>().ToggleMesh(false);
        }

        // LEFT CHARACTER -------------------------------
        if (left_character.GetName() == "Jaime")
        {
            left_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<JaimeController>().UpdateHUD(true, true);
            left_character.GetComponent<CompAnimation>().PlayAnimationNode("In");
            left_character.GetComponent<JaimeController>().ToggleMesh(true);
            left_character.GetComponent<JaimeController>().jaime_sword_obj.GetComponent<CompCollider>().CollisionActive(true);
            left_character.GetComponent<JaimeController>().force_audio = true;
            GetLinkedObject("this_obj_lasthp").GetComponent<CompImage>().FillAmount(left_character.GetComponent<JaimeController>().curr_hp / 100.0f);
        }
        else if (left_character.GetName() == "Daenerys")
        {
            left_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<DaenerysController>().UpdateHUD(true, true);
            left_character.GetComponent<CompAnimation>().PlayAnimationNode("In");
            left_character.GetComponent<DaenerysController>().ToggleMesh(true);
            left_character.GetComponent<DaenerysController>().force_audio = true;
            GetLinkedObject("this_obj_lasthp").GetComponent<CompImage>().FillAmount(left_character.GetComponent<DaenerysController>().curr_hp / 100.0f);
        }
        else if (left_character.GetName() == "Theon")
        {
            left_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.CURRENT);
            left_character.GetComponent<TheonController>().UpdateHUD(true, true);
            left_character.GetComponent<TheonController>().ToggleMesh(true);
            left_character.GetComponent<CompAnimation>().PlayAnimationNode("In");
            left_character.GetComponent<TheonController>().force_audio = true;
            GetLinkedObject("this_obj_lasthp").GetComponent<CompImage>().FillAmount(left_character.GetComponent<TheonController>().curr_hp / 100.0f);
        }
    }

    void ChangeRight()
    {
        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            current_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.BEHIND_RIGHT);
            current_character.GetComponent<JaimeController>().UpdateHUD(false, false);
            current_character.GetComponent<JaimeController>().ToggleMesh(false);
            current_character.GetComponent<JaimeController>().jaime_sword_obj.GetComponent<CompCollider>().CollisionActive(false);
        }
        else if (current_character.GetName() == "Daenerys")
        {
            current_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.BEHIND_RIGHT);
            current_character.GetComponent<DaenerysController>().UpdateHUD(false, false);
            current_character.GetComponent<DaenerysController>().ToggleMesh(false);
        }
        else if (current_character.GetName() == "Theon")
        {
            current_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.BEHIND_RIGHT);
            current_character.GetComponent<TheonController>().UpdateHUD(false, false);
            current_character.GetComponent<TheonController>().ToggleMesh(false);
        }

        // RIGHT CHARACTER -------------------------------
        if (right_character.GetName() == "Jaime")
        {
            right_character.GetComponent<JaimeController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<JaimeController>().UpdateHUD(true, false);
            right_character.GetComponent<CompAnimation>().PlayAnimationNode("In");
            right_character.GetComponent<JaimeController>().ToggleMesh(true);
            right_character.GetComponent<JaimeController>().jaime_sword_obj.GetComponent<CompCollider>().CollisionActive(true);
            right_character.GetComponent<JaimeController>().force_audio = true;
            GetLinkedObject("this_obj_lasthp").GetComponent<CompImage>().FillAmount(right_character.GetComponent<JaimeController>().curr_hp / 100.0f);
        }
        else if (right_character.GetName() == "Daenerys")
        {
            right_character.GetComponent<DaenerysController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<DaenerysController>().UpdateHUD(true, false);
            right_character.GetComponent<CompAnimation>().PlayAnimationNode("In");
            right_character.GetComponent<DaenerysController>().ToggleMesh(true);
            right_character.GetComponent<DaenerysController>().force_audio = true;
            GetLinkedObject("this_obj_lasthp").GetComponent<CompImage>().FillAmount(right_character.GetComponent<DaenerysController>().curr_hp / 100.0f);
        }
        else if (right_character.GetName() == "Theon")
        {
            right_character.GetComponent<TheonController>().SetPosition(CharacterController.Position.CURRENT);
            right_character.GetComponent<TheonController>().UpdateHUD(true, false);
            right_character.GetComponent<CompAnimation>().PlayAnimationNode("In");
            right_character.GetComponent<TheonController>().ToggleMesh(true);
            right_character.GetComponent<TheonController>().force_audio = true;
            GetLinkedObject("this_obj_lasthp").GetComponent<CompImage>().FillAmount(right_character.GetComponent<TheonController>().curr_hp / 100.0f);
        }
    }

    void CurrentToOut()
    {
        changing = true;

        Vector3 pos = transform.local_position;
        Vector3 vfront = transform.forward;
        Mathf.Round(vfront.x);
        Mathf.Round(vfront.y);
        Mathf.Round(vfront.z);

        JaimeController jcontroller;
        DaenerysController dcontroller;
        TheonController tcontroller;

        current_character.GetComponent<CompAnimation>().PlayAnimationNode("Out");

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

    bool IsCharacterAnimationRunning(GameObject characterGO, string name)
    {
        if (characterGO.GetName() == "Jaime")
        {
            return characterGO.GetComponent<JaimeController>().IsAnimationRunning(name);
        }
        else if (characterGO.GetName() == "Daenerys")
        {
            return characterGO.GetComponent<DaenerysController>().IsAnimationRunning(name);
        }
        else if (characterGO.GetName() == "Theon")
        {
            return characterGO.GetComponent<TheonController>().IsAnimationRunning(name);
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
            if (left_character.GetName() == "Jaime" && state != State.CUTSCENE)
            {
                jaime_s_button.Clicked();
            }
            else if (left_character.GetName() == "Daenerys" && state != State.CUTSCENE)
            {
                daenerys_s_button.Clicked();
            }
            else if (left_character.GetName() == "Theon" && state != State.CUTSCENE)
            {
                theon_s_button.Clicked();
            }
        }
        else if (side == Side.RIGHT)
        {
            if (right_character.GetName() == "Jaime" && state != State.CUTSCENE)
            {
                jaime_s_button.Clicked();
            }
            else if (right_character.GetName() == "Daenerys" && state != State.CUTSCENE)
            {
                daenerys_s_button.Clicked();
            }
            else if (right_character.GetName() == "Theon" && state != State.CUTSCENE)
            {
                theon_s_button.Clicked();
            }
        }
    }

    //Call thius function to deal damage to the current character
    public bool GetDamage(float dmg)
    {
        //Rumble Gamepad
        if (dmg != 0.0f)
            Input.RumblePlay(0.5f, 200);

        // Shield Ability Consumable
        if (player_obj.GetComponent<Shield>().IsActive())
        {
            player_obj.GetComponent<Shield>().Break();
            return false;
        }
        else
        {
            // 0 DAMAGE TAKEN IN GOD MODE
            if (god_mode || no_damage)
            {
                dmg = 0;
            }

            // CURRENT CHARACTER -------------------------------
            if (GetCurrCharacterState() != (int)CharacterController.State.DEAD)
            {
                if (current_character.GetName() == "Jaime")
                {
                    if (current_character.GetComponent<JaimeController>().GetDamage(dmg))
                    {
                        if (health.GetCurrentHealth() <= 0)
                        {
                            StatsScore.CharacterDead();

                            JaimeController jaime_controller = current_character.GetComponent<JaimeController>();
                            jaime_controller.SetState(CharacterController.State.DEAD);
                            jaime_controller.jaime_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                            jaime_controller.jaime_icon_obj_stamina.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                            jaime_controller.jaime_icon_obj_stamina.GetComponent<CompImage>().SetRender(false);

                            //Deactivate Secondary ability button
                            jaime_s_button.Deactivate();
                            jaime_s_script.Die();

                            if (dmg != 0.0f)
                                jaime_controller.PlayFx("JaimeDead");
                        }
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (current_character.GetName() == "Daenerys")
                {
                    current_character.GetComponent<DaenerysController>().GetDamage(dmg);
                    if (health.GetCurrentHealth() <= 0)
                    {
                        StatsScore.CharacterDead();

                        DaenerysController daenerys_controller = current_character.GetComponent<DaenerysController>();
                        daenerys_controller.SetState(CharacterController.State.DEAD);
                        daenerys_controller.daenerys_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        daenerys_controller.daenerys_icon_obj_mana.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        daenerys_controller.daenerys_icon_obj_mana.GetComponent<CompImage>().SetRender(false);

                        //Deactivate Secondary ability button
                        daenerys_s_button.Deactivate();
                        daenerys_s_script.Die();
                        if (dmg != 0.0f)
                            daenerys_controller.PlayFx("DaenerysDead");
                    }
                    return true;
                }
                else if (current_character.GetName() == "Theon")
                {
                    current_character.GetComponent<TheonController>().GetDamage(dmg);
                    if (health.GetCurrentHealth() <= 0)
                    {
                        StatsScore.CharacterDead();

                        TheonController theon_controller = current_character.GetComponent<TheonController>();
                        theon_controller.SetState(CharacterController.State.DEAD);
                        theon_controller.theon_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        theon_controller.theon_icon_obj_stamina.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        theon_controller.theon_icon_obj_stamina.GetComponent<CompImage>().SetRender(false);

                        //Deactivate Secondary ability button
                        theon_s_button.Deactivate();
                        theon_s_script.Die();
                        if (dmg != 0.0f)
                            theon_controller.PlayFx("TheonDead");
                    }
                    return true;
                }
            }
            return true;
        }
    }

    public bool GetDamageProjectile(float dmg)
    {
        //Rumble Gamepad
        if (dmg != 0.0f)
            Input.RumblePlay(0.5f, 200);

        // Shield Ability Consumable
        if (player_obj.GetComponent<Shield>().IsActive())
        {
            player_obj.GetComponent<Shield>().Break();
            return false;
        }
        else
        {
            // 0 DAMAGE TAKEN IN GOD MODE
            if (god_mode || no_damage)
            {
                dmg = 0;
            }

            // CURRENT CHARACTER -------------------------------
            if (GetCurrCharacterState() != (int)CharacterController.State.DEAD)
            {
                if (current_character.GetName() == "Jaime")
                {
                    if (current_character.GetComponent<JaimeController>().GetDamageProj(dmg))
                    {
                        if (health.GetCurrentHealth() <= 0)
                        {
                            StatsScore.CharacterDead();

                            JaimeController jaime_controller = current_character.GetComponent<JaimeController>();
                            jaime_controller.SetState(CharacterController.State.DEAD);
                            jaime_controller.jaime_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                            jaime_controller.jaime_icon_obj_stamina.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                            jaime_controller.jaime_icon_obj_stamina.GetComponent<CompImage>().SetRender(false);

                            //Deactivate Secondary ability button
                            jaime_s_button.Deactivate();
                            jaime_s_script.Die();

                            if (dmg != 0.0f)
                                jaime_controller.PlayFx("JaimeDead");
                        }
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (current_character.GetName() == "Daenerys")
                {
                    current_character.GetComponent<DaenerysController>().GetDamage(dmg);
                    if (health.GetCurrentHealth() <= 0)
                    {
                        StatsScore.CharacterDead();

                        DaenerysController daenerys_controller = current_character.GetComponent<DaenerysController>();
                        daenerys_controller.SetState(CharacterController.State.DEAD);
                        daenerys_controller.daenerys_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        daenerys_controller.daenerys_icon_obj_mana.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        daenerys_controller.daenerys_icon_obj_mana.GetComponent<CompImage>().SetRender(false);

                        //Deactivate Secondary ability button
                        daenerys_s_button.Deactivate();
                        daenerys_s_script.Die();
                        if (dmg != 0.0f)
                            daenerys_controller.PlayFx("DaenerysDead");
                    }
                    return true;
                }
                else if (current_character.GetName() == "Theon")
                {
                    current_character.GetComponent<TheonController>().GetDamage(dmg);
                    if (health.GetCurrentHealth() <= 0)
                    {
                        StatsScore.CharacterDead();

                        TheonController theon_controller = current_character.GetComponent<TheonController>();
                        theon_controller.SetState(CharacterController.State.DEAD);
                        theon_controller.theon_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        theon_controller.theon_icon_obj_stamina.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        theon_controller.theon_icon_obj_stamina.GetComponent<CompImage>().SetRender(false);

                        //Deactivate Secondary ability button
                        theon_s_button.Deactivate();
                        theon_s_script.Die();
                        if (dmg != 0.0f)
                            theon_controller.PlayFx("TheonDead");
                    }
                    return true;
                }
            }
            return true;
        }
    }

    //Call thius function to deal damage to the current character
    public bool Push(float dmg, PathNode tile)
    {
        //Rumble Gamepad
        Input.RumblePlay(0.5f, 200);

        // Shield Ability Consumable
        if (player_obj.GetComponent<Shield>().IsActive())
        {
            player_obj.GetComponent<Shield>().Break();
            return false;
        }
        else
        {
            // 0 DAMAGE TAKEN IN GOD MODE
            if (god_mode || no_damage)
            {
                dmg = 0;
            }

            // CURRENT CHARACTER -------------------------------
            if (GetCurrCharacterState() != (int)CharacterController.State.DEAD)
            {
                if (current_character.GetName() == "Jaime")
                {
                    if (current_character.GetComponent<JaimeController>().Push(dmg, tile))
                    {
                        if (health.GetCurrentHealth() <= 0)
                        {
                            StatsScore.CharacterDead();

                            JaimeController jaime_controller = current_character.GetComponent<JaimeController>();
                            jaime_controller.SetState(CharacterController.State.DEAD);
                            jaime_controller.jaime_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                            jaime_controller.jaime_icon_obj_stamina.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                            jaime_controller.jaime_icon_obj_stamina.GetComponent<CompImage>().SetRender(false);

                            //Deactivate Secondary ability button
                            jaime_s_button.Deactivate();
                            jaime_s_script.Die();

                            jaime_controller.PlayFx("JaimeDead");
                        }
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (current_character.GetName() == "Daenerys")
                {
                    current_character.GetComponent<DaenerysController>().Push(dmg, tile);
                    if (health.GetCurrentHealth() <= 0)
                    {
                        StatsScore.CharacterDead();

                        DaenerysController daenerys_controller = current_character.GetComponent<DaenerysController>();
                        daenerys_controller.SetState(CharacterController.State.DEAD);
                        daenerys_controller.daenerys_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        daenerys_controller.daenerys_icon_obj_mana.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        daenerys_controller.daenerys_icon_obj_mana.GetComponent<CompImage>().SetRender(false);

                        //Deactivate Secondary ability button
                        daenerys_s_button.Deactivate();
                        daenerys_s_script.Die();

                        daenerys_controller.PlayFx("DaenerysDead");
                    }
                    return true;
                }
                else if (current_character.GetName() == "Theon")
                {
                    current_character.GetComponent<TheonController>().Push(dmg, tile);
                    if (health.GetCurrentHealth() <= 0)
                    {
                        StatsScore.CharacterDead();

                        TheonController theon_controller = current_character.GetComponent<TheonController>();
                        theon_controller.SetState(CharacterController.State.DEAD);
                        theon_controller.theon_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        theon_controller.theon_icon_obj_stamina.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
                        theon_controller.theon_icon_obj_stamina.GetComponent<CompImage>().SetRender(false);

                        //Deactivate Secondary ability button
                        theon_s_button.Deactivate();
                        theon_s_script.Die();

                        theon_controller.PlayFx("TheonDead");
                    }
                    return true;
                }
            }
            return true;
        }
    }

    public int GetCurrCharacterState()
    {
        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            return current_character.GetComponent<JaimeController>().GetState();
        }
        else if (current_character.GetName() == "Daenerys")
        {
            return current_character.GetComponent<DaenerysController>().GetState();
        }
        else if (current_character.GetName() == "Theon")
        {
            return current_character.GetComponent<TheonController>().GetState();
        }
        else
        {
            return -1;
        }
    }

    public void Drown()
    {
        GetDamage(drown_dmg);
        //state = State.DROWNING;
    }

    public bool IsIdle()
    {
        //False if a cutscene is being reproduced
        if (state == State.CUTSCENE)
        {
            return false;
        }

        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            if (current_character.GetComponent<JaimeController>().GetState() == (int)CharacterController.State.IDLE)
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

    public void PlayerDie(GameObject character)
    {
        if (character.GetName() == "Jaime")
        {
            character.GetComponent<JaimeController>().SetState(CharacterController.State.DEAD);
        }
        else if (character.GetName() == "Daenerys")
        {
            character.GetComponent<DaenerysController>().SetState(CharacterController.State.DEAD);
        }
        else if (character.GetName() == "Theon")
        {
            character.GetComponent<TheonController>().SetState(CharacterController.State.DEAD);
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

    public State GetManagerState()
    {
        return state;
    }

    public void SetManagerState(State value)
    {
        state = value;
    }


    // THIS IS HARDCODED FOR SCORE SCREEN --------------------------------
    public float GetTotalHP()
    {
        float result_hp = 0.0f;

        //CURRENT CHARACTER HP
        if (current_character.GetName() == "Jaime")
        {
            result_hp += current_character.GetComponent<JaimeController>().curr_hp;
        }
        else if (current_character.GetName() == "Daenerys")
        {
            result_hp += current_character.GetComponent<DaenerysController>().curr_hp;
        }
        else if (current_character.GetName() == "Theon")
        {
            result_hp += current_character.GetComponent<TheonController>().curr_hp;
        }

        //LEFT CHARACTER HP
        if (left_character.GetName() == "Jaime")
        {
            result_hp += left_character.GetComponent<JaimeController>().curr_hp;
        }
        else if (left_character.GetName() == "Daenerys")
        {
            result_hp += left_character.GetComponent<DaenerysController>().curr_hp;
        }
        else if (left_character.GetName() == "Theon")
        {
            result_hp += left_character.GetComponent<TheonController>().curr_hp;
        }

        //RIGHT CHARACTER HP
        if (right_character.GetName() == "Jaime")
        {
            result_hp += right_character.GetComponent<JaimeController>().curr_hp;
        }
        else if (right_character.GetName() == "Daenerys")
        {
            result_hp += right_character.GetComponent<DaenerysController>().curr_hp;
        }
        else if (right_character.GetName() == "Theon")
        {
            result_hp += right_character.GetComponent<TheonController>().curr_hp;
        }

        Debug.Log("TOTAL HP: " + result_hp, Department.PLAYER, Color.RED);

        return result_hp;
    }

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
    // THIS IS HARDCODED FOR SCORE SCREEN --------------------------------

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

    public int GetState()
    {
        return (int)state;
    }

    public void ApplyFatigue(float fatigue)
    {
        // CURRENT CHARACTER -------------------------------
        if (current_character.GetName() == "Jaime")
        {
            current_character.GetComponent<JaimeController>().DecreaseStamina(fatigue);
        }
        else if (current_character.GetName() == "Daenerys")
        {
            current_character.GetComponent<DaenerysController>().DecreaseMana(fatigue);
        }
        else if (current_character.GetName() == "Theon")
        {
            current_character.GetComponent<TheonController>().DecreaseStamina(fatigue);
        }
    }

    public void SetCurrentPlayerState(CharacterController.State state)
    {
        current_character.GetComponent<CharacterController>().SetState(state);
    }

    public void HealCharacters(float percentage)
    {
        is_healing = true;

        if (current_character.GetName() == "Jaime")
        {
            float max_hp = current_character.GetComponent<JaimeController>().max_hp;
            heal_destination = max_hp * percentage + health.GetCurrentHealth();

            if (heal_destination > max_hp)
            {
                heal_destination = max_hp;
            }
            if (left_character.GetName() == "Daenerys")
            {
                left_character.GetComponent<DaenerysController>().Heal(percentage);
                right_character.GetComponent<TheonController>().Heal(percentage);
            }
            else if (left_character.GetName() == "Theon")
            {
                right_character.GetComponent<DaenerysController>().Heal(percentage);
                left_character.GetComponent<TheonController>().Heal(percentage);
            }
        }

        else if (current_character.GetName() == "Daenerys")
        {
            float max_hp = current_character.GetComponent<DaenerysController>().max_hp;
            heal_destination = max_hp * percentage + health.GetCurrentHealth();
            if (heal_destination > max_hp)
            {
                heal_destination = max_hp;
            }
            if (left_character.GetName() == "Jaime")
            {
                left_character.GetComponent<JaimeController>().Heal(percentage);
                right_character.GetComponent<TheonController>().Heal(percentage);
            }
            else if (left_character.GetName() == "Theon")
            {
                right_character.GetComponent<JaimeController>().Heal(percentage);
                left_character.GetComponent<TheonController>().Heal(percentage);
            }
        }

        else if (current_character.GetName() == "Theon")
        {
            float max_hp = current_character.GetComponent<TheonController>().max_hp;
            heal_destination = max_hp * percentage + health.GetCurrentHealth();
            if (heal_destination > max_hp)
            {
                heal_destination = max_hp;
            }

            if (left_character.GetName() == "Daenerys")
            {
                left_character.GetComponent<DaenerysController>().Heal(percentage);
                right_character.GetComponent<JaimeController>().Heal(percentage);
            }
            else if (left_character.GetName() == "Jaime")
            {
                right_character.GetComponent<DaenerysController>().Heal(percentage);
                left_character.GetComponent<JaimeController>().Heal(percentage);
            }
        }
    }

    public void CheckGodMode()
    {
        //GOD MODE
        if (Input.GetKeyDown(KeyCode.F1))
        {
            god_mode = !god_mode;

            //Enable god mode sprite
            god_mode_image.SetRender(god_mode);
        }

        //NO DAMAGE MODE
        if (Input.GetKeyDown(KeyCode.F11))
        {
            no_damage = !no_damage;
            no_damage_text.SetActive(no_damage);
        }


        //NO CDS MODE
        if (Input.GetKeyDown(KeyCode.F10))
        {
            no_cds = !no_cds;
            no_cds_text.SetActive(no_cds);
        }


        //NO ENERGY MODE
        if (Input.GetKeyDown(KeyCode.F9))
        {
            no_energy = !no_energy;
            no_energy_text.SetActive(no_energy);
        }
    }

    public void SetCurrentCharacterState(CharacterController.State currstate)
    {
        if (current_character.GetName() == "Jaime")
        {
            current_character.GetComponent<JaimeController>().SetState(currstate);
        }
        else if (current_character.GetName() == "Daenerys")
        {
            current_character.GetComponent<DaenerysController>().SetState(currstate);
        }
        else if (current_character.GetName() == "Theon")
        {
            current_character.GetComponent<TheonController>().SetState(currstate);
        }
    }

    public void SaveInfoPlayer()
    {
        if (current_character.GetName() == "Jaime")
        {
            SceneManager.PushSaveInfo(current_character.GetComponent<JaimeController>().curr_hp);
        }
        if (right_character.GetName() == "Jaime")
        {
            SceneManager.PushSaveInfo(right_character.GetComponent<JaimeController>().curr_hp);
        }
        if (left_character.GetName() == "Jaime")
        {
            SceneManager.PushSaveInfo(left_character.GetComponent<JaimeController>().curr_hp);
        }

        if (current_character.GetName() == "Daenerys")
        {
            SceneManager.PushSaveInfo(current_character.GetComponent<DaenerysController>().curr_hp);
        }
        if (left_character.GetName() == "Daenerys")
        {
            SceneManager.PushSaveInfo(left_character.GetComponent<DaenerysController>().curr_hp);
        }
        if (right_character.GetName() == "Daenerys")
        {
            SceneManager.PushSaveInfo(right_character.GetComponent<DaenerysController>().curr_hp);
        }

        if (current_character.GetName() == "Theon")
        {
            SceneManager.PushSaveInfo(current_character.GetComponent<TheonController>().curr_hp);
        }
        if (left_character.GetName() == "Theon")
        {
            SceneManager.PushSaveInfo(left_character.GetComponent<TheonController>().curr_hp);
        }
        if (right_character.GetName() == "Theon")
        {
            SceneManager.PushSaveInfo(right_character.GetComponent<TheonController>().curr_hp);
        }
    }

    public void LoadInfoPlayer()
    {
        float theoninfo = SceneManager.PopLoadInfo();
        float daenerysinfo = SceneManager.PopLoadInfo();
        float jaimeinfo = SceneManager.PopLoadInfo();

        if (theoninfo == 1503.0f)
        {
            left_character.GetComponent<TheonController>().curr_hp = left_character.GetComponent<TheonController>().max_hp;
        }
        if (jaimeinfo == 1503.0f)
        {
            current_character.GetComponent<JaimeController>().curr_hp = current_character.GetComponent<JaimeController>().max_hp;
        }
        if (daenerysinfo == 1503.0f)
        {
            right_character.GetComponent<DaenerysController>().curr_hp = right_character.GetComponent<DaenerysController>().max_hp;
        }

        if (theoninfo == 0.0f)
        {
            LeftCharacterDie();
            left_character.GetComponent<TheonController>().theon_icon_obj_hp.GetComponent<CompImage>().SetRender(false);
        }
        if (daenerysinfo == 0.0f)
        {
            RightCharacterDie();
            left_character.GetComponent<DaenerysController>().daenerys_icon_obj_hp.GetComponent<CompImage>().SetRender(false);
        }

        Debug.Log("INFO PLAYER", Department.PLAYER, Color.RED);
        GetLinkedObject("health_obj").GetComponent<Hp>().SetHP(current_character.GetComponent<JaimeController>().curr_hp, current_character.GetComponent<JaimeController>().max_hp);
        GetLinkedObject("this_obj_lasthp").GetComponent<CompImage>().FillAmount(current_character.GetComponent<JaimeController>().curr_hp / 100.0f);
        GetLinkedObject("daenerys_icon_obj_hp").GetComponent<CompImage>().FillAmount(right_character.GetComponent<DaenerysController>().curr_hp / 100.0f);
        GetLinkedObject("theon_icon_obj_hp").GetComponent<CompImage>().FillAmount(left_character.GetComponent<TheonController>().curr_hp / 100.0f);


        if (jaimeinfo <= 0.0f)
        {
            GetDamage(0.0f);
            current_character.GetComponent<JaimeController>().SetState(CharacterController.State.DEAD);
            current_character.GetComponent<JaimeController>().jaime_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
            current_character.GetComponent<JaimeController>().jaime_icon_obj_stamina.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
            current_character.GetComponent<JaimeController>().jaime_icon_obj_stamina.GetComponent<CompImage>().SetRender(false);
            current_character.GetComponent<JaimeController>().jaime_icon_obj_hp.GetComponent<CompImage>().SetRender(false);

            jaime_s_button.Deactivate();
            jaime_s_script.Die();

            if (theoninfo > 0.0f)
            {
                ChangeLeft();

                temporal_change = current_character;
                current_character = left_character;
                left_character = temporal_change;

                state = State.IDLE;
            }
            else
            {
                ChangeRight();

                temporal_change = current_character;
                current_character = right_character;
                right_character = temporal_change;

                state = State.IDLE;
            }
        }
    }

    public string GetCurrentCharacterName()
    {
        return current_character.GetName();
    }

    public void RightCharacterDie()
    {
        StatsScore.CharacterDead();

        DaenerysController daenerys_controller = right_character.GetComponent<DaenerysController>();
        daenerys_controller.SetState(CharacterController.State.DEAD);
        daenerys_controller.daenerys_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
        daenerys_controller.daenerys_icon_obj_mana.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
        daenerys_controller.daenerys_icon_obj_mana.GetComponent<CompImage>().SetRender(false);

        //Deactivate Secondary ability button
        daenerys_s_button.Deactivate();
        daenerys_s_script.Die();
    }
    public void LeftCharacterDie()
    {
        StatsScore.CharacterDead();

        TheonController theon_controller = left_character.GetComponent<TheonController>();
        theon_controller.SetState(CharacterController.State.DEAD);
        theon_controller.theon_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
        theon_controller.theon_icon_obj_stamina.GetComponent<CompImage>().SetColor(new Vector3(0.3f, 0.3f, 0.3f), 1.0f);
        theon_controller.theon_icon_obj_stamina.GetComponent<CompImage>().SetRender(false);

        //Deactivate Secondary ability button
        theon_s_button.Deactivate();
        theon_s_script.Die();
    }
}

