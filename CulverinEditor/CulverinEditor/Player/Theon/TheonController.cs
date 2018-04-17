using CulverinEditor;
using CulverinEditor.Debug;

public class TheonController : CharacterController
{
    //MESH ELEMENTS
    public GameObject theon_obj; 
    public GameObject L_Arm_Theon;
    public GameObject R_Arm_Theon;
    public GameObject CrossBow;
    public GameObject Arrow;

    //UI ELEMENTS
    public GameObject theon_icon_obj;
    public GameObject theon_icon_obj_hp;
    public GameObject theon_icon_obj_stamina;
    public GameObject theon_button_left;
    public GameObject theon_button_right;
    public GameObject theon_left_flag;
    public GameObject theon_right_flag;

    CompImage theon_icon_stamina_bar;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;
    private float stamina_regen = 0.0f;

    //LEFT ABILITY STATS-------------------
    public float left_ability_dmg = 10;
    public float left_ability_cost = 10.0f;
    private TheonCD_Left cd_left;
    //----------------------------------------
    //RIGHT ABILITY STATS-------------------
    public bool reloading = false;
    public float right_ability_dmg = 10;
    public float right_ability_cost = 10.0f;
    private TheonCD_Right cd_right;
    bool do_push_attack = false;

    public bool secondary_ability = false;
    public float sec_ability_cost = 30;
    TheonCD_Secondary sec_ability_cd;

    bool arrow1 = false;
    bool arrow2 = false;
    bool arrow3 = false;
    float arrowtimers = 0.0f;

    public GameObject theon_blood_particles;
    public GameObject theon_sparks_particles;

    public GameObject theon_button_left_idle;
    public GameObject theon_button_right_idle;
    //----------------------------------------

    protected override void Start()
    {
        SetPosition(Position.BEHIND_LEFT);

        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        theon_obj = GetLinkedObject("theon_obj");
        L_Arm_Theon = GetLinkedObject("L_Arm_Theon");
        R_Arm_Theon = GetLinkedObject("R_Arm_Theon");
        CrossBow = GetLinkedObject("CrossBow");
        Arrow = GetLinkedObject("Arrow");

        theon_icon_obj = GetLinkedObject("theon_icon_obj");
        theon_button_right = GetLinkedObject("theon_button_right");
        theon_button_left = GetLinkedObject("theon_button_left");
        theon_button_right_idle = GetLinkedObject("theon_button_right_idle");
        theon_button_left_idle = GetLinkedObject("theon_button_left_idle");
        theon_left_flag = GetLinkedObject("theon_left_flag");
        theon_right_flag = GetLinkedObject("theon_right_flag");

        theon_icon_obj_hp = GetLinkedObject("theon_icon_obj_hp");
        theon_icon_obj_stamina = GetLinkedObject("theon_icon_obj_stamina");
        stamina_regen = GetLinkedObject("stamina_obj").GetComponent<Stamina>().regen;

        theon_blood_particles = GetLinkedObject("theon_blood_particles");
        theon_sparks_particles = GetLinkedObject("theon_sparks_particles");

        //Start Idle animation
        anim_controller = theon_obj.GetComponent<CompAnimation>();
        anim_controller.PlayAnimationNode("Out");
        ToggleMesh(false);

        //Move icon to the left
        theon_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
        theon_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(-115.0f, 430.0f, 0.0f));
        theon_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(1.0f, 1.0f, 1.0f), 1.0f);
        GetLinkedObject("theon_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));
        GetLinkedObject("theon_s_button_obj_idle").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));

        //Disable Theon Abilities buttons
        EnableAbilities(false);

        arrow1 = false;
        arrow2 = false;
        arrow3 = false;
        secondary_ability = false;
        reloading = false;
        arrowtimers = 0.0f;
    }

    public override void Update()
    {
        base.Update();
    }

    public override void ControlCharacter()
    {
        // First check if you are alive
        health = GetLinkedObject("health_obj").GetComponent<Hp>();
        if (health.GetCurrentHealth() > 0)
        {
            // Check if player is moving to block attacks/abilities
            movement = GetLinkedObject("player_obj").GetComponent<MovementController>();
            if (!movement.IsMoving())
            {
                /* Player is alive */
                Debug.Log("THEON STATE:" + state, Department.PLAYER);
                switch (state)
                {
                    case State.IDLE:
                        {
                           
                            //Check For Input + It has to check if he's moving to block attack (¿?)
                            CheckAttack();
                        
                            if (reloading)
                            {
                                anim_controller = theon_obj.GetComponent<CompAnimation>();
                                if (anim_controller.IsAnimationRunning("Idle"))
                                    reloading = false;
                            }
                            break;
                        }
                    case State.ATTACKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller = theon_obj.GetComponent<CompAnimation>();

                            if (anim_controller.IsAnimationStopped("Attack"))
                            {
                                CrossBow.GetComponent<CompAnimation>().PlayAnimation("Reload");
                                state = State.RELOADING;
                                reloading = true;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                            }
                            break;
                        }
                    case State.RELOADING:
                        {
                            //Check for end of the Attack animation
                            anim_controller = theon_obj.GetComponent<CompAnimation>();
                            if (anim_controller.IsAnimOverXTime(0.5f)) 
                            {
                                //Activate arrow Placement
                                Arrow.GetComponent<CompMesh>().SetEnabled(true, Arrow);                       
                                PlayFx("CrossbowRecharge");
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                            }
                            break;
                        }
                    case State.STUN:
                        {
                            //Check for end of the Attack animation
                            //anim_controller = theon_obj.GetComponent<CompAnimation>();
                            
                            //Apply damage over x time of the attack animation
                            if (do_push_attack && theon_obj.GetComponent<CompAnimation>().IsAnimOverXTime(0.6f))
                            {
                                Debug.Log("CAN I DO RIGHT ATTACK", Department.IA);
                                DoRightAbility();
                                do_push_attack = false;
                            }
             

                            if (theon_obj.GetComponent<CompAnimation>().IsAnimationStopped("Attack2"))
                            { 
                                state = State.IDLE;
                            }
                            break;
                        }
                    case State.BLOCKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller = theon_obj.GetComponent<CompAnimation>();
                            if (anim_controller.IsAnimationStopped("Block"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                            }
                            break;
                        }
                    case State.HIT:
                        {
                            //Check for end of the Attack animation
                            anim_controller = theon_obj.GetComponent<CompAnimation>();
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

    public override void ManageEnergy()
    {
        if (state != State.DEAD)
        {
            //Regen Stamina Bar
            if (curr_stamina < max_stamina)
            {
                curr_stamina += stamina_regen;
                if (curr_stamina > max_stamina)
                {
                    curr_stamina = max_stamina;
                }
                float calc_stamina = curr_stamina / max_stamina;
                theon_icon_stamina_bar = theon_icon_obj_stamina.GetComponent<CompImage>();
                theon_icon_stamina_bar.FillAmount(calc_stamina);
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

    public override void SecondaryAbility()
    {
        secondary_ability = true;
        // Decrease stamina -----------
        DecreaseStamina(sec_ability_cost);
        DecreaseMana(sec_ability_cost);
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
                Debug.Log("GET STATE DAMAGE", Department.IA);
                SetAnimationTransition("ToHit", true);
                SetState(State.HIT);
            }
            PlayFx("TheonHurt");
        }

        else
        {
            SetAnimationTransition("ToDeath", true);
            SetState(State.DEAD);
            PlayFx("TheonDead");
        }

        return true;
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller = theon_obj.GetComponent<CompAnimation>();
        anim_controller.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active, bool left)
    {
        //Update Hp bar
        if (active)
        {
            //Set Icon in the center
            theon_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(1.0f, 1.0f, 1.0f));
            theon_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(0.0f, 365.0f, 0.0f));
            theon_icon_obj_hp.GetComponent<CompImage>().SetEnabled(false, theon_icon_obj_hp);
            theon_icon_obj_stamina.GetComponent<CompImage>().SetEnabled(false, theon_icon_obj_stamina);
            
            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            //Update Stamina
            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            stamina.SetStamina(curr_stamina, max_stamina);

            //Enable Theon Abilities buttons
            EnableAbilities(true);

            //Disable Secondary button
            GetLinkedObject("theon_s_button_obj").GetComponent<CompButton>().SetInteractivity(false);
            GetLinkedObject("theon_s_button_obj").GetComponent<CompImage>().SetRender(false);
            GetLinkedObject("theon_s_button_obj_idle").GetComponent<CompImage>().SetRender(false);
        }

        //Get values from var and store them
        else
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            curr_hp = health.GetCurrentHealth();

            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            curr_stamina = stamina.GetCurrentStamina();

            //Set icon at the left
            if (left)
            {
                theon_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                theon_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(-115.0f, 430.0f, 0.0f));
                GetLinkedObject("theon_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));
                GetLinkedObject("theon_s_button_obj_idle").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));
            }

            //Set the icon at the right
            else
            {
                theon_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                theon_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(115.0f, 430.0f, 0.0f));
                GetLinkedObject("theon_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(-123.0f, -31.5f, 0.0f));
                GetLinkedObject("theon_s_button_obj_idle").GetComponent<CompRectTransform>().SetPosition(new Vector3(-123.0f, -31.5f, 0.0f));
            }

            //Enable Secondary Button
            GetLinkedObject("theon_s_button_obj").GetComponent<CompButton>().SetInteractivity(true);
            GetLinkedObject("theon_s_button_obj").GetComponent<CompImage>().SetRender(true);
            GetLinkedObject("theon_s_button_obj_idle").GetComponent<CompImage>().SetRender(true);

            //Enable Secondary Bars & Update them
            theon_icon_obj_hp.GetComponent<CompImage>().FillAmount(curr_hp / max_hp);
            theon_icon_obj_stamina.GetComponent<CompImage>().FillAmount(curr_stamina / max_stamina);
            theon_icon_obj_hp.GetComponent<CompImage>().SetEnabled(true, theon_icon_obj_hp);
            theon_icon_obj_stamina.GetComponent<CompImage>().SetEnabled(true, theon_icon_obj_stamina);

            //Disable Theon Abilities buttons
            EnableAbilities(false);
        }
    }

    public override bool IsAnimationStopped(string name)
    {
        anim_controller = theon_obj.GetComponent<CompAnimation>();
        return anim_controller.IsAnimationStopped(name);
    }

    public override bool IsAnimationRunning(string name)
    {
        anim_controller = theon_obj.GetComponent<CompAnimation>();
        return anim_controller.IsAnimationRunning(name);
    }

    public override void ToggleMesh(bool active)
    {
        L_Arm_Theon.GetComponent<CompMesh>().SetEnabled(active, L_Arm_Theon);
        R_Arm_Theon.GetComponent<CompMesh>().SetEnabled(active, R_Arm_Theon);
        CrossBow.GetComponent<CompMesh>().SetEnabled(active, CrossBow);
        Arrow.GetComponent<CompMesh>().SetEnabled(active, Arrow);
    }

    public bool OnLeftClick()
    {
        // Check if player is in Idle State
        if (state == State.IDLE) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(left_ability_cost))
            {
                cd_left = theon_button_left.GetComponent<TheonCD_Left>();
                //Check if the ability is not in cooldown
                if (!cd_left.in_cd)
                { 
                    // First, OnClick of LeftWeapon, then, onClick of Cooldown
                    DoLeftAbility();

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
        button = theon_button_left.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoLeftAbility() //Might be virtual
    {
        // Decrease stamina -----------
        DecreaseStamina(left_ability_cost);

        GameObject arrow = Instantiate("ArrowTheon");
        GameObject player = GetLinkedObject("player_obj");

        arrow.transform.SetRotation(player.transform.GetRotation());
        arrow.transform.SetPosition(new Vector3(curr_position.x, curr_position.y - 1.5f, curr_position.z));
        Arrow arrow_script = arrow.GetComponent<Arrow>();
        arrow_script.speed = curr_forward;

        arrow_script.arrow_blood_particles = theon_blood_particles;
        arrow_script.arrow_sparks_particles = theon_sparks_particles;

        Arrow.GetComponent<CompMesh>().SetEnabled(false, Arrow);

        //Set Attack Animation
        SetAnimationTransition("ToAttack1", true);
        CrossBow.GetComponent<CompAnimation>().PlayAnimation("Attack");

        PlayFx("CrossbowShot");

        SetState(CharacterController.State.ATTACKING);
    }

    public void PlayFx()
    {
        //GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("SwordSlash");
    }

    public bool OnRightClick()
    {     
        // Check if player is in Idle State
        if (state == State.IDLE)
        {
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(right_ability_cost))
            {
                cd_right = theon_button_right.GetComponent<TheonCD_Right>();
                //Check if the ability is not in cooldown
                if (!cd_right.in_cd)
                {
                    // Decrease stamina -----------
                    DecreaseStamina(right_ability_cost);

                    PlayFx("TheonMeleShout");
                    SetAnimationTransition("ToAttack2", true);

                    do_push_attack = true;

                    //Go to PUSH State
                    SetState(State.STUN); 
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

    public void PrepareRightAbility()
    {
        button = theon_button_right.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoRightAbility() //Might be virtual
    {
        GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 40.0f);

        Debug.Log("RIGHT ABILITY", Department.IA);

        if (coll_object != null)
        {

            Debug.Log("OBJ COLLECTED:" + coll_object.GetTag(), Department.IA);

            PlayFx("TheonMeleHit");

            if (coll_object.GetTag() == "Enemy")
            {
                Debug.Log("IS AN ENEMY", Department.IA);
                // Check the specific enemy in front of you and apply dmg or call object OnContact
                EnemiesManager enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();
                movement = GetLinkedObject("player_obj").GetComponent<MovementController>();
                enemy_manager.Push(coll_object, movement.GetForwardDir());
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
                sec_ability_cd = GetLinkedObject("theon_s_button_obj").GetComponent<TheonCD_Secondary>();
                //Check if the ability is not in cooldown
                if (!sec_ability_cd.in_cd)
                {
                    PlayFx("TheonRope");
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

    public override void EnableAbilities(bool active)
    {
        //theon_button_left.SetActive(active);
        //theon_button_right.SetActive(active);

        //Disable Button Interaction
        theon_button_left.GetComponent<CompButton>().SetInteractivity(active);
        theon_button_right.GetComponent<CompButton>().SetInteractivity(active);  

        //Disable Image
        theon_button_left.GetComponent<CompImage>().SetRender(active);
        theon_button_right.GetComponent<CompImage>().SetRender(active);
        theon_button_left_idle.GetComponent<CompImage>().SetRender(active);
        theon_button_right_idle.GetComponent<CompImage>().SetRender(active);

        //Right Cooldown Text Render
        GetLinkedObject("theon_right_cd_text").GetComponent<CompText>().SetRender(active);
        //Left Cooldown Text Render
        GetLinkedObject("theon_left_cd_text").GetComponent<CompText>().SetRender(active);
        //Sec Cooldown Text Render
        GetLinkedObject("theon_secondary_cd_text").GetComponent<CompText>().SetRender(!active);


        //Disable Flags
        theon_left_flag.SetActive(active);
        theon_right_flag.SetActive(active);
    }

    public void TheonSecondaryAbility()
    {
        if(secondary_ability == true)
        {
            arrowtimers += Time.deltaTime;
            if(arrowtimers >= 0.5f && arrow1 == false)
            {
                arrow1 = true;
                PlayFx("CrossbowShot");
                GameObject arrow = Instantiate("ArrowTheon");
                GameObject player = GetLinkedObject("player_obj");

                arrow.transform.SetPosition(GetSecondaryPosition(curr_position));
                arrow.transform.SetRotation(player.transform.GetRotation());

                Arrow arrow_script = arrow.GetComponent<Arrow>();
                arrow_script.speed = GetSecondaryForward(curr_forward);

                GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
                if (coll_object != null)
                {
                    coll_object.GetTag();
                    if (coll_object.CompareTag("Enemy"))
                    {
                        arrow_script.speed = GetSecondaryForwardToEnemy(arrow.transform.GetPosition(), coll_object.transform.GetPosition());
                    }
                }

                arrow_script.arrow_blood_particles = theon_blood_particles;
                arrow_script.arrow_sparks_particles = theon_sparks_particles;
            }

            if (arrowtimers >= 1.0f && arrow2 == false)
            {
                arrow2 = true;
                PlayFx("CrossbowShot");

                GameObject arrow = Instantiate("ArrowTheon");
                GameObject player = GetLinkedObject("player_obj");

                arrow.transform.SetPosition(GetSecondaryPosition(curr_position));
                arrow.transform.SetRotation(player.transform.GetRotation());

                Arrow arrow_script = arrow.GetComponent<Arrow>();
                arrow_script.speed = GetSecondaryForward(curr_forward);

                GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
                if (coll_object != null)
                {
                    coll_object.GetTag();
                    if (coll_object.CompareTag("Enemy"))
                    {
                        arrow_script.speed = GetSecondaryForwardToEnemy(arrow.transform.GetPosition(), coll_object.transform.GetPosition());
                    }
                }

                arrow_script.arrow_blood_particles = theon_blood_particles;
                arrow_script.arrow_sparks_particles = theon_sparks_particles;
            }

            if (arrowtimers >= 1.5f && arrow3 == false)
            {
                arrowtimers = 0.0f;
                arrow1 = false;
                arrow2 = false;
                secondary_ability = false;

                PlayFx("CrossbowShot");

                GameObject arrow = Instantiate("ArrowTheon");
                GameObject player = GetLinkedObject("player_obj");

                arrow.transform.SetPosition(GetSecondaryPosition(curr_position));
                arrow.transform.SetRotation(player.transform.GetRotation());

                Arrow arrow_script = arrow.GetComponent<Arrow>();
                arrow_script.speed = GetSecondaryForward(curr_forward);

                GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
                if (coll_object != null)
                {
                    coll_object.GetTag();
                    if (coll_object.CompareTag("Enemy"))
                    {
                        arrow_script.speed = GetSecondaryForwardToEnemy(arrow.transform.GetPosition(), coll_object.transform.GetPosition());
                    }
                }

                arrow_script.arrow_blood_particles = theon_blood_particles;
                arrow_script.arrow_sparks_particles = theon_sparks_particles;
            }
        }
    }

    public void ActivateSecondaryAbility()
    {
        secondary_ability = true;
    }

    public void Heal()
    {
        curr_hp = max_hp;
    }
}