using CulverinEditor;
using CulverinEditor.Debug;

public class CharacterController : CulverinBehaviour
{
    public enum State
    {
        IDLE = 0,
        ATTACKING,
        BLOCKING,
        COVER,
        DEAD
    }

    public enum Position
    {
        CURRENT = 0,
        BEHIND
    }

    public GameObject player_obj;

    public MovementController movement;             // To manage when the player is moving to block attacks/abilities
    public GameObject health_obj;
    public Hp health;                               // To handle current hp
    public GameObject stamina_obj;
    public Stamina stamina;                         // To handle current stamina
    public GameObject rweapon_obj;
    public RightWeapon right_weapon;                // Script that will handle right weapon the player is carrying (with its own progression system, stats...)
    public CompAnimation anim_controller_right;     // Animation component to handle animations
    public GameObject lweapon_obj;
    public LeftWeapon left_weapon;                  // Script that will handle left weapon the player is carrying (with its own progression system, stats...)
    public CompAnimation anim_controller_left;      // Animation component to handle animations

    State state = State.IDLE;                       // To manage player state
    Position position = Position.CURRENT;           // To manage player position

    void Start()
    {
        // Start Idle animation
        lweapon_obj = GetLinkedObject("lweapon_obj");
        anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
        anim_controller_left.PlayAnimation("Idle");
        rweapon_obj = GetLinkedObject("rweapon_obj_anim");
        anim_controller_right = rweapon_obj.GetComponent<CompAnimation>();
        anim_controller_right.PlayAnimation("Idle");
    }

    void Update()
    {
        if (position == Position.CURRENT)
        {
            ControlCharacter();
        }
        else if(position == Position.BEHIND)
        {
            // Special Ability is controlled by CharactersManager
        }
    }

    public void ControlCharacter()
    {
        // Debug method to control Hp
        CheckHealth();

        // First check if you are alive
        health_obj = GetLinkedObject("health_obj");
        health = health_obj.GetComponent<Hp>();
        if (health.GetCurrentHealth() > 0)
        {
            // Check if player is moving to block attacks/abilities
            movement = GetComponent<MovementController>();
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
                            lweapon_obj = GetLinkedObject("lweapon_obj");
                            anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
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
                            lweapon_obj = GetLinkedObject("lweapon_obj");
                            anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();

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
                            lweapon_obj = GetLinkedObject("lweapon_obj");
                            anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
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

    public void CheckAttack()
    {
        //Left Attack
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            Debug.Log("Pressed 1");
            lweapon_obj = GetLinkedObject("lweapon_obj");
            left_weapon = lweapon_obj.GetComponent<LeftWeapon>();
            left_weapon.PrepareAttack();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Pressed 2");
            rweapon_obj = GetLinkedObject("rweapon_obj");
            right_weapon = rweapon_obj.GetComponent<RightWeapon>();
            right_weapon.PrepareAttack();
        }
    }

    public void SetState(State new_state)
    {
        state = new_state;
    }

    public int GetState()
    {
        return (int)state;
    }

    public float GetCurrentStamina()
    {
        stamina_obj = GetLinkedObject("stamina_obj");
        stamina = stamina_obj.GetComponent<Stamina>();
        float ret = stamina.GetCurrentStamina();
        return ret;
    }

    public void DecreaseStamina(float stamina_cost)
    {
        Debug.Log("Decrease Stamina");
        stamina_obj = GetLinkedObject("stamina_obj");
        stamina = stamina_obj.GetComponent<Stamina>();
        stamina.DecreaseStamina(stamina_cost);
    }

    public void CheckHealth()
    {
        // Debug for check Health control
        if (Input.GetKeyDown(KeyCode.O))
        {
            health_obj = GetLinkedObject("health_obj");
            health = health_obj.GetComponent<Hp>();
            health.GetDamage(10.0f);
        }
        else if (Input.GetKeyDown(KeyCode.P))
        {
            health_obj = GetLinkedObject("health_obj");
            health = health_obj.GetComponent<Hp>();
            health.GetDamage(-10.0f);
        }
    }

    public void GetDamage(float dmg)
    {
        if (state == State.COVER)
        {
            rweapon_obj = GetLinkedObject("rweapon_obj");
            anim_controller_right = rweapon_obj.GetComponent<CompAnimation>();
            anim_controller_right.SetTransition("ToBlock");

            lweapon_obj = GetLinkedObject("lweapon_obj");
            anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
            anim_controller_left.SetTransition("ToBlock");

            player_obj = GetLinkedObject("player_obj");
            player_obj.GetComponent<CompAudio>().PlayEvent("MetalHit");

            SetState(State.BLOCKING);
        }
        else
        {
            health_obj = GetLinkedObject("health_obj");
            health = health_obj.GetComponent<Hp>();
            health.GetDamage(dmg);

            // SET HIT ANIMATION
            rweapon_obj = GetLinkedObject("rweapon_obj");
            anim_controller_right = rweapon_obj.GetComponent<CompAnimation>();
            anim_controller_right.SetTransition("ToHit");

            lweapon_obj = GetLinkedObject("lweapon_obj");
            anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
            anim_controller_left.SetTransition("ToHit");
        }
    }
}