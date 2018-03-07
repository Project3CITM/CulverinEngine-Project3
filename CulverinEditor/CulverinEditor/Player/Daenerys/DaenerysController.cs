using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysController : CharacterController
{
    public GameObject daenerys_obj; //Maybe this should be in charactercontroller since we're only having 1 camera which will be the movementcontroller/charactermanager?

    public GameObject rweapon_daenerys_obj;
    public GameObject lweapon_daenerys_obj;
    public GameObject daenerys_icon_obj;
    
    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_mana = 100.0f;
    public float curr_mana = 100.0f;
    public float sec_ability_cd = 10.0f;
    private float sec_ability_current_cd = 10.0f;

    protected override void Start()
    {
        SetPosition(Position.BEHIND);
        sec_ability_current_cd = sec_ability_cd;
        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        daenerys_obj = GetLinkedObject("daenerys_obj");
       daenerys_icon_obj = GetLinkedObject("daenerys_icon_obj");
       lweapon_daenerys_obj = GetLinkedObject("lweapon_daenerys_obj");
       rweapon_daenerys_obj = GetLinkedObject("rweapon_daenerys_obj");

       //Disable icon
       icon = daenerys_icon_obj.GetComponent<CompImage>();
       icon.SetEnabled(false, daenerys_icon_obj);

       //Disable Mana bar
       icon = GetLinkedObject("mana_obj").GetComponent<CompImage>();
       icon.SetEnabled(false, GetLinkedObject("mana_obj"));

       Debug.Log(gameObject.GetName());

       // Start Idle animation
       anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
       anim_controller_left.PlayAnimation("Idle");
       anim_controller_right = rweapon_daenerys_obj.GetComponent<CompAnimation>();
       anim_controller_right.PlayAnimation("Idle");

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
                            anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Attack1"))
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
                            anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();

                            if (anim_controller_left.IsAnimationStopped("Cover"))
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
                            anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Block"))
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
                            anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Hit"))
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
            lweapon_daenerys_obj.GetComponent<DaenerysWeapon_Left>().PrepareAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Daenerys Pressed 2");
            rweapon_daenerys_obj.GetComponent<DaenerysWeapon_Right>().PrepareAbility();
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
        anim_controller_right = rweapon_daenerys_obj.GetComponent<CompAnimation>();
        anim_controller_right.SetTransition(name, value);

        anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
        anim_controller_left.SetTransition(name, value);
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
        anim_controller_right = rweapon_daenerys_obj.GetComponent<CompAnimation>();
        return anim_controller_right.IsAnimationStopped(name);
    }

    public override void ToggleMesh(bool active)
    {
        lweapon_daenerys_obj.GetComponent<CompMesh>().SetEnabled(active, lweapon_daenerys_obj);
        rweapon_daenerys_obj.GetComponent<CompMesh>().SetEnabled(active, rweapon_daenerys_obj);
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
}