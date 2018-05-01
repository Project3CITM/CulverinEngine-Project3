using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeController : CharacterController
{
    //MESH ELEMENTS
    public GameObject jaime_obj;
    public GameObject larm_jaime_obj; 
    public GameObject rarm_jaime_obj;
    public GameObject jaime_sword_obj; //PARTICLES
    SwordParticles sword_particles;

    //UI ELEMENTS
    public GameObject jaime_icon_obj;
    public GameObject jaime_icon_obj_hp;
    public GameObject jaime_icon_obj_stamina;

    public GameObject jaime_button_left;
    public GameObject jaime_button_left_idle;

    public GameObject jaime_button_right;
    public GameObject jaime_button_right_idle;
    public GameObject jaime_right_text_counter;

    public GameObject combo_obj; /* Combo controller */
    ComboController combo_controller;

    GameObject jaime_s_button;
    GameObject jaime_s_button_idle;
    public GameObject jaime_sec_text_counter;

    public GameObject jaime_left_flag;
    public GameObject jaime_right_flag;
    
    //CAMERA
    public GameObject Global_Camera;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;
    private float stamina_regen = 0.0f;

    //Sec Ability Stats
    public float duration = 4.0f;
    public float sec_ability_cost = 30.0f;
    public float sec_ability_cd_time = 20.0f;
    private JaimeCD_Secondary sec_ability_cd;

    //Left Ability Stats 
    public float left_ability_dmg = 10.0f;
    public float left_ability_dmg2 = 1.0f;
    public float left_ability_dmg3 = 15.0f;
    public float left_ability_cost = 10.0f;
    public float left_ability_cd_time = 0.7f;
    private JaimeCD_Left cd_left;
    private bool do_left_attack = false;

    public float blood_amount = 0.2f;

    /* To perform different animations depending on the hit streak */
    string[] anim_name = { "Attack1", "Attack2", "Attack3" };
    string current_anim = "Attack1";

    //Right Ability Stats 
    public float right_ability_cost = 50.0f;
    public float right_ability_cd_time = 10.0f;
    private JaimeCD_Right cd_right;

    public float cover_duration = 3.0f;
    private float cover_timer = 0.0f;

    protected override void Start()
    {
        base.Start();

        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        jaime_obj = GetLinkedObject("jaime_obj");
        larm_jaime_obj = GetLinkedObject("larm_jaime_obj");
        rarm_jaime_obj = GetLinkedObject("rarm_jaime_obj");
        jaime_sword_obj = GetLinkedObject("jaime_sword_obj");
        sword_particles = jaime_sword_obj.GetComponent<SwordParticles>();

        jaime_icon_obj = GetLinkedObject("jaime_icon_obj");

        jaime_button_left = GetLinkedObject("jaime_button_left");
        jaime_button_left_idle = GetLinkedObject("jaime_button_left_idle");
        combo_obj = GetLinkedObject("combo_obj");
        combo_controller = combo_obj.GetComponent<ComboController>();
        cd_left = jaime_button_left.GetComponent<JaimeCD_Left>();

        jaime_button_right = GetLinkedObject("jaime_button_right");
        jaime_button_right_idle = GetLinkedObject("jaime_button_right_idle");
        jaime_right_text_counter = GetLinkedObject("jaime_right_text_counter");
        cd_right = jaime_button_right.GetComponent<JaimeCD_Right>();

        jaime_s_button = GetLinkedObject("jaime_s_button_obj");
        jaime_s_button_idle = GetLinkedObject("jaime_s_button_obj_idle");
        jaime_sec_text_counter = GetLinkedObject("jaime_sec_text_counter");
        sec_ability_cd = jaime_s_button.GetComponent<JaimeCD_Secondary>();

        jaime_left_flag = GetLinkedObject("jaime_left_flag");
        jaime_right_flag = GetLinkedObject("jaime_right_flag");

        jaime_icon_obj_hp = GetLinkedObject("jaime_icon_obj_hp");
        jaime_icon_obj_stamina = GetLinkedObject("jaime_icon_obj_stamina");
        stamina_regen = stamina.regen;

        Global_Camera = GetLinkedObject("Global_Camera");

        //Link components with correct gameobjects
        LinkComponents(jaime_icon_obj, jaime_icon_obj_hp, jaime_icon_obj_stamina, null,
                       jaime_button_left, jaime_button_right, jaime_s_button, jaime_s_button_idle,
                       null, jaime_right_text_counter, jaime_sec_text_counter,
                       larm_jaime_obj, rarm_jaime_obj, jaime_button_left_idle, jaime_button_right_idle);


        //Start Idle animation
        anim_controller.PlayAnimation("Idle");

        //Disable Jaime secondary ability button
        sec_button.SetInteractivity(false);
        sec_button_img.SetRender(false);
        sec_button_idle_img.SetRender(false);

        //Set Icon in the center
        icon_img.SetEnabled(true);
        icon_img.SetColor(new Vector3(1.0f, 1.0f, 1.0f), 1.0f);

        icon_trans.SetScale(new Vector3(1.0f, 1.0f, 1.0f));
        icon_trans.SetUIPosition(new Vector3(0.0f, 22.0f, 0.0f));

        icon_hp_img.SetEnabled(false);
        icon_stamina_img.SetEnabled(false);

        cover_timer = 0.0f;
    }

    public override void Update()
    {
        base.Update();
    }

    public override void ControlCharacter()
    {
        //Debug.Log(state, Department.PLAYER);
        curr_hp = health.GetCurrentHealth();
        
        // First check if you are alive
        if (curr_hp > 0)
        {
            //Check play breath audio
            base.CheckHealth(curr_hp, max_hp, "JaimeBreathing");

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
                            break;
                        }
                    case State.ATTACKING:
                        {
                            //Check for end of the Attack animation                       
                            //Apply damage over x time of the attack animation
                            if (do_left_attack && anim_controller.IsAnimOverXTime(0.7f))
                            {
                                DoLeftAbility();
                                do_left_attack = false;
                            }

                            if (anim_controller.IsAnimationStopped(current_anim))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                            }
                            break;
                        }
                    case State.FAIL_ATTACK:
                        {
                            //Check for end of the Attack animation
                            if (anim_controller.IsAnimationStopped("Attack Fail"))
                            {
                                state = State.IDLE;
                            }
                            break;
                        }
                    case State.COVER:
                        {
                            //Check for end of the Attack animation
                            if (anim_controller.IsAnimationStopped("CoverIn") && cover_timer <= 0.0f)
                            {
                                anim_controller.SetTransition("ToCoverIdle");
                            }
                            if (anim_controller.IsAnimationRunning("CoverIdle") == true)
                            {
                                cover_timer += Time.deltaTime;
                            }
                            if(anim_controller.IsAnimationRunning("CoverIdle") && cover_timer >= cover_duration)
                            {
                                anim_controller.SetTransition("ToCoverOut");
                            }
                            if (anim_controller.IsAnimationStopped("CoverOut") && cover_timer >= cover_duration)
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
        int curr_x = 0;
        int curr_y = 0;
        //int enemy_x = 0;
        //int enemy_y = 0;

        //Do Damage Around
        movement.GetPlayerPos(out curr_x, out curr_y);

        // Decrease stamina -----------
        DecreaseStamina(sec_ability_cost);
        DecreaseMana(sec_ability_cost);

        PlayFx("JaimeWarCry");

        // Activate the shield that protects from damage once
        player.GetComponent<Shield>().ActivateShield();
    }

    public override bool GetDamage(float dmg)
    {
        if (state == State.COVER)
        {
            SetAnimationTransition("ToBlock", true);
            Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("J_Block");

            PlayFx("MetalHit");       
            //PlayFx("MetalClash");
            PlayFx("JaimeBlock");

            DecreaseStamina(right_ability_cost);

            SetState(State.BLOCKING);

            return false;
        }
        else
        {
            health.GetDamage(dmg);

            if (health.GetCurrentHealth() > 0)
            {
                if (GetState() == 0)
                {
                    Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("Hit");
                    SetAnimationTransition("ToHit", true);
                    SetState(State.HIT);
                }

                PlayFx("JaimeHurt");
                play_breathing_audio = true;

                //Damage Feedback
                damage_feedback.SetDamage(health.GetCurrentHealth(), max_hp);
            }

            else
            {
                Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("J_Death");
                SetAnimationTransition("ToDeath", true);
                SetState(State.DEAD);

                PlayFx("JaimeDead");
            }

            //Reset hit count
            combo_controller.ResetHitStreak();

            return true;
        }
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

            //Enable Jaime Abilities buttons
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

            //Enable Secondary Bars And Update them
            icon_hp_img.FillAmount(curr_hp / max_hp);
            icon_stamina_img.FillAmount(curr_stamina / max_stamina);
            icon_hp_img.SetEnabled(true);
            icon_stamina_img.SetEnabled(true);

            //Disable Jaime Abilities buttons
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
        right_arm.SetEnabled(active);
        jaime_sword_obj.GetComponent<CompMesh>().SetEnabled(active);
    }

    public void PrepareLeftAbility()
    {
        left_button.Clicked(); // This will execute LeftCooldown  
    }

    public bool OnLeftClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(left_ability_cost))
            {
                //Check if the ability is not in cooldown
                if (!cd_left.in_cd)
                {
                    // Play the Sound FX
                    PlayFx("JaimeFailImpact"); //GOOD ONE

                    // Decrease stamina -----------
                    DecreaseStamina(left_ability_cost);

                    // Set Attacking Animation depending on the hit_streak
                    current_anim = anim_name[combo_controller.GetHitStreak()];
                    SetAnimationTransition("To" + current_anim, true);
                    if(current_anim == "Attack1")
                    {
                        anim_controller.PlayAnimationNode("J_Attack1");
                    }
                    if(current_anim == "Attack2")
                    {
                        anim_controller.PlayAnimationNode("J_Attack2");
                    }
                    if(current_anim == "Attack3")
                    {
                        anim_controller.PlayAnimationNode("J_Attack3");
                    }
                    do_left_attack = true;

                    //Go to ATTACK State
                    SetState(State.ATTACKING);
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (characters_manager.jaime_tired == false)
                {
                    PlayFx("JaimeTired");
                    characters_manager.jaime_tired = true;
                }
                return false;
            }
        }
        return false;
    }

    public void DoLeftAbility()
    {
        // Attack the enemy in front of you
        GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 30.0f);
        if (coll_object != null)
        {
            coll_object.GetTag();
            if (coll_object.CompareTag("Enemy"))
            {
                //Get current hit streak
                int hit_streak = combo_controller.GetHitStreak();

                //Enable particles emission of enemy blood
                sword_particles.EnableEnemyCollision(true);

                // Check the specific enemy in front of you and apply dmg or call object OnContact
                float damage = 1.0f;

                if (hit_streak == 0) 
                {
                    damage = left_ability_dmg;
                }
                else if (hit_streak == 1)
                {
                    damage = left_ability_dmg2;
                }
                else if (hit_streak == 2)
                {
                    damage = left_ability_dmg3;
                }

                /* ---------- IN CASE THAT THE ENEMY BLOCKS THE ATTACK, UNCOMMENT AND COMPLETE THIS CODE ---------- */
                if (enemy_manager.ApplyDamage(coll_object, damage, Enemy_BT.ENEMY_GET_DAMAGE_TYPE.DEFAULT))
                {
                    //Increase the blood of the sword
                    sword_particles.SetBlood(blood_amount); 

                    PlayFx("Enemy_Flesh_Hit");

                    if (hit_streak == 0)
                    {
                        //Start combo time controller to manage hit streaks
                        combo_controller.StartComboTime();
                    }

                    // If damage done effectively, increase Hit Streak
                    combo_controller.IncreaseHitStreak();
                }
                else
                {
                    //Decrease the blood of the sword
                    sword_particles.SetBlood(-blood_amount);

                    // Reset Hit Count
                    combo_controller.ResetHitStreak();

                    //Enable particles emission of enemy blood
                    sword_particles.EnableWallCollision(true);

                    //Set FailAttack Transition & Audio
                    SetAnimationTransition("ToFail", true);

                    PlayFx("JaimeImpact");

                    SetState(State.FAIL_ATTACK);
                }
                /* ----------------------------------------------------------------------------------- */
            }
            else if(coll_object.CompareTag("obstacle") || coll_object.CompareTag("trap_floor"))
            {
                //Decrease the blood of the sword
                sword_particles.SetBlood(-blood_amount);

                //Reset Hit Count
                combo_controller.ResetHitStreak();

                //Enable particles emission of sparks
                sword_particles.EnableWallCollision(true);

                //Set FailAttack Transition & Audio
                SetAnimationTransition("ToFail", true);

                PlayFx("JaimeImpact");

                SetState(State.FAIL_ATTACK);
            }
        }
        else
        {
            //Decrease the blood of the sword
            sword_particles.SetBlood(-blood_amount);

            //Reset Hit Count
            combo_controller.ResetHitStreak();
        }
    }

    public void PrepareRightAbility()
    {
        right_button.Clicked(); // This will execute RightCooldown    
    }

    public bool OnRightClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0)
        {
            Debug.Log("Get State = 0", Department.PLAYER, Color.YELLOW);
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(right_ability_cost))
            {
                Debug.Log("Can Waste Stamina", Department.PLAYER, Color.YELLOW);
                //Check if the ability is not in cooldown
                if (!cd_right.in_cd)
                {
                    DoRightAbility();
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (characters_manager.jaime_tired == false)
                {
                    PlayFx("JaimeTired");
                    characters_manager.jaime_tired = true;
                }
                return false;
            }
        }
        return false;
    }

    public void DoRightAbility()
    {
        //Set Animation
        SetAnimationTransition("ToCoverIn", true);
        cover_timer = 0.0f;

        // Set Covering State
        SetState(CharacterController.State.COVER);
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
                    SecondaryAbility();
                    return true;
                }
                else
                    return false;
            }
            else
            {
                if (characters_manager.jaime_tired == false)
                {
                    PlayFx("JaimeTired");
                    characters_manager.jaime_tired = true;
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
        jaime_left_flag.SetActive(active);
        jaime_right_flag.SetActive(active);
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