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
    public GameObject theon_button_left_idle;
    public GameObject theon_left_text_counter;

    public GameObject theon_button_right;
    public GameObject theon_button_right_idle;
    public GameObject theon_right_text_counter;


    GameObject theon_s_button;
    GameObject theon_s_button_idle;
    public GameObject theon_sec_text_counter;


    public GameObject theon_left_flag;
    public GameObject theon_right_flag;

    //PARTICLES
    public GameObject theon_blood_particles;
    public GameObject theon_sparks_particles;

    //CAMERA
    public GameObject Global_Camera;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;
    private float stamina_regen = 0.0f;

    //Sec Ability Stats
    public float sec_ability_dmg = 30.0f;
    public float sec_ability_cost = 50.0f;
    public float sec_ability_cd_time = 20.0f;
    private TheonCD_Secondary sec_ability_cd;

    //Left Ability Stats
    public float left_ability_dmg = 10;
    public float left_ability_cost = 10.0f;
    public float left_ability_cd_time = 2.0f;
    private TheonCD_Left cd_left;

    //Right Ability Stats
    public float right_ability_dmg = 10;
    public float right_ability_cost = 10.0f;
    public float right_ability_cd_time = 2.0f;
    private TheonCD_Right cd_right;
    bool do_push_attack = false;

    bool arrow1 = false;
    bool arrow2 = false;
    bool arrow3 = false;
    float arrowtimers = 0.0f;

    public bool reloading = false;
    public bool secondary_ability = false;

    protected override void Start()
    {
        base.Start();

        SetPosition(Position.BEHIND_LEFT);

        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        theon_obj = GetLinkedObject("theon_obj");
        L_Arm_Theon = GetLinkedObject("L_Arm_Theon");
        R_Arm_Theon = GetLinkedObject("R_Arm_Theon");
        CrossBow = GetLinkedObject("CrossBow");
        Arrow = GetLinkedObject("Arrow");

        theon_icon_obj = GetLinkedObject("theon_icon_obj");

        theon_button_left = GetLinkedObject("theon_button_left");
        theon_button_left_idle = GetLinkedObject("theon_button_left_idle");
        theon_left_text_counter = GetLinkedObject("theon_left_text_counter");
        cd_left = theon_button_left.GetComponent<TheonCD_Left>();

        theon_button_right = GetLinkedObject("theon_button_right");
        theon_button_right_idle = GetLinkedObject("theon_button_right_idle");
        theon_right_text_counter = GetLinkedObject("theon_right_text_counter");
        cd_right = theon_button_right.GetComponent<TheonCD_Right>();

        theon_s_button = GetLinkedObject("theon_s_button_obj");
        theon_s_button_idle = GetLinkedObject("theon_s_button_obj_idle");
        theon_sec_text_counter = GetLinkedObject("theon_sec_text_counter");
        sec_ability_cd = theon_s_button.GetComponent<TheonCD_Secondary>();

        theon_left_flag = GetLinkedObject("theon_left_flag");
        theon_right_flag = GetLinkedObject("theon_right_flag");

        theon_icon_obj_hp = GetLinkedObject("theon_icon_obj_hp");
        theon_icon_obj_stamina = GetLinkedObject("theon_icon_obj_stamina");
        stamina_regen = stamina.GetComponent<Stamina>().regen;

        theon_blood_particles = GetLinkedObject("theon_blood_particles");
        theon_sparks_particles = GetLinkedObject("theon_sparks_particles");

        Global_Camera = GetLinkedObject("Global_Camera");

        //Link components with correct gameobjects
        LinkComponents(theon_icon_obj, theon_icon_obj_hp, theon_icon_obj_stamina, null,
                       theon_button_left, theon_button_right, theon_s_button, theon_s_button_idle,
                       theon_left_text_counter, theon_right_text_counter, theon_sec_text_counter,
                       L_Arm_Theon, R_Arm_Theon, theon_button_left_idle, theon_button_right_idle);

        //Start Idle animation
        anim_controller.PlayAnimationNode("Out");
        ToggleMesh(false);

        //Move icon to the left
        icon_trans.SetScale(new Vector3(0.7f, 0.7f, 0.7f));
        icon_trans.SetUIPosition(new Vector3(-115.0f, 100.0f, 0.0f));

        icon_img.SetColor(new Vector3(1.0f, 1.0f, 1.0f), 1.0f);

        sec_button_trans.SetUIPosition(new Vector3(124.0f, -33.0f, 0.0f));
        sec_button_idle_trans.SetUIPosition(new Vector3(124.0f, -33.0f, 0.0f));

        arrow1 = false;
        arrow2 = false;
        arrow3 = false;
        secondary_ability = false;
        reloading = false;
        arrowtimers = 0.0f;

        //Disable Theon Abilities buttons
        EnableAbilities(false);
    }

    public override void Update()
    {
        base.Update();
    }

    public override void ControlCharacter()
    {
        //Debug.Log(state, Department.PLAYER);
        // First check if you are alive
        if (health.GetCurrentHealth() > 0)
        {
            // Check if player is moving to block attacks/abilities
            if (!movement.IsMoving())
            {
                /* Player is alive */
                switch (state)
                {
                    case State.IDLE:
                        {                      
                            //Check For Input + It has to check if he's moving to block attack (¿?)
                            CheckAttack();                     
                            if (reloading)
                            {
                                if (anim_controller.IsAnimationRunning("Idle"))
                                {
                                    reloading = false;
                                }
                            }
                            break;
                        }
                    case State.ATTACKING:
                        {
                            //Check for end of the Attack animation
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
                            if (anim_controller.IsAnimOverXTime(0.5f)) 
                            {
                                //Activate arrow Placement
                                Arrow.GetComponent<CompMesh>().SetEnabled(true);                       
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
                            //Apply damage over x time of the attack animation
                            if (do_push_attack && anim_controller.IsAnimOverXTime(0.6f))
                            {
                                DoRightAbility();
                                do_push_attack = false;
                            }    

                            if (anim_controller.IsAnimationStopped("Attack2"))
                            { 
                                state = State.IDLE;
                            }
                            break;
                        }
                    case State.BLOCKING:
                        {
                            //Check for end of the Attack animation
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
                icon_stamina_img.FillAmount(calc_stamina);
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

            PlayFx("TheonHurt");

            //Damage Feedback
            damage_feedback.SetDamage(health.GetCurrentHealth(), max_hp);
        }

        else
        {
            Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("T_Death");
            SetAnimationTransition("ToDeath", true);
            SetState(State.DEAD);

            PlayFx("TheonDead");
        }

        return true;
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active, bool left)
    {
        //Update Hp bar
        if (active)
        {
            //Set Icon in the center
            icon_trans.SetScale(new Vector3(1.0f, 1.0f, 1.0f));
            icon_trans.SetUIPosition(new Vector3(0.0f, 22.0f, 0.0f));
            icon_hp_img.SetEnabled(false);
            icon_stamina_img.SetEnabled(false);
            
            //Update HP
            health.SetHP(curr_hp, max_hp);

            //Update Stamina
            stamina.SetStamina(curr_stamina, max_stamina);

            //Enable Theon Abilities buttons
            EnableAbilities(true);

            //Disable Secondary button
            sec_button.SetInteractivity(false);
            sec_button_img.SetRender(false);
            sec_button_idle_img.SetRender(false);
        }

        //Get values from var and store them
        else
        {
            curr_hp = health.GetCurrentHealth();
            curr_stamina = stamina.GetCurrentStamina();

            //Set icon at the left
            if (left)
            {
                icon_trans.SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                icon_trans.SetUIPosition(new Vector3(-115.0f, 100.0f, 0.0f));
                sec_button_trans.SetUIPosition(new Vector3(124.0f, -33.0f, 0.0f));
                sec_button_idle_trans.SetUIPosition(new Vector3(124.0f, -33.0f, 0.0f));
            }

            //Set the icon at the right
            else
            {
                icon_trans.SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                icon_trans.SetUIPosition(new Vector3(115.0f, 100.0f, 0.0f));
                sec_button_trans.SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));
                sec_button_idle_trans.SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));
            }

            //Enable Secondary Button
            sec_button.SetInteractivity(true);
            sec_button_img.SetRender(true);
            sec_button_idle_img.SetRender(true);

            //Enable Secondary Bars & Update them
            icon_hp_img.FillAmount(curr_hp / max_hp);
            icon_stamina_img.FillAmount(curr_stamina / max_stamina);
            icon_hp_img.SetEnabled(true);
            icon_stamina_img.SetEnabled(true);

            //Disable Theon Abilities buttons
            EnableAbilities(false);
        }
    }

    public override bool IsAnimationStopped(string name)
    {
        return anim_controller.IsAnimationStopped(name);
    }

    public override bool IsAnimationRunning(string name)
    {
        return anim_controller.IsAnimationRunning(name);
    }

    public override void ToggleMesh(bool active)
    {
        left_arm.SetEnabled(active);
        Debug.Log("Left Arm");
        right_arm.SetEnabled(active);
        Debug.Log("Right Arm");
        CrossBow.GetComponent<CompMesh>().SetEnabled(active);
        Debug.Log("Crossbow");
        Arrow.GetComponent<CompMesh>().SetEnabled(active);
        Debug.Log("Arrow");
    }

    public bool OnLeftClick()
    {
        // Check if player is in Idle State
        if (state == State.IDLE) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(left_ability_cost))
            {
                //Check if the ability is not in cooldown
                if (!cd_left.in_cd)
                { 
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
                if (characters_manager.theon_tired == false)
                {
                    PlayFx("TheonTired");
                    characters_manager.theon_tired = true;
                }
                return false;
            }
        }
        return false;
    }

    public void PrepareLeftAbility()
    {
        left_button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoLeftAbility() //Might be virtual
    {
        // Decrease stamina -----------
        DecreaseStamina(left_ability_cost);

        GameObject arrow = Instantiate("ArrowTheon");

        arrow.transform.SetRotation(player.transform.GetRotation());
        arrow.transform.SetPosition(new Vector3(curr_position.x, curr_position.y - 1.5f, curr_position.z));
        Arrow arrow_script = arrow.GetComponent<Arrow>();
        arrow_script.speed = curr_forward;

        arrow_script.arrow_blood_particles = theon_blood_particles;
        arrow_script.arrow_sparks_particles = theon_sparks_particles;

        Arrow.GetComponent<CompMesh>().SetEnabled(false);

        //Set Attack Animation
        Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("T_Attack");
        SetAnimationTransition("ToAttack1", true);
        CrossBow.GetComponent<CompAnimation>().PlayAnimation("Attack");

        PlayFx("CrossbowShot");

        SetState(CharacterController.State.ATTACKING);
    }

    public bool OnRightClick()
    {     
        // Check if player is in Idle State
        if (state == State.IDLE)
        {
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(right_ability_cost))
            {
                //Check if the ability is not in cooldown
                if (!cd_right.in_cd)
                {
                    // Decrease stamina -----------
                    DecreaseStamina(right_ability_cost);

                    PlayFx("TheonMeleShout");
                    Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("T_Push");
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
                if (characters_manager.theon_tired == false)
                {
                    PlayFx("TheonTired");
                    characters_manager.theon_tired = true;
                }
                return false;
            }
        }
        return false;
    }

    public void PrepareRightAbility()
    {
        right_button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoRightAbility() //Might be virtual
    {
        GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 40.0f);

        Debug.Log("RIGHT ABILITY", Department.IA);

        if (coll_object != null)
        {
            PlayFx("TheonMeleHit");

            if (coll_object.GetTag() == "Enemy")
            {
                // Check the specific enemy in front of you and apply dmg or call object OnContact
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
                if (characters_manager.theon_tired == false)
                {
                    PlayFx("TheonTired");
                    characters_manager.theon_tired = true;
                }
                return false;
            }
        }
        return false;
    }

    public override void EnableAbilities(bool active)
    {
        base.EnableAbilities(active);

        //Flags
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
                Vector3 player_pos = curr_position;
                player_pos.y += 4;
                arrow.transform.SetPosition(GetSecondaryPosition(player_pos));
                arrow.transform.SetRotation(player.transform.GetRotation());

                Arrow arrow_script = arrow.GetComponent<Arrow>();
                arrow_script.speed = GetSecondaryForward(curr_forward);

                GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
                if (coll_object != null)
                {
                    coll_object.GetTag();
                    if (coll_object.CompareTag("Enemy"))
                    {
                        Vector3 enemypos = coll_object.transform.GetPosition();
                        enemypos.y += 7;
                        arrow_script.speed = GetSecondaryForwardToEnemy(arrow.transform.GetPosition(), enemypos);
                        arrow_script.speed *= 2;
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
                Vector3 player_pos = curr_position;
                player_pos.y += 4;
                arrow.transform.SetPosition(GetSecondaryPosition(player_pos));
                arrow.transform.SetRotation(player.transform.GetRotation());

                Arrow arrow_script = arrow.GetComponent<Arrow>();
                arrow_script.speed = GetSecondaryForward(curr_forward);

                GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
                if (coll_object != null)
                {
                    coll_object.GetTag();
                    if (coll_object.CompareTag("Enemy"))
                    {
                        Vector3 enemypos = coll_object.transform.GetPosition();
                        enemypos.y += 7;
                        arrow_script.speed = GetSecondaryForwardToEnemy(arrow.transform.GetPosition(), enemypos);
                        arrow_script.speed *= 2.5f;
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
                Vector3 player_pos = curr_position;
                player_pos.y += 4;
                arrow.transform.SetPosition(GetSecondaryPosition(player_pos));
                arrow.transform.SetRotation(player.transform.GetRotation());

                Arrow arrow_script = arrow.GetComponent<Arrow>();
                arrow_script.speed = GetSecondaryForward(curr_forward);

                GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
                if (coll_object != null)
                {
                    coll_object.GetTag();
                    if (coll_object.CompareTag("Enemy"))
                    {
                        Vector3 enemypos = coll_object.transform.GetPosition();
                        enemypos.y += 7;
                        arrow_script.speed = GetSecondaryForwardToEnemy(arrow.transform.GetPosition(), enemypos);
                        arrow_script.speed *= 1.5f;
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

    public void Heal(float percentage)
    {
        if (state != State.DEAD)
        {
            curr_hp += max_hp * percentage;
            if (curr_hp > max_hp)
            {
                curr_hp = max_hp;
            }
            icon_hp_img.FillAmount(curr_hp / max_hp);
        }
    }
}