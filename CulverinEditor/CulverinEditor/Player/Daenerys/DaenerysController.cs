﻿using CulverinEditor;
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
    public GameObject daenerys_fireball_particles;

    CompImage daenerys_icon_mana_bar;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_mana = 100.0f;
    public float curr_mana = 100.0f;
    private float mana_regen = 0.0f;

    public float sec_ability_cost = 30.0f;
    DaenerysCD_Secondary sec_ability_cd;

    //Left Ability Stats
    public float mana_cost_percentage_left = 20f;
    public float damage_percentage_left = 10f;
    public int distance_left_attack = 3;
    private DaenerysCD_Left cd_left;

    //Right Ability Stats
    public float mana_cost_percentage_right = 20f;
    public float damage_percentage_right = 10f;
    private DaenerysCD_Right cd_right;
    private bool set_fire_wall = false;

    private bool play_audio_roar = false;

    public GameObject daenerys_button_left_obj_idle;
    public GameObject daenerys_button_right_obj_idle;

    public GameObject Global_Camera;

    protected override void Start()
    {
        SetPosition(Position.BEHIND_RIGHT);

        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        daenerys_obj = GetLinkedObject("daenerys_obj");
        rarm_daenerys_obj = GetLinkedObject("rarm_daenerys_obj");
        larm_daenerys_obj = GetLinkedObject("larm_daenerys_obj");

        daenerys_icon_obj = GetLinkedObject("daenerys_icon_obj");
        daenerys_button_left_obj = GetLinkedObject("daenerys_button_left_obj");
        daenerys_button_right_obj = GetLinkedObject("daenerys_button_right_obj");
        daenerys_button_left_obj_idle = GetLinkedObject("daenerys_button_left_obj_idle");
        daenerys_button_right_obj_idle = GetLinkedObject("daenerys_button_right_obj_idle");

        daenerys_left_flag = GetLinkedObject("daenerys_left_flag");
        daenerys_right_flag = GetLinkedObject("daenerys_right_flag");

        daenerys_icon_obj_hp = GetLinkedObject("daenerys_icon_obj_hp");
        daenerys_icon_obj_mana = GetLinkedObject("daenerys_icon_obj_mana");
        mana_regen = GetLinkedObject("mana_obj").GetComponent<Mana>().regen;

        daenerys_fireball_particles = GetLinkedObject("daenerys_fireball_particles");

        Global_Camera = GetLinkedObject("Global_Camera");

        // Start Idle animation
        anim_controller = daenerys_obj.GetComponent<CompAnimation>();
        anim_controller.PlayAnimationNode("Out");
        ToggleMesh(false);

        //Disable Mana bar
        icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
        icon.SetEnabled(false, GetLinkedObject("mana_obj"));

        //Move icon to the right
        daenerys_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
        daenerys_icon_obj.GetComponent<CompRectTransform>().SetUIPosition(new Vector3(115.0f, 100.0f, 0.0f));
        daenerys_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(1.0f, 1.0f, 1.0f), 1.0f);
        GetLinkedObject("daenerys_s_button_obj").GetComponent<CompRectTransform>().SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));
        GetLinkedObject("daenerys_s_button_obj_idle").GetComponent<CompRectTransform>().SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));

        //Disable Daenerys Abilities buttons
        EnableAbilities(false);
    }

    public override void Update()
    {
        base.Update();
    }

    public override void ControlCharacter()
    {
         
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
                                if (anim_controller.IsAnimOverXTime(0.6f) && play_audio_roar)
                                {
                                    GameObject flamethr = Instantiate("CorrectDaenerysFireBreath");

                                    flamethr.transform.SetPosition(GetFlameBreathPosition(curr_position));
                                    flamethr.transform.SetRotation(flamethr.transform.GetRotation());

                                    Fireball fballscript = flamethr.GetComponent<Fireball>();
                                    fballscript.vfront = curr_forward;

                                    GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
                                    if (coll_object != null)
                                    {
                                        coll_object.GetTag();
                                        if (coll_object.CompareTag("Enemy"))
                                        {
                                            fballscript.vfront = GetSecondaryForwardToEnemy(flamethr.transform.GetPosition(), coll_object.transform.GetPosition());
                                        }
                                    }
                                    PlayFx("DaenerysDragonRoar");                             
                                    play_audio_roar = false;
                                }

                                if (anim_controller.IsAnimOverXTime(1.0f))
                                {
                                    state = State.IDLE;
                                }
                            }
                            else if(play_audio_roar == false)
                            {
                                state = State.IDLE;
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
                                    player_pos.y -= 9.1f;
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
                                if (anim_controller.IsAnimOverXTime(0.8f))
                                {
                                    state = State.IDLE;
                                }
                            }
                            else if(set_fire_wall == true && IsAnimationStopped("AttackRight"))
                            {
                                state = State.IDLE;
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

    public override void ManageEnergy()
    {
        if (state != State.DEAD)
        {
            if (curr_mana < max_mana)
            {
                curr_mana += mana_regen;
                if (curr_mana > max_mana)
                {
                    curr_mana = max_mana;
                }
                float calc_mana = curr_mana / max_mana;
                daenerys_icon_mana_bar = daenerys_icon_obj_mana.GetComponent<CompImage>();
                daenerys_icon_mana_bar.FillAmount(calc_mana);
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

    public override bool GetDamage(float dmg)
    {
        health = GetLinkedObject("health_obj").GetComponent<Hp>();
        health.GetDamage(dmg);

        // SET HIT ANIMATION
        if (health.GetCurrentHealth() > 0)
        {
            if (GetState() == 0)
            {
                Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("Hit");
                SetAnimationTransition("ToHit", true);
                SetState(State.HIT);
            }
            PlayFx("DaenerysHurt");

            //Damage Feedback
            GetLinkedObject("player_obj").GetComponent<DamageFeedback>().SetDamage(health.GetCurrentHealth(), max_hp);
        }

        else
        {
            SetAnimationTransition("ToDeath", true);
            Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("D_Death");
            SetState(State.DEAD);
            PlayFx("DaenerysDead");
        }

        return true;
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
            daenerys_icon_obj.GetComponent<CompRectTransform>().SetUIPosition(new Vector3(0.0f, 22.0f, 0.0f));
            daenerys_icon_obj_hp.GetComponent<CompImage>().SetEnabled(false, daenerys_icon_obj_hp);
            daenerys_icon_obj_mana.GetComponent<CompImage>().SetEnabled(false, daenerys_icon_obj_mana);

            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            //Disable Stamina Bar
            icon = GetLinkedObject("stamina_obj").GetComponent<CompImage>();
            icon.SetEnabled(false, GetLinkedObject("stamina_obj"));

            icon = GetLinkedObject("leftamina_bar").GetComponent<CompImage>();
            icon.SetEnabled(false, GetLinkedObject("leftamina_bar"));

            //Enable Mana Bar
            icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
            icon.SetEnabled(true, GetLinkedObject("mana_obj"));

            icon = GetLinkedObject("leftmana_bar").GetComponent<CompImage>();
            icon.SetEnabled(true, GetLinkedObject("leftmana_bar"));

            //Update Mana 
            mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
            mana.SetMana(curr_mana, max_mana);

            //Enable Daenerys Abilities buttons
            EnableAbilities(true);

            //Disable Secondary button
            GetLinkedObject("daenerys_s_button_obj").GetComponent<CompButton>().SetInteractivity(false);
            GetLinkedObject("daenerys_s_button_obj").GetComponent<CompImage>().SetRender(false);
            GetLinkedObject("daenerys_s_button_obj_idle").GetComponent<CompImage>().SetRender(false);

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
                daenerys_icon_obj.GetComponent<CompRectTransform>().SetUIPosition(new Vector3(-115.0f, 100.0f, 0.0f));
                GetLinkedObject("daenerys_s_button_obj").GetComponent<CompRectTransform>().SetUIPosition(new Vector3(124.0f, -33.0f, 0.0f));
                GetLinkedObject("daenerys_s_button_obj_idle").GetComponent<CompRectTransform>().SetUIPosition(new Vector3(124.0f, -33.0f, 0.0f));

            }
            //Set the icon at the right
            else
            {
                daenerys_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                daenerys_icon_obj.GetComponent<CompRectTransform>().SetUIPosition(new Vector3(115.0f, 100.0f, 0.0f));
                GetLinkedObject("daenerys_s_button_obj").GetComponent<CompRectTransform>().SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));
                GetLinkedObject("daenerys_s_button_obj_idle").GetComponent<CompRectTransform>().SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));
            }

            //Enable Secondary Button
            GetLinkedObject("daenerys_s_button_obj").GetComponent<CompButton>().SetInteractivity(true);
            GetLinkedObject("daenerys_s_button_obj").GetComponent<CompImage>().SetRender(true);
            GetLinkedObject("daenerys_s_button_obj_idle").GetComponent<CompImage>().SetRender(true);

            //Enable Secondary Bars & Update them
            daenerys_icon_obj_hp.GetComponent<CompImage>().FillAmount(curr_hp / max_hp);
            daenerys_icon_obj_mana.GetComponent<CompImage>().FillAmount(curr_mana / max_mana);
            daenerys_icon_obj_hp.GetComponent<CompImage>().SetEnabled(true, daenerys_icon_obj_hp);
            daenerys_icon_obj_mana.GetComponent<CompImage>().SetEnabled(true, daenerys_icon_obj_mana);

            //Disable Mana Bar
            icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
            icon.SetEnabled(false, GetLinkedObject("mana_obj"));

            icon = GetLinkedObject("leftmana_bar").GetComponent<CompImage>();
            icon.SetEnabled(false, GetLinkedObject("leftmana_bar"));

            //Enable Stamina Bar
            icon = GetLinkedObject("stamina_obj").GetComponent<CompImage>();
            icon.SetEnabled(true, GetLinkedObject("stamina_obj"));

            icon = GetLinkedObject("leftamina_bar").GetComponent<CompImage>();
            icon.SetEnabled(true, GetLinkedObject("leftamina_bar"));

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
            if (CanWasteMana(mana_cost))
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

                    Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("D_Firebreath");

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
        audio = daenerys_obj.GetComponent<CompAudio>();
        PlayFx("Dracarys");

        play_audio_roar = true;
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
                if (CanWasteMana(mana_cost))
                {
                    cd_right = daenerys_button_right_obj.GetComponent<DaenerysCD_Right>();
                    //Check if the ability is not in cooldown
                    if (!cd_right.in_cd)
                    {
                        SetState(State.FIRE_WALL);

                        // First, OnClick of RightWeapon, then, onClick of Cooldown
                        DoRightAbility();

                        // Set Animation
                        Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("D_Firewall");
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
            if (CanWasteStamina(sec_ability_cost))
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

        fball.transform.SetPosition(GetSecondaryPosition(curr_position));
        fball.transform.SetRotation(pla_obj.transform.GetRotation());

        Fireball fballscript = fball.GetComponent<Fireball>();
        fballscript.vfront = curr_forward;

        GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
        if(coll_object != null)
        {
            coll_object.GetTag();
            if(coll_object.CompareTag("Enemy"))
            {
                fballscript.vfront = GetSecondaryForwardToEnemy(fball.transform.GetPosition(), coll_object.transform.GetPosition());
            }
        }
        // Decrease stamina -----------
        DecreaseStamina(sec_ability_cost);
    }

    public override void EnableAbilities(bool active)
    {
        //daenerys_button_left_obj.SetActive(active);
        //daenerys_button_right_obj.SetActive(active);

        //Disable Button Interaction
        daenerys_button_left_obj.GetComponent<CompButton>().SetInteractivity(active);
        daenerys_button_right_obj.GetComponent<CompButton>().SetInteractivity(active);

        //Disable Image
        daenerys_button_left_obj.GetComponent<CompImage>().SetRender(active);
        daenerys_button_right_obj.GetComponent<CompImage>().SetRender(active);
        daenerys_button_left_obj_idle.GetComponent<CompImage>().SetRender(active);
        daenerys_button_right_obj_idle.GetComponent<CompImage>().SetRender(active);

        //Right Cooldown Text Render
        GetLinkedObject("daenerys_right_cd_text").GetComponent<CompText>().SetRender(active);
        //Left Cooldown Text Render
        GetLinkedObject("daenerys_left_cd_text").GetComponent<CompText>().SetRender(active);
        //Sec Cooldown Text Render
        GetLinkedObject("daenerys_secondary_cd_text").GetComponent<CompText>().SetRender(!active);

        //Disable Flags
        daenerys_left_flag.SetActive(active);
        daenerys_right_flag.SetActive(active);
    }

    public void Heal(float percentage)
    {
        if (state != State.DEAD)
        {
            curr_hp += max_hp * percentage;
            if (curr_hp > max_hp)
            {
                curr_hp = max_hp;
            }
            daenerys_icon_obj_hp.GetComponent<CompImage>().FillAmount(curr_hp / max_hp);
        }
    }
}