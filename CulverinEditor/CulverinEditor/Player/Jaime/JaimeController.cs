using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeController : CharacterController
{
    //MESH ELEMENTS
    public GameObject jaime_obj;
    public GameObject larm_jaime_obj; //To enable/disable mesh
    public GameObject rarm_jaime_obj; //To enable/disable mesh
    public GameObject Global_Camera;
    
    //UI ELEMENTS
    public GameObject jaime_icon_obj;
    public GameObject jaime_icon_obj_hp;
    public GameObject jaime_icon_obj_stamina;
    public GameObject jaime_button_left;
    public GameObject jaime_button_right;
    public GameObject jaime_left_flag;
    public GameObject jaime_right_flag;

    CompAnimation jaime_anim_controller;
    CompImage jaime_icon_stamina_bar;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;
    private float stamina_regen = 0.0f;

    //Secondary Ability Stats ---
    public float duration = 4.0f;
    public float sec_ability_damage = 10.0f;
    public float sec_ability_cost = 30;
    JaimeCD_Secondary sec_ability_cd;
    // ---------------------

    //Left Ability Stats ---
    public float left_ability_dmg = 10.0f;
    public float left_ability_dmg2 = 1.0f;
    public float left_ability_dmg3 = 15.0f;
    public float left_ability_cost = 10.0f;
    JaimeCD_Left left_ability_cd;
    bool do_left_attack = false;
    public GameObject combo_obj; // Combo controller

    /* To perform different animations depending on the hit streak */
    string[] anim_name = { "Attack1", "Attack2", "Attack3" };
    string current_anim = "Attack1";
    // ---------------------

    //Right Ability Stats ---
    public float right_ability_dmg = 0.0f;
    public float right_ability_cost = 50.0f;
    JaimeCD_Right right_ability_cd;

    public GameObject jaime_sword_obj;
    // ---------------------

    //Particle emitter GameObject
    public GameObject particles_jaime;

    public GameObject jaime_button_left_idle;
    public GameObject jaime_button_right_idle;

    public float cover_duration = 3.0f;
    private float cover_timer = 0.0f;

    protected override void Start()
    {
        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        jaime_obj = GetLinkedObject("jaime_obj");
        larm_jaime_obj = GetLinkedObject("larm_jaime_obj");
        rarm_jaime_obj = GetLinkedObject("rarm_jaime_obj");
        jaime_sword_obj = GetLinkedObject("jaime_sword_obj");

        Global_Camera = GetLinkedObject("Global_Camera");

        jaime_icon_obj = GetLinkedObject("jaime_icon_obj");
        jaime_button_left = GetLinkedObject("jaime_button_left");
        jaime_button_right = GetLinkedObject("jaime_button_right");
        jaime_button_left_idle = GetLinkedObject("jaime_button_left_idle");
        jaime_button_right_idle = GetLinkedObject("jaime_button_right_idle");

        jaime_left_flag = GetLinkedObject("jaime_left_flag");
        jaime_right_flag = GetLinkedObject("jaime_right_flag");

        jaime_icon_obj_hp = GetLinkedObject("jaime_icon_obj_hp");
        jaime_icon_obj_stamina = GetLinkedObject("jaime_icon_obj_stamina");
        stamina_regen = GetLinkedObject("stamina_obj").GetComponent<Stamina>().regen;

        particles_jaime = GetLinkedObject("particles_jaime");

        combo_obj = GetLinkedObject("combo_obj");

        //Start Idle animation
        anim_controller = jaime_obj.GetComponent<CompAnimation>();
        anim_controller.PlayAnimation("Idle");

        //Disable Jaime secondary ability button
        GetLinkedObject("jaime_s_button_obj").GetComponent<CompButton>().SetInteractivity(false);
        GetLinkedObject("jaime_s_button_obj").GetComponent<CompImage>().SetRender(false);
        GetLinkedObject("jaime_s_button_obj_idle").GetComponent<CompImage>().SetRender(false);

        //Set Icon in the center
        jaime_icon_obj.GetComponent<CompImage>().SetEnabled(true, jaime_icon_obj);
        jaime_icon_obj.GetComponent<CompImage>().SetColor(new Vector3(1.0f, 1.0f, 1.0f), 1.0f);

        jaime_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(1.0f, 1.0f, 1.0f));
        jaime_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(0.0f, 365.0f, 0.0f));
        jaime_icon_obj_hp.GetComponent<CompImage>().SetEnabled(false, jaime_icon_obj_hp);
        jaime_icon_obj_stamina.GetComponent<CompImage>().SetEnabled(false, jaime_icon_obj_stamina);

        cover_timer = 0.0f;
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
                Debug.Log("JAIME STATE:" + state, Department.PLAYER);
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
                            anim_controller = jaime_obj.GetComponent<CompAnimation>();

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
                            anim_controller = jaime_obj.GetComponent<CompAnimation>();

                            if (anim_controller.IsAnimationStopped("Attack Fail"))
                            {
                                state = State.IDLE;
                            }
                            break;
                        }
                    case State.COVER:
                        {
                            //Check for end of the Attack animation
                            if (jaime_obj.GetComponent<CompAnimation>().IsAnimationStopped("CoverIn") && cover_timer <= 0.0f)
                            {
                                jaime_obj.GetComponent<CompAnimation>().SetTransition("ToCoverIdle");
                            }
                            if(jaime_obj.GetComponent<CompAnimation>().IsAnimationRunning("CoverIdle") == true)
                            {
                                cover_timer += Time.deltaTime;
                            }
                            if(jaime_obj.GetComponent<CompAnimation>().IsAnimationRunning("CoverIdle") && cover_timer >= cover_duration)
                            {
                                jaime_obj.GetComponent<CompAnimation>().SetTransition("ToCoverOut");
                            }
                            if (jaime_obj.GetComponent<CompAnimation>().IsAnimationStopped("CoverOut") && cover_timer >= cover_duration)
                            {
                                state = State.IDLE;
                            }
                            break;
                        }
                    case State.BLOCKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller = jaime_obj.GetComponent<CompAnimation>();
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
                            anim_controller = jaime_obj.GetComponent<CompAnimation>();
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
                jaime_icon_stamina_bar = jaime_icon_obj_stamina.GetComponent<CompImage>();
                jaime_icon_stamina_bar.FillAmount(calc_stamina);
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
        movement = GetLinkedObject("player_obj").GetComponent<MovementController>();
        movement.GetPlayerPos(out curr_x, out curr_y);

        //Check enemy in the tiles around the player
        //for (int i = -1; i < 1; i++)
        //{
        //    for (int j = -1; j < 1; j++)
        //    {
        //        if (i == 0 && j == 0)
        //        {
        //            continue;
        //        }
        //        enemy_x = curr_x + j;
        //        enemy_y = curr_x + i;

        //        //Apply damage on the enemy in the specified tile (SURE ?¿)
        //        //GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>().DamageEnemyInTile(enemy_x, enemy_y, sec_ability_damage);
        //    }
        //}

        // Decrease stamina -----------
        DecreaseStamina(sec_ability_cost);
        DecreaseMana(sec_ability_cost);

        PlayFx("JaimeWarCry");

        // Activate the shield that protects from damage once
        GetLinkedObject("player_obj").GetComponent<Shield>().ActivateShield();
    }

    public override bool GetDamage(float dmg)
    {
        if (state == State.COVER)
        {
            SetAnimationTransition("ToBlock", true);
            Global_Camera.GetComponent<CompAnimation>().PlayAnimation("J_Block");
            GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("MetalHit");
            
            //PlayFx("MetalClash");
            PlayFx("JaimeBlock");

            DecreaseStamina(right_ability_cost);

            SetState(State.BLOCKING);

            return false;
        }
        else
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.GetDamage(dmg);

            if (health.GetCurrentHealth() > 0)
            {
                if (GetState() == 0)
                {
                    SetAnimationTransition("ToHit", true);
                    SetState(State.HIT);
                }
                PlayFx("JaimeHurt");
            }

            else
            {
                SetAnimationTransition("ToDeath", true);
                SetState(State.DEAD);
            }

            //Reset hit count
            combo_obj.GetComponent<ComboController>().ResetHitStreak();

            return true;
        }
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller = jaime_obj.GetComponent<CompAnimation>();
        anim_controller.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active, bool left)
    {
        //Update Hp bar
        if (active)
        {
            //Set Icon in the center
            jaime_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(1.0f, 1.0f, 1.0f));
            jaime_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(0.0f, 365.0f, 0.0f));
            jaime_icon_obj_hp.GetComponent<CompImage>().SetEnabled(false, jaime_icon_obj_hp);
            jaime_icon_obj_stamina.GetComponent<CompImage>().SetEnabled(false, jaime_icon_obj_stamina);

            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            //Update Stamina
            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            stamina.SetStamina(curr_stamina, max_stamina);

            //Enable Jaime Abilities buttons
            EnableAbilities(true);

            //Disable Secondary button     
            GetLinkedObject("jaime_s_button_obj").GetComponent<CompButton>().SetInteractivity(false);
            GetLinkedObject("jaime_s_button_obj").GetComponent<CompImage>().SetRender(false);
            GetLinkedObject("jaime_s_button_obj_idle").GetComponent<CompImage>().SetRender(false);
            GetLinkedObject("jaime_s_button_obj_idle").GetComponent<CompImage>().SetRender(false);        
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
                jaime_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                jaime_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(-115.0f, 430.0f, 0.0f));
                GetLinkedObject("jaime_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));           
                GetLinkedObject("jaime_s_button_obj_idle").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));
            }
            //Set the icon at the right
            else
            {
                jaime_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                jaime_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(115.0f, 430.0f, 0.0f));
                GetLinkedObject("jaime_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(-123.0f, -31.5f, 0.0f));
                GetLinkedObject("jaime_s_button_obj_idle").GetComponent<CompRectTransform>().SetPosition(new Vector3(-123.0f, -31.5f, 0.0f));
            }

            //Enable Secondary Button
            GetLinkedObject("jaime_s_button_obj").GetComponent<CompButton>().SetInteractivity(true);
            GetLinkedObject("jaime_s_button_obj").GetComponent<CompImage>().SetRender(true);
            GetLinkedObject("jaime_s_button_obj_idle").GetComponent<CompImage>().SetRender(true);

            //Enable Secondary Bars And Update them
            jaime_icon_obj_hp.GetComponent<CompImage>().FillAmount(curr_hp / max_hp);
            jaime_icon_obj_stamina.GetComponent<CompImage>().FillAmount(curr_stamina / max_stamina);
            Debug.Log(curr_stamina / max_stamina);
            jaime_icon_obj_hp.GetComponent<CompImage>().SetEnabled(true, jaime_icon_obj_hp);
            jaime_icon_obj_stamina.GetComponent<CompImage>().SetEnabled(true, jaime_icon_obj_stamina);

            //Disable Jaime Abilities buttons
            EnableAbilities(false);
        }
    }

    public override void ToggleMesh(bool active)
    {
        larm_jaime_obj.GetComponent<CompMesh>().SetEnabled(active, larm_jaime_obj);
        rarm_jaime_obj.GetComponent<CompMesh>().SetEnabled(active, rarm_jaime_obj);
        jaime_sword_obj.GetComponent<CompMesh>().SetEnabled(active, jaime_sword_obj);
    }

    public override bool IsAnimationStopped(string name)
    {
        anim_controller = jaime_obj.GetComponent<CompAnimation>();
        return anim_controller.IsAnimationStopped(name);
    }

    public override bool IsAnimationRunning(string name)
    {
        anim_controller = jaime_obj.GetComponent<CompAnimation>();
        return anim_controller.IsAnimationRunning(name);
    }

    public void PrepareLeftAbility()
    {
        button = jaime_button_left.GetComponent<CompButton>();
        button.Clicked(); // This will execute LeftCooldown  
    }

    public bool OnLeftClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(left_ability_cost))
            {
                left_ability_cd = jaime_button_left.GetComponent<JaimeCD_Left>();
                //Check if the ability is not in cooldown
                if (!left_ability_cd.in_cd)
                {
                    // Play the Sound FX
                    PlayFx("JaimeFailImpact"); //GOOD ONE

                    // Decrease stamina -----------
                    DecreaseStamina(left_ability_cost);

                    // Set Attacking Animation depending on the hit_streak
                    current_anim = anim_name[combo_obj.GetComponent<ComboController>().GetHitStreak()];
                    SetAnimationTransition("To" + current_anim, true);
                    jaime_anim_controller = Global_Camera.GetComponent<CompAnimation>();
                    if(current_anim == "Attack1")
                    {
                        jaime_anim_controller.PlayAnimation("J_Attack1");
                    }
                    if(current_anim == "Attack2")
                    {
                        jaime_anim_controller.PlayAnimation("J_Attack2");
                    }
                    if(current_anim == "Attack3")
                    {
                        jaime_anim_controller.PlayAnimation("J_Attack3");
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
                int hit_streak = combo_obj.GetComponent<ComboController>().GetHitStreak();

                //Enable particles emission of enemy blood
                particles_jaime.GetComponent<SwordParticles>().EnableEnemyCollision(true);

                // Check the specific enemy in front of you and apply dmg or call object OnContact
                EnemiesManager enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();

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
                if (enemy_manager.ApplyDamage(coll_object, damage))
                {
                    GetComponent<CompAudio>().PlayEvent("Enemy_Flesh_Hit");

                    if (hit_streak == 0)
                    {
                        //Start combo time controller to manage hit streaks
                        combo_obj.GetComponent<ComboController>().StartComboTime();
                    }

                    // If damage done effectively, increase Hit Streak
                    combo_obj.GetComponent<ComboController>().IncreaseHitStreak();
                }
                else
                {
                    // Reset Hit Count
                    combo_obj.GetComponent<ComboController>().ResetHitStreak(); 

                    //Enable particles emission of enemy blood
                    particles_jaime.GetComponent<SwordParticles>().EnableWallCollision(true);

                    //Set FailAttack Transition & Audio
                    SetAnimationTransition("ToFail", true);

                    //PlayFx -> Obstacle Impact
                    //PlayFx("JaimeImpactStone");
                    PlayFx("JaimeImpact");

                    SetState(State.FAIL_ATTACK);
                }
                /* ----------------------------------------------------------------------------------- */
            }
            else if(coll_object.CompareTag("obstacle"))
            {
                //Reset Hit Count
                combo_obj.GetComponent<ComboController>().ResetHitStreak(); 

                //Enable particles emission of sparks
                particles_jaime.GetComponent<SwordParticles>().EnableWallCollision(true);

                //Set FailAttack Transition & Audio
                SetAnimationTransition("ToFail", true);

                //PlayFx -> Obstacle Impact
                //PlayFx("JaimeImpactStone");
                PlayFx("JaimeImpact");

                SetState(State.FAIL_ATTACK);
            }
        }
        else
        {
            //Reset Hit Count
            combo_obj.GetComponent<ComboController>().ResetHitStreak();
        }

        // Play the Sound FX
        //PlayFx("JaimeFailImpact");
    }

    public void PrepareRightAbility()
    {
        button = jaime_button_right.GetComponent<CompButton>();
        button.Clicked(); // This will execute RightCooldown    
    }

    public bool OnRightClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(right_ability_cost))
            {
                right_ability_cd = jaime_button_right.GetComponent<JaimeCD_Right>();
                //Check if the ability is not in cooldown
                if (!right_ability_cd.in_cd)
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
                return false;
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
                sec_ability_cd = GetLinkedObject("jaime_s_button_obj").GetComponent<JaimeCD_Secondary>();
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
                return false;
        }
        return false;
    }

    public override void EnableAbilities(bool active)
    {
        //jaime_button_left.SetActive(active);
        //jaime_button_right.SetActive(active);

        //Button Interaction
        jaime_button_left.GetComponent<CompButton>().SetInteractivity(active);
        jaime_button_right.GetComponent<CompButton>().SetInteractivity(active);

        //Image
        jaime_button_left.GetComponent<CompImage>().SetRender(active);
        jaime_button_right.GetComponent<CompImage>().SetRender(active);
        jaime_button_left_idle.GetComponent<CompImage>().SetRender(active);
        jaime_button_right_idle.GetComponent<CompImage>().SetRender(active);
        
        //Right Cooldown Text Render
        GetLinkedObject("jaime_right_cd_text").GetComponent<CompText>().SetRender(active);

        //Sec Cooldown Text Render
        GetLinkedObject("jaime_secondary_cd_text").GetComponent<CompText>().SetRender(!active);

        //Disable Flags
        jaime_left_flag.SetActive(active);
        jaime_right_flag.SetActive(active);
    }

    public void Heal()
    {
        curr_hp = max_hp;
    }
}