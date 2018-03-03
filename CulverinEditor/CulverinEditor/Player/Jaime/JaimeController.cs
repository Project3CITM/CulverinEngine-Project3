using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeController : CharacterController
{
    public GameObject jaime_obj; //Maybe this should be in charactercontroller since we're only having 1 camera which will be the movementcontroller/charactermanager?

    public GameObject health_jaime_obj;
    public GameObject stamina_jaime_obj;
    public GameObject rweapon_jaime_obj;
    public GameObject lweapon_jaime_obj;

    protected override void Start()
    {
        jaime_obj = GetLinkedObject("jaime_obj");
        health_jaime_obj = GetLinkedObject("health_jaime_obj");
        stamina_jaime_obj = GetLinkedObject("stamina_jaime_obj");
        lweapon_jaime_obj = GetLinkedObject("lweapon_jaime_obj");
        rweapon_jaime_obj = GetLinkedObject("rweapon_jaime_obj");

        // Start Idle animation
        anim_controller_left = lweapon_jaime_obj.GetComponent<CompAnimation>();
        anim_controller_left.PlayAnimation("Idle");
        anim_controller_right = rweapon_jaime_obj.GetComponent<CompAnimation>();
        anim_controller_right.PlayAnimation("Idle");
    }

    public override void ControlCharacter()
    {
        // Debug method to control Hp
        CheckHealth();

        // First check if you are alive
        health = health_jaime_obj.GetComponent<Hp>();
        if (health.GetCurrentHealth() > 0)
        {
            // Check if player is moving to block attacks/abilities
            movement = jaime_obj.GetComponent<MovementController>();
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
                            anim_controller_left = lweapon_jaime_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Attack1"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Attacking");
                            }
                            break;
                        }
                    case State.COVER:
                        {
                            //Check for end of the Attack animation
                            anim_controller_left = lweapon_jaime_obj.GetComponent<CompAnimation>();

                            if (anim_controller_left.IsAnimationStopped("Cover"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Covering");
                            }
                            break;
                        }
                    case State.BLOCKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller_left = lweapon_jaime_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Block"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Blocking");
                            }
                            break;
                        }
                    case State.HIT:
                        {
                            //Check for end of the Attack animation
                            anim_controller_left = lweapon_jaime_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Hit"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Hit");
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
            Debug.Log("Pressed 1");
            left_weapon = lweapon_jaime_obj.GetComponent<JaimeWeapon_Left>();
            left_weapon.PrepareAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Pressed 2");
            right_weapon = rweapon_jaime_obj.GetComponent<JaimeWeapon_Right>();
            right_weapon.PrepareAbility();
        }
    }

    public override void SecondaryAbility()
    {
        Debug.Log("Jaime Secondary Ability");
    }

    public override void CheckHealth()
    {
        // Debug for check Health control
        if (Input.GetKeyDown(KeyCode.O))
        {
            health = health_jaime_obj.GetComponent<Hp>();
            health.GetDamage(10.0f);
        }
        else if (Input.GetKeyDown(KeyCode.P))
        {
            health = health_jaime_obj.GetComponent<Hp>();
            health.GetDamage(-10.0f);
        }
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
            health = health_jaime_obj.GetComponent<Hp>();
            health.GetDamage(dmg);

            // SET HIT ANIMATION
            SetAnimationTransition("ToHit", true);

            SetState(State.HIT);
        }
    }

    public override float GetCurrentStamina()
    {
        stamina = stamina_jaime_obj.GetComponent<Stamina>();
        float ret = stamina.GetCurrentStamina();
        return ret;
    }

    public override void DecreaseStamina(float stamina_cost)
    {
        Debug.Log("Decrease Stamina");
        stamina = stamina_jaime_obj.GetComponent<Stamina>();
        stamina.DecreaseStamina(stamina_cost);
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller_right = rweapon_jaime_obj.GetComponent<CompAnimation>();
        anim_controller_right.SetTransition(name, value);

        anim_controller_left = lweapon_jaime_obj.GetComponent<CompAnimation>();
        anim_controller_left.SetTransition(name, value);
    }

}