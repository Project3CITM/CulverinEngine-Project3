using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysController : CharacterController
{
    //MESH ELEMENTS
    public GameObject daenerys_obj;
    public GameObject rarm_daenerys_obj;
    public GameObject larm_daenerys_obj;

    //UI ELEMENTS
    public GameObject daenerys_icon_obj;
    public GameObject daenerys_icon_obj_hp;
    public GameObject daenerys_icon_obj_mana;
    public GameObject daenerys_button_left_obj;
    public GameObject daenerys_button_right_obj;
    public GameObject daenerys_left_flag;
    public GameObject daenerys_right_flag;

    public GameObject particle_firebreath_obj;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_mana = 100.0f;
    public float curr_mana = 100.0f;

    public float sec_ability_cost = 30.0f;
    DaenerysCD_Secondary sec_ability_cd;

    private CompParticleSystem particle_system;

    //Left Ability Stats
    public float mana_cost_percentage_left = 20f;
    public float damage_percentage_left = 10f;
    public int distance_left_attack = 3;
    private DaenerysCD_Left cd_left;
    private bool set_fire_breath = false;

    //Right Ability Stats
    public float mana_cost_percentage_right = 20f;
    public float damage_percentage_right = 10f;
    private DaenerysCD_Right cd_right;
    private bool set_fire_wall = false;

    protected override void Start()
    {
        SetPosition(Position.BEHIND);

        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        daenerys_obj = GetLinkedObject("daenerys_obj");
        rarm_daenerys_obj = GetLinkedObject("rarm_daenerys_obj");
        larm_daenerys_obj = GetLinkedObject("larm_daenerys_obj");

        daenerys_icon_obj = GetLinkedObject("daenerys_icon_obj");
        daenerys_button_left_obj = GetLinkedObject("daenerys_button_left_obj");
        daenerys_button_right_obj = GetLinkedObject("daenerys_button_right_obj");
        daenerys_left_flag = GetLinkedObject("daenerys_left_flag");
        daenerys_right_flag = GetLinkedObject("daenerys_right_flag");

        daenerys_icon_obj_hp = GetLinkedObject("daenerys_icon_obj_hp");
        daenerys_icon_obj_mana = GetLinkedObject("daenerys_icon_obj_mana");

        particle_firebreath_obj = GetLinkedObject("particle_firebreath_obj");

        // Start Idle animation
        anim_controller = daenerys_obj.GetComponent<CompAnimation>();
        anim_controller.PlayAnimation("Idle");
        ToggleMesh(false);

        //Disable Mana bar
        icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
        icon.SetEnabled(false, GetLinkedObject("mana_obj"));

        //Move icon to the right
        daenerys_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
        daenerys_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(115.0f, 430.0f, 0.0f));
        GetLinkedObject("daenerys_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(-123.0f, -31.5f, 0.0f));

        //Disable Daenerys Abilities buttons
        EnableAbilities(false);
    }

    public override void Update()
    {
        base.Update();
    }

    public override void ControlCharacter()
    {
        // Debug method to control Hp
        CheckHealth();
         
        //// First check if you are alive
        health = GetLinkedObject("health_obj").GetComponent<Hp>();
        if (health.GetCurrentHealth() > 0)
        {
            // Check if player is moving to block attacks/abilities
            movement = GetLinkedObject("player_obj").GetComponent<MovementController>();
            if (!movement.IsMoving())
            { 
                /* Player is alive */
                switch (state)
                {
                    case State.IDLE:
                        {
                            //Check For Input + It has to check if she's moving to block attack (¿?)
                            CheckAttack();
                            break;
                        }
                    case State.ATTACKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller = daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller.IsAnimationStopped("Idle"))
                            {
                                anim_controller = daenerys_obj.GetComponent<CompAnimation>();
                                if (set_fire_breath == false && anim_controller.IsAnimOverXTime(0.4f))
                                {
                                    set_fire_breath = true;
                                    particle_system = particle_firebreath_obj.GetComponent<CompParticleSystem>();
                                    particle_system.ActivateEmission(true);
                                    audio = daenerys_obj.GetComponent<CompAudio>();
                                    audio.PlayEvent("Dracarys");

                                    // Attack all enemies in 3 rows in front of you
                                    int tile_x, tile_y;
                                    GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out tile_x, out tile_y);
                                    MovementController.Direction direction = GetLinkedObject("player_obj").GetComponent<MovementController>().curr_dir;
                                    for (int i = 0; i < distance_left_attack; i++)
                                    {
                                        switch (direction)
                                        {
                                            case MovementController.Direction.NORTH:
                                                {
                                                    tile_y -= 1;
                                                    break;
                                                }
                                            case MovementController.Direction.SOUTH:
                                                {
                                                    tile_y += 1;
                                                    break;
                                                }
                                            case MovementController.Direction.EAST:
                                                {
                                                    tile_x += 1;
                                                    break;
                                                }
                                            case MovementController.Direction.WEST:
                                                {
                                                    tile_x -= 1;
                                                    break;
                                                }
                                            default:
                                                {
                                                    break;
                                                }
                                        }
                                        if (GetLinkedObject("player_obj").GetComponent<MovementController>().CheckIsWalkable(tile_x, tile_y) == false)
                                        {
                                            break;
                                        }
                                        GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>().DamageEnemyInTile(tile_x, tile_y, damage_percentage_right);
                                    }

                                    GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 40.0f);

                                    if (coll_object != null)
                                    {
                                        CompCollider obj_collider = coll_object.GetComponent<CompCollider>();

                                        if (obj_collider != null)
                                        {
                                            obj_collider.CallOnContact();
                                        }
                                    }
                                }
                                anim_controller = daenerys_obj.GetComponent<CompAnimation>();
                                if (anim_controller.IsAnimOverXTime(0.9f))
                                {
                                    state = State.IDLE;
                                    particle_system = particle_firebreath_obj.GetComponent<CompParticleSystem>();
                                    particle_system.ActivateEmission(false);
                                }
                            }                          
                            break;
                        }
                    case State.FIRE_WALL:
                        {
                            //Check for end of the Attack animation
                            anim_controller = daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller.IsAnimationStopped("Idle"))
                            {
                                anim_controller = daenerys_obj.GetComponent<CompAnimation>();
                                if (set_fire_wall == false && anim_controller.IsAnimOverXTime(0.3f))
                                {
                                    int tile_x, tile_y;
                                    GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out tile_x, out tile_y);
                                    Vector3 player_pos = GetLinkedObject("player_obj").GetComponent<Transform>().GetPosition();
                                    player_pos.y -= 8.7f;
                                    MovementController.Direction direction = GetLinkedObject("player_obj").GetComponent<MovementController>().curr_dir;
                                    switch (direction)
                                    {
                                        case MovementController.Direction.NORTH:
                                            {
                                                tile_y -= 1;
                                                player_pos.z -= 25.4f;
                                                break;
                                            }
                                        case MovementController.Direction.SOUTH:
                                            {
                                                tile_y += 1;
                                                player_pos.z += 25.4f;
                                                break;
                                            }
                                        case MovementController.Direction.EAST:
                                            {
                                                tile_x += 1;
                                                player_pos.x += 25.4f;
                                                break;
                                            }
                                        case MovementController.Direction.WEST:
                                            {
                                                tile_x -= 1;
                                                player_pos.x -= 25.4f;
                                                break;
                                            }
                                        default:
                                            {
                                                break;
                                            }
                                    }
                                    //GET TILE POS!
                                    GameObject fire_wall = Instantiate("FireWall");
                                    fire_wall.transform.SetPosition(player_pos);
                                    fire_wall.GetComponent<FireWall>().SetTiles(tile_x, tile_y);
                                    set_fire_wall = true;
                                }

                                anim_controller = daenerys_obj.GetComponent<CompAnimation>();
                                if (anim_controller.IsAnimOverXTime(0.9f))
                                {
                                    state = State.IDLE;
                                }
                                else
                                {
                                    // Keep playing specific attack animation  until it ends
                                }
                            }
                            break;
                        }
                    case State.HIT:
                        {
                            //Check for end of the Attack animation
                            anim_controller = daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller.IsAnimationStopped("Hit"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                            }
                            break;
                        }
                    case State.DEAD:
                        {
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }
        }
    }

    public override void CheckAttack()
    {
        if (Input.GetInput_KeyDown("LAttack", "Player"))
        {
            PrepareLeftAbility();
        }
        if (Input.GetInput_KeyDown("RAttack", "Player"))
        {
            PrepareRightAbility();
        }
    }

    public override void GetDamage(float dmg)
    {
        health = GetLinkedObject("health_obj").GetComponent<Hp>();
        health.GetDamage(dmg);

        // SET HIT ANIMATION
        if (health.GetCurrentHealth() > 0)
        {
            if (GetState() == 0)
            {
                SetAnimationTransition("ToHit", true);
                SetState(State.HIT);
            }
            audio = daenerys_obj.GetComponent<CompAudio>();
            audio.PlayEvent("DaenerysHurt");
        }

        else
        {
            SetState(State.DEAD);
            PlayFx("DaenerysDead");
        }
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller = daenerys_obj.GetComponent<CompAnimation>();
        anim_controller.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active, bool left)
    {
        //Update Hp bar
        if (active)
        {
            //Set Icon in the center
            daenerys_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(1.0f, 1.0f, 1.0f));
            daenerys_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(0.0f, 365.0f, 0.0f));
            daenerys_icon_obj_hp.GetComponent<CompImage>().SetEnabled(false, daenerys_icon_obj_hp);
            daenerys_icon_obj_mana.GetComponent<CompImage>().SetEnabled(false, daenerys_icon_obj_mana);

            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            //Disable Stamina Bar
            icon = GetLinkedObject("stamina_obj").GetComponent<CompImage>();
            icon.SetEnabled(false, GetLinkedObject("stamina_obj"));

            //Enable Mana Bar
            icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
            icon.SetEnabled(true, GetLinkedObject("mana_obj"));

            //Update Mana 
            mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
            mana.SetMana(curr_mana, max_mana);

            //Enable Daenerys Abilities buttons
            EnableAbilities(true);

            //Disable Secondary button
            GetLinkedObject("daenerys_s_button_obj").SetActive(false);
        }

        //Get values from var and store them
        else
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            curr_hp = health.GetCurrentHealth();

            mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
            curr_mana = mana.GetCurrentMana();

            //Set icon at the left
            if (left)
            {
                daenerys_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                daenerys_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(-115.0f, 430.0f, 0.0f));
                GetLinkedObject("daenerys_s_button_obj").SetActive(true);
                GetLinkedObject("daenerys_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));
            }
            //Set the icon at the right
            else
            {
                daenerys_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                daenerys_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(115.0f, 430.0f, 0.0f));
                GetLinkedObject("daenerys_s_button_obj").SetActive(true);
                GetLinkedObject("daenerys_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(-123.0f, -31.5f, 0.0f));
            }

            //Enable Secondary Bars & Update them
            daenerys_icon_obj_hp.GetComponent<CompImage>().FillAmount(curr_hp / max_hp);
            daenerys_icon_obj_mana.GetComponent<CompImage>().FillAmount(curr_mana / max_mana);
            daenerys_icon_obj_hp.GetComponent<CompImage>().SetEnabled(true, daenerys_icon_obj_hp);
            daenerys_icon_obj_mana.GetComponent<CompImage>().SetEnabled(true, daenerys_icon_obj_mana);

            //Disable Mana Bar
            icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
            icon.SetEnabled(false, GetLinkedObject("mana_obj"));

            //Enable Stamina Bar
            icon = GetLinkedObject("stamina_obj").GetComponent<CompImage>();
            icon.SetEnabled(true, GetLinkedObject("stamina_obj"));

            //Disable Daenerys Abilities buttons
            EnableAbilities(false);
        }
    }

    public override bool IsAnimationStopped(string name)
    {
        anim_controller = daenerys_obj.GetComponent<CompAnimation>();
        return anim_controller.IsAnimationStopped(name);
    }

    public override bool IsAnimationRunning(string name)
    {
        Debug.Log("[orange] DAENERYS ANIMATION RUNNING");
        anim_controller = daenerys_obj.GetComponent<CompAnimation>();
        return anim_controller.IsAnimationRunning(name);
    }

    public override void ToggleMesh(bool active)
    {
        rarm_daenerys_obj.GetComponent<CompMesh>().SetEnabled(active, rarm_daenerys_obj);
        larm_daenerys_obj.GetComponent<CompMesh>().SetEnabled(active, larm_daenerys_obj);
    }

    //LEFT ARM ------------------------------
    public bool OnLeftClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            float mana_cost = mana_cost_percentage_left * max_mana / 100.0f;
            if (GetCurrentMana() > mana_cost)
            {
                cd_left = daenerys_button_left_obj.GetComponent<DaenerysCD_Left>();
                //Check if the ability has enough charges
                if (cd_left.GetCurrentCharges() > 0)
                {
                    //Set Attacking State
                    SetState(State.ATTACKING);

                    // First, OnClick of LeftWeapon, then, onClick of Cooldown
                    DoLeftAbility();

                    // Set Attacking Animation
                    SetAnimationTransition("ToAttackLeft", true);

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
        return false;
    }

    public void PrepareLeftAbility()
    {
        button = daenerys_button_left_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoLeftAbility() //Might be virtual
    {
        // Decrease mana -----------
        DecreaseManaPercentage(mana_cost_percentage_left);
        set_fire_breath = false;
    }

    //------------------------------
    //RIGHT ARM --------------------

    public bool OnRightClick()
    {
        daenerys_obj.GetComponent<DaenerysController>();
        // Check if player is in Idle State
        if (GetState() == 0)
        {
            // Check if player has enough mana to perform its attack
            int tile_x, tile_y;
            GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out tile_x, out tile_y);
            MovementController.Direction direction = GetLinkedObject("player_obj").GetComponent<MovementController>().curr_dir;
            switch (direction)
            {
                case MovementController.Direction.NORTH:
                    {
                        tile_y -= 1;
                        break;
                    }
                case MovementController.Direction.SOUTH:
                    {
                        tile_y += 1;
                        break;
                    }
                case MovementController.Direction.EAST:
                    {
                        tile_x += 1;
                        break;
                    }
                case MovementController.Direction.WEST:
                    {
                        tile_x -= 1;
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
            if (GetLinkedObject("player_obj").GetComponent<MovementController>().CheckIsWalkable(tile_x, tile_y))
            {
                float mana_cost = mana_cost_percentage_right * max_mana / 100.0f;
                if (GetCurrentMana() > mana_cost)
                {
                    cd_right = daenerys_button_right_obj.GetComponent<DaenerysCD_Right>();
                    //Check if the ability is not in cooldown
                    if (!cd_right.in_cd)
                    {
                        SetState(State.FIRE_WALL);

                        // First, OnClick of RightWeapon, then, onClick of Cooldown
                        DoRightAbility();

                        // Set Animation
                        SetAnimationTransition("ToAttackRight", true);

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
            else
            {
                return false;
            }
        }
        return false;
    }

    public void PrepareRightAbility()
    {
        button = daenerys_button_right_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoRightAbility()
    {
        // Decrease mana -----------
        daenerys_obj.GetComponent<DaenerysController>();
        DecreaseManaPercentage(mana_cost_percentage_right);

        set_fire_wall = false;

        GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 30 * distance_left_attack);
        if (coll_object != null)
        {
            CompCollider obj_collider = coll_object.GetComponent<CompCollider>();
            if (obj_collider != null)
            {
                obj_collider.CallOnContact();
            }

        }
    }

    public bool OnSecondaryClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (GetCurrentStamina() > sec_ability_cost)
            {
                sec_ability_cd = GetLinkedObject("daenerys_s_button_obj").GetComponent<DaenerysCD_Secondary>();
                //Check if the ability is not in cooldown
                if (!sec_ability_cd.in_cd)
                {
                    SecondaryAbility();
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
        return false;
    }

    public override void SecondaryAbility()
    {
        GameObject fball = Instantiate("DaenerysFireball");
        GameObject pla_obj = GetLinkedObject("player_obj");
        fball.transform.SetPosition(pla_obj.transform.GetPosition());
        fball.transform.SetRotation(pla_obj.transform.GetRotation());
        fball.GetComponent<Fireball>().vfront = curr_forward;

        // Decrease stamina -----------
        DecreaseMana(sec_ability_cost);
    }

    public override void EnableAbilities(bool active)
    {
        daenerys_button_left_obj.SetActive(active);
        daenerys_button_right_obj.SetActive(active);
        daenerys_left_flag.SetActive(active);
        daenerys_right_flag.SetActive(active);
    }

    //---------------------------   
}