using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysController : CharacterController
{
    public GameObject daenerys_obj; //Maybe this should be in charactercontroller since we're only having 1 camera which will be the movementcontroller/charactermanager?

    public GameObject daenerys_icon_obj;

    public GameObject daenerys_button_left_obj;
    public GameObject daenerys_button_right_obj;

    public GameObject rarm_daenerys_obj;
    public GameObject larm_daenerys_obj;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_mana = 100.0f;
    public float curr_mana = 100.0f;
    public float sec_ability_cd = 10.0f;
    private float sec_ability_current_cd = 10.0f;

    //Left Ability Stats
    public float mana_cost_percentage_left = 20f;
    public float damage_percentage_left = 10f;
    private DaenerysCD_Left cd_left;

    //Right Ability Stats
    public float mana_cost_percentage = 20f;
    public float damage_percentage = 10f;
    private DaenerysCD_Right cd_right;

    protected override void Start()
    {
        SetPosition(Position.BEHIND);
        sec_ability_current_cd = sec_ability_cd;
        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
       daenerys_obj = GetLinkedObject("daenerys_obj");
       daenerys_icon_obj = GetLinkedObject("daenerys_icon_obj");
       daenerys_button_left_obj = GetLinkedObject("daenerys_button_left_obj");
       daenerys_button_right_obj = GetLinkedObject("daenerys_button_right_obj");
       rarm_daenerys_obj = GetLinkedObject("rarm_daenerys_obj");
       larm_daenerys_obj = GetLinkedObject("larm_daenerys_obj");

        //Disable icon
        icon = daenerys_icon_obj.GetComponent<CompImage>();
       icon.SetEnabled(false, daenerys_icon_obj);

       //Disable Mana bar
       icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
       icon.SetEnabled(false, GetLinkedObject("mana_obj"));

       Debug.Log(gameObject.GetName());

       // Start Idle animation
       anim_controller = daenerys_obj.GetComponent<CompAnimation>();
       anim_controller.PlayAnimation("Idle");

       ToggleMesh(false);
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
            //if (!movement.IsMoving())
            if(1 == 1)
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
                            if (anim_controller.IsAnimationStopped("Attack1"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation until it ends
                                Debug.Log("Daenerys Attacking");
                            }
                            break;
                        }
                    case State.COVER:
                        {
                            //Check for end of the Attack animation
                            anim_controller = daenerys_obj.GetComponent<CompAnimation>();

                            if (anim_controller.IsAnimationStopped("Cover"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Daenerys Covering");
                            }
                            break;
                        }
                    case State.BLOCKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller = daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller.IsAnimationStopped("Block"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Daenerys Blocking");
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
                                Debug.Log("Daenerys Hit");
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
            Debug.Log("Daenerys Pressed 1");
            PrepareLeftAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Daenerys Pressed 2");
            PrepareRightAbility();
        }
    }

    public override void SecondaryAbility()
    {
        Debug.Log("Daenerys Secondary Ability");
    }


    public override void GetDamage(float dmg)
    {
        Debug.Log("Daenerys Get Damage");

        health = GetLinkedObject("health_obj").GetComponent<Hp>();
        health.GetDamage(dmg);

        // SET HIT ANIMATION
        SetAnimationTransition("ToHit", true);

        SetState(State.HIT);
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller = daenerys_obj.GetComponent<CompAnimation>();
        anim_controller.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active)
    {
        //Update Hp bar
        if (active)
        {
            Debug.Log("Update HP Daenerys");
            
            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            //Disable Stamina Bar
            icon = GetLinkedObject("stamina_obj").GetComponent<CompImage>();
            icon.SetEnabled(false, GetLinkedObject("stamina_obj"));

            Debug.Log("Disabled Stamina Daenerys");

            //Enable Mana Bar
            icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
            icon.SetEnabled(true, GetLinkedObject("mana_obj"));

            Debug.Log("Enabled Mana Daenerys");

            //Update Mana 
            mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
            mana.SetMana(curr_mana, max_mana);

            Debug.Log("Set Mana Daenerys");
        }

        //Get values from var and store them
        else
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            curr_hp = health.GetCurrentHealth();

            mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
            curr_mana = mana.GetCurrentMana();

            //Disable Mana Bar
            icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
            icon.SetEnabled(false, GetLinkedObject("mana_obj"));

            //Enable Stamina Bar
            icon = GetLinkedObject("stamina_obj").GetComponent<CompImage>();
            icon.SetEnabled(true, GetLinkedObject("stamina_obj"));
        }

        Debug.Log("Update Child Daenerys");

        //Change current character icon
        icon = daenerys_icon_obj.GetComponent<CompImage>();
        icon.SetEnabled(active, daenerys_icon_obj);
    }

    public override bool IsAnimationStopped(string name)
    {
        anim_controller = daenerys_obj.GetComponent<CompAnimation>();
        return anim_controller.IsAnimationStopped(name);
    }

    public override void ToggleMesh(bool active)
    {
        rarm_daenerys_obj.GetComponent<CompMesh>().SetEnabled(active, rarm_daenerys_obj);
        larm_daenerys_obj.GetComponent<CompMesh>().SetEnabled(active, larm_daenerys_obj);
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
                    Debug.Log("Daenerys LW Going to Attack");

                    //Set Attacking State
                    SetState(State.ATTACKING);

                    // First, OnClick of LeftWeapon, then, onClick of Cooldown
                    DoLeftAbility();

                    // Set Attacking Animation
                    SetAnimationTransition("ToAttack1", true);

                    // Play the Sound FX
                    PlayFx("Dracarys");

                    return true;
                }
                else
                {
                    Debug.Log("Ability doesn't have enough charges");
                    return false;
                }
            }
            else
            {
                Debug.Log("Not Enough Mana");
                return false;
            }
        }
        return false;
    }

    public void PrepareLeftAbility()
    {
        Debug.Log("Daenerys LW Prepare Ability");
        button = daenerys_button_left_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoLeftAbility() //Might be virtual
    {
        Debug.Log("Daenerys LW Attack Left");

        // Decrease mana -----------
        DecreaseManaPercentage(mana_cost_percentage_left);

        Debug.Log("Daenerys LW Going to hit");

        // Attack all enemies in 3 rows in front of you
        //if (GetLinkedObject("player_obj").GetComponent<MovementController>().EnemyInFront())
        //{
        //    // To change => check the specific enemy in front of you
        //    enemy = enemy_obj.GetComponent<EnemyController>();
        //    enemy.HitPercentage(damage_percentage);
        //}
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
            float mana_cost = mana_cost_percentage * max_mana / 100.0f;
            if (GetCurrentMana() > mana_cost)
            {
                cd_right = daenerys_button_right_obj.GetComponent<DaenerysCD_Right>();
                //Check if the ability is not in cooldown
                if (!cd_right.in_cd)
                {
                    SetState(State.COVER);
                    Debug.Log("Daenerys RW Going to Set Fire Wall");

                    // First, OnClick of RightWeapon, then, onClick of Cooldown
                    DoRightAbility();

                    // Set Animation
                    SetAnimationTransition("ToFireWall", true);

                    return true;
                }
                else
                {
                    Debug.Log("Daenerys RW Ability in CD");
                    return false;
                }
            }
            else
            {
                Debug.Log("Daenerys RW Not Enough Mana");
                return false;
            }
        }
        return false;
    }

    public void PrepareRightAbility()
    {
        Debug.Log("Daenerys RW Prepare Fire Wall");
        button = daenerys_button_right_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoRightAbility()
    {
        Debug.Log("Daenerys RW Attack Right");

        // Decrease mana -----------
        daenerys_obj.GetComponent<DaenerysController>();
        DecreaseManaPercentage(mana_cost_percentage);

        Debug.Log("Daenerys RW Going to hit");

        // Set a fire wall in north tile
        //if (GetLinkedObject("player_obj").GetComponent<MovementController>().EnemyInFront())
        //{
        //    // To change => check the specific enemy in front of you
        //    enemy = enemy_obj.GetComponent<EnemyController>();
        //    enemy.HitPercentage(damage_percentage);
        //}
    }

    //---------------------------   
}