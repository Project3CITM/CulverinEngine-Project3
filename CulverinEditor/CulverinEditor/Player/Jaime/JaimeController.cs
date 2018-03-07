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
    public GameObject jaime_button_left;
    public GameObject jaime_button_right;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;
    public float sec_ability_cd = 10.0f;
    private float sec_ability_current_cd = 10.0f;

    //Left Ability Stats ---
    public float left_ability_dmg = 10.0f;
    public float left_ability_cost = 10.0f;
    JaimeCD_Left left_ability_cd;
    // ---------------------

    //Right Ability Stats ---
    public float right_ability_dmg = 0.0f;
    public float right_ability_cost = 50.0f;
    JaimeCD_Right right_ability_cd;
    // ---------------------

    protected override void Start()
    {
        sec_ability_current_cd = sec_ability_cd;

        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        jaime_obj = GetLinkedObject("jaime_obj");
        larm_jaime_obj = GetLinkedObject("larm_jaime_obj");
        rarm_jaime_obj = GetLinkedObject("rarm_jaime_obj");

        jaime_icon_obj = GetLinkedObject("jaime_icon_obj");
        jaime_button_left = GetLinkedObject("jaime_button_left");
        jaime_button_right = GetLinkedObject("jaime_button_right");

        // Start Idle animation
        anim_controller = jaime_obj.GetComponent<CompAnimation>();
        anim_controller.PlayAnimation("Idle");
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
            //movement = GetLinkedObject("player_obj").GetComponent<MovementController>();
            //if (!movement.IsMoving())
            if(1 == 1)
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
                            anim_controller = jaime_obj.GetComponent<CompAnimation>();
                            if (anim_controller.IsAnimationStopped("Attack1"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Jaime Attacking");
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
                                Debug.Log("Jaime Covering");
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
                                Debug.Log("Jaime Blocking");
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
                                Debug.Log("Jaime Hit");
                            }
                            break;
                        }
                    case State.DEAD:
                        {
                            Debug.Log("We are going doown");
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
        //Left Attack
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            Debug.Log("Jaime Pressed 1");
            PrepareLeftAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Jaime Pressed 2");
            PrepareRightAbility();
        }
    }

    public override void SecondaryAbility()
    {
        Debug.Log("Jaime Secondary Ability");
    }

    public override void GetDamage(float dmg)
    {
        if (state == State.COVER)
        {
            SetAnimationTransition("ToBlock", true);

            jaime_obj.GetComponent<CompAudio>().PlayEvent("MetalHit");

            SetState(State.BLOCKING);
        }
        else
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.GetDamage(dmg);

            // SET HIT ANIMATION
            SetAnimationTransition("ToHit", true);

            SetState(State.HIT);
        }
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        Debug.Log("Jaime Transitioning between animations");
        anim_controller = jaime_obj.GetComponent<CompAnimation>();
        anim_controller.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active)
    {
        //Update Hp bar
        if (active)
        {
            Debug.Log("Update HP Jaime");
            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            Debug.Log("Update Stamina Jaime");
            //Update Stamina
            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            stamina.SetStamina(curr_stamina, max_stamina);
        }

        //Get values from var and store them
        else
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            curr_hp = health.GetCurrentHealth();

            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            curr_stamina = stamina.GetCurrentStamina();
        }

        Debug.Log("Update Child Jaime");

        //Change current character icon
        icon = jaime_icon_obj.GetComponent<CompImage>();
        icon.SetEnabled(active, jaime_icon_obj); 
    }

    public override void ToggleMesh(bool active)
    {
        larm_jaime_obj.GetComponent<CompMesh>().SetEnabled(active, larm_jaime_obj);
        rarm_jaime_obj.GetComponent<CompMesh>().SetEnabled(active, rarm_jaime_obj);
    }

    public override bool IsAnimationStopped(string name)
    {
        anim_controller = jaime_obj.GetComponent<CompAnimation>();
        return anim_controller.IsAnimationStopped(name);
    }

    public bool IsSecondaryAbilityReady()
    {
        if (sec_ability_current_cd <= 0.0f)
            return true;
        else
            return false;
    }

    public override float GetSecondaryAbilityCoolDown()
    {
        return sec_ability_cd;
    }

    public override void ResetCoolDown()
    {
        sec_ability_current_cd = sec_ability_cd;
    }

    public override void ReduceSecondaryAbilityCoolDown()
    {
        sec_ability_current_cd -= Time.DeltaTime();
        Debug.Log(sec_ability_current_cd);
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
                    Debug.Log("Jaime LW Going to Attack");
                    DoLeftAbility();              
                    return true;
                }
                else
                {
                    Debug.Log("Ability in CD");
                    return false;
                }
            }
            else
            {
                Debug.Log("Not Enough Stamina");
                return false;
            }
        }
        return false;
    }

    public void DoLeftAbility()
    {
        Debug.Log("Jaime LW Attack Left");

        // Decrease stamina -----------
        DecreaseStamina(left_ability_cost);

        Debug.Log("Jaime LW Going to hit");

        // Attack the enemy in front of you
        //if (GetLinkedObject("player_obj").GetComponent<MovementController>().EnemyInFront())
        //{
        //    // To change => check the specific enemy in front of you
        //    enemy = enemy_obj.GetComponent<EnemyController>();
        //    enemy.Hit(attack_dmg);
        //}

        // Set Attacking Animation
        SetAnimationTransition("ToAttack1", true);

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
                    Debug.Log("Jaime RW Going to Block");
                    DoRightAbility();
                    return true;
                }
                else
                {
                    Debug.Log("Jaime RW Ability in CD");
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
        Debug.Log("Jaime LW Attack Right");

        // Decrease stamina -----------
        DecreaseStamina(right_ability_cost);

        // Set Covering State
        SetState(CharacterController.State.COVER);
    }
}