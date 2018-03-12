using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeController : CharacterController
{
    //MESH ELEMENTS
    public GameObject jaime_obj;
    public GameObject larm_jaime_obj; //To enable/disable mesh
    public GameObject rarm_jaime_obj; //To enable/disable mesh

    //UI ELEMENTS
    public GameObject jaime_icon_obj;
    public GameObject jaime_icon_obj_hp;
    public GameObject jaime_icon_obj_stamina;
    public GameObject jaime_button_left;
    public GameObject jaime_button_right;
    public GameObject jaime_left_flag;
    public GameObject jaime_right_flag;


    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;

    //Secondary Ability Stats ---
    public float duration = 4.0f;
    public float sec_ability_damage = 10.0f;
    public float sec_ability_cost = 30;
    JaimeCD_Secondary sec_ability_cd;
    // ---------------------

    //Left Ability Stats ---
    public float left_ability_dmg = 10.0f;
    public float left_ability_cost = 10.0f;
    JaimeCD_Left left_ability_cd;

    /* To perform different animations depending on the hit streak */
    int hit_streak = 0; 
    string[] anim_name = { "Attack1", "Attack2", "Attack3" };
    string current_anim = "Attack1";
    // ---------------------

    //Right Ability Stats ---
    public float right_ability_dmg = 0.0f;
    public float right_ability_cost = 50.0f;
    JaimeCD_Right right_ability_cd;

    public GameObject jaime_sword_obj;

    public float left_ability_dmg2 = 1.0f;
    public float left_ability_dmg3 = 15.0f;
    public float reset_hit_time = 1.0f;
    public float curr_hit_time = 0.0f;
    // ---------------------

    protected override void Start()
    {
        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        jaime_obj = GetLinkedObject("jaime_obj");
        larm_jaime_obj = GetLinkedObject("larm_jaime_obj");
        rarm_jaime_obj = GetLinkedObject("rarm_jaime_obj");
        jaime_sword_obj = GetLinkedObject("jaime_sword_obj");

        jaime_icon_obj = GetLinkedObject("jaime_icon_obj");
        jaime_button_left = GetLinkedObject("jaime_button_left");
        jaime_button_right = GetLinkedObject("jaime_button_right");
        jaime_left_flag = GetLinkedObject("jaime_left_flag");
        jaime_right_flag = GetLinkedObject("jaime_right_flag");

        jaime_icon_obj_hp = GetLinkedObject("jaime_icon_obj_hp");
        jaime_icon_obj_stamina = GetLinkedObject("jaime_icon_obj_stamina");

        //Start Idle animation
        anim_controller = jaime_obj.GetComponent<CompAnimation>();
        anim_controller.PlayAnimation("Idle");

        //Disable Jaime secondary ability button
        GetLinkedObject("jaime_s_button_obj").SetActive(false);

        //Set Icon in the center
        jaime_icon_obj.GetComponent<CompImage>().SetEnabled(true, jaime_icon_obj);
        jaime_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(1.0f, 1.0f, 1.0f));
        jaime_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(0.0f, 365.0f, 0.0f));
        jaime_icon_obj_hp.GetComponent<CompImage>().SetEnabled(false, jaime_icon_obj_hp);
        jaime_icon_obj_stamina.GetComponent<CompImage>().SetEnabled(false, jaime_icon_obj_stamina);
    }

    public override void Update()
    {
        base.Update();
    }

    public override void ControlCharacter()
    {
        // Debug method to control Hp
        CheckHealth();

        // First check if you are alive
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
                            curr_hit_time += Time.deltaTime;
                            if (curr_hit_time > reset_hit_time) 
                            {
                                hit_streak = 0;
                            }

                            //Check For Input + It has to check if he's moving to block attack (¿?)
                            CheckAttack();
                            break;
                        }
                    case State.ATTACKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller = jaime_obj.GetComponent<CompAnimation>();
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
                    case State.COVER:
                        {
                            //Check for end of the Attack animation
                            anim_controller = jaime_obj.GetComponent<CompAnimation>();                     
                            if (anim_controller.IsAnimationStopped("Cover"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
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
        int enemy_x = 0;
        int enemy_y = 0;

        //Do Damage Around
        movement = GetLinkedObject("player_obj").GetComponent<MovementController>();
        movement.GetPlayerPos(out curr_x, out curr_y);

        //Check enemy in the tiles around the player
        for (int i = -1; i < 1; i++)
        {
            for (int j = -1; j < 1; j++)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                enemy_x = curr_x + j;
                enemy_y = curr_x + i;

                //Apply damage on the enemy in the specified tile
                GetLinkedObject("enemies_obj").GetComponent<EnemiesManager>().DamageEnemyInTile(enemy_x, enemy_y, sec_ability_damage);
            }
        }

        PlayFx("BehindMe");

        // Activate the shield that protects from damage once
        GetLinkedObject("player_obj").GetComponent<Shield>().ActivateShield();
    }

    public override void GetDamage(float dmg)
    {
        if (state == State.COVER)
        {
            SetAnimationTransition("ToBlock", true);

            GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("MetalHit");
            PlayFx("MetalClash");
            SetState(State.BLOCKING);
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
                audio = jaime_obj.GetComponent<CompAudio>();
                audio.PlayEvent("JaimeHurt");
            }

            else
            {
                SetState(State.DEAD);
                PlayFx("JaimeDead");
            }
            

            //Reset hit count
            hit_streak = 0;
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
            GetLinkedObject("jaime_s_button_obj").SetActive(false);
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
                GetLinkedObject("jaime_s_button_obj").SetActive(true);
                GetLinkedObject("jaime_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));
            }
            //Set the icon at the right
            else
            {
                jaime_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                jaime_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(115.0f, 430.0f, 0.0f));
                GetLinkedObject("jaime_s_button_obj").SetActive(true);
                GetLinkedObject("jaime_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(-123.0f, -31.5f, 0.0f));
            }

            //Enable Secondary Bars And Update them
            jaime_icon_obj_hp.GetComponent<CompImage>().FillAmount(curr_hp / max_hp);
            jaime_icon_obj_stamina.GetComponent<CompImage>().FillAmount(curr_stamina / max_stamina);
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
            if (GetCurrentStamina() > left_ability_cost)
            {
                left_ability_cd = jaime_button_left.GetComponent<JaimeCD_Left>();
                //Check if the ability is not in cooldown
                if (!left_ability_cd.in_cd)
                { 
                    DoLeftAbility(left_ability_cost);              
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

    public void DoLeftAbility(float cost)
    {
        // Decrease stamina -----------
        DecreaseStamina(cost);

        // Set Attacking Animation depending on the hit_streak
        current_anim = anim_name[hit_streak];
        SetAnimationTransition("To"+current_anim, true);

        // Attack the enemy in front of you
        GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 40.0f);
        Debug.Log("Raycast");
        if (coll_object != null)
        {
            Debug.Log("Collided object found");
            Debug.Log(coll_object.GetTag());
            if (coll_object.CompareTag("Enemy"))
            {
                curr_hit_time = 0.0f;
                Debug.Log("Enemies manager 1");
                // Check the specific enemy in front of you and apply dmg or call object OnContact
                EnemiesManager enemy_manager = GetLinkedObject("enemies_obj").GetComponent<EnemiesManager>();
                Debug.Log("Enemies manager 2");

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

                //enemy_manager.ApplyDamage(coll_object, damage);
                Debug.Log("Apply Damage");

                if (hit_streak < 2)
                {
                    hit_streak++; //Increase hit count
                }
                else
                {
                    hit_streak = 0; //Reset hit count
                }
            }
            else
            {
                // Call Collider OnContact to notify raycast
                CompCollider obj_collider = coll_object.GetComponent<CompCollider>();
                if (obj_collider != null)
                {
                    obj_collider.CallOnContact();
                }
                else
                {
                    hit_streak = 0; //Reset hit count
                }
            }
        }
        else
        {
            hit_streak = 0; //Reset hit count
        }

        // Play the Sound FX
        PlayFx("SwordSlash");

        SetState(CharacterController.State.ATTACKING);
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
            if (GetCurrentStamina() > right_ability_cost)
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
            {
                Debug.Log("Jaime RW Not Enough Stamina");
                return false;
            }
        }
        return false;
    }

    public void DoRightAbility()
    {
        //Decrease stamina -----------
        DecreaseStamina(right_ability_cost);

        //Set Animation
        SetAnimationTransition("ToCover", true);

        // Set Covering State
        SetState(CharacterController.State.COVER);
    }

    public bool OnSecondaryClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (GetCurrentStamina() > sec_ability_cost)
            {
                sec_ability_cd = GetLinkedObject("jaime_s_button_obj").GetComponent<JaimeCD_Secondary>();
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

    public override void EnableAbilities(bool active)
    {
        jaime_button_left.SetActive(active);
        jaime_button_right.SetActive(active);
        jaime_left_flag.SetActive(active);
        jaime_right_flag.SetActive(active);
    }
}