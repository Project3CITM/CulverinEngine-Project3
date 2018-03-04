using CulverinEditor;
using CulverinEditor.Debug;

public class TheonController : CharacterController
{
    public GameObject theon_obj; //Maybe this should be in charactercontroller since we're only having 1 camera which will be the movementcontroller/charactermanager?

    public GameObject rweapon_theon_obj;
    public GameObject lweapon_theon_obj;
    public GameObject theon_icon_obj;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;

    protected override void Start()
    {
        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        theon_obj = GetLinkedObject("theon_obj");
        theon_icon_obj = GetLinkedObject("theon_icon_obj");
        lweapon_theon_obj = GetLinkedObject("lweapon_theon_obj");
        rweapon_theon_obj = GetLinkedObject("rweapon_theon_obj");

        //Disable icon
        icon = theon_icon_obj.GetComponent<CompImage>();
        icon.SetEnabled(false, theon_icon_obj);

        Debug.Log(gameObject.GetName());

        // Start Idle animation
        //anim_controller_left = lweapon_theon_obj.GetComponent<CompAnimation>();
        //anim_controller_left.PlayAnimation("Idle");
        //anim_controller_right = rweapon_theon_obj.GetComponent<CompAnimation>();
        //anim_controller_right.PlayAnimation("Idle");
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
                //switch (state)
                //{
                //    case State.IDLE:
                //        {
                //            //Check For Input + It has to check if he's moving to block attack (¿?)
                //            CheckAttack();
                //            break;
                //        }
                //    case State.ATTACKING:
                //        {
                //            //Check for end of the Attack animation
                //            anim_controller_left = lweapon_theon_obj.GetComponent<CompAnimation>();
                //            if (anim_controller_left.IsAnimationStopped("Attack1"))
                //            {
                //                state = State.IDLE;
                //            }
                //            else
                //            {
                //                // Keep playing specific attack animation  until it ends
                //                Debug.Log("Theon Attacking");
                //            }
                //            break;
                //        }
                //    case State.COVER:
                //        {
                //            //Check for end of the Attack animation
                //            anim_controller_left = lweapon_theon_obj.GetComponent<CompAnimation>();

                //            if (anim_controller_left.IsAnimationStopped("Cover"))
                //            {
                //                state = State.IDLE;
                //            }
                //            else
                //            {
                //                // Keep playing specific attack animation  until it ends
                //                Debug.Log("Jaime Covering");
                //            }
                //            break;
                //        }
                //    case State.BLOCKING:
                //        {
                //            //Check for end of the Attack animation
                //            anim_controller_left = lweapon_theon_obj.GetComponent<CompAnimation>();
                //            if (anim_controller_left.IsAnimationStopped("Block"))
                //            {
                //                state = State.IDLE;
                //            }
                //            else
                //            {
                //                // Keep playing specific attack animation  until it ends
                //                Debug.Log("Theon Blocking");
                //            }
                //            break;
                //        }
                //    case State.HIT:
                //        {
                //            //Check for end of the Attack animation
                //            anim_controller_left = lweapon_theon_obj.GetComponent<CompAnimation>();
                //            if (anim_controller_left.IsAnimationStopped("Hit"))
                //            {
                //                state = State.IDLE;
                //            }
                //            else
                //            {
                //                // Keep playing specific attack animation  until it ends
                //                Debug.Log("Theon Hit");
                //            }
                //            break;
                //        }
                //    case State.DEAD:
                //        {
                //            Debug.Log("We are going doown");
                //            break;
                //        }
                //    default:
                //        {
                //            break;
                //        }
                //}
            }
        }
    }

    public override void CheckAttack()
    {
        //Left Attack
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            Debug.Log("Theon Pressed 1");
            left_weapon = lweapon_theon_obj.GetComponent<JaimeWeapon_Left>();
            left_weapon.PrepareAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Theon Pressed 2");
            right_weapon = rweapon_theon_obj.GetComponent<JaimeWeapon_Right>();
            right_weapon.PrepareAbility();
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

            GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("MetalHit");

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
        //anim_controller_right = rweapon_theon_obj.GetComponent<CompAnimation>();
        //anim_controller_right.SetTransition(name, value);

        //anim_controller_left = lweapon_theon_obj.GetComponent<CompAnimation>();
        //anim_controller_left.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active)
    {
        Debug.Log("Update HP Theon");
        
        //Update Hp bar
        if (active)
        {
            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            //Update Stamina
            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            stamina.SetStamina(curr_stamina, max_stamina);
        }

        Debug.Log("Update Child Theon");

        //Change current character icon
        icon = theon_icon_obj.GetComponent<CompImage>();
        icon.SetEnabled(active, theon_icon_obj);
    }
}