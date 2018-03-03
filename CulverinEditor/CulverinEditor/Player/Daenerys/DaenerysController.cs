using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysController : CharacterController
{
    public GameObject daenerys_obj; //Maybe this should be in charactercontroller since we're only having 1 camera which will be the movementcontroller/charactermanager?

    public GameObject rweapon_daenerys_obj;
    public GameObject lweapon_daenerys_obj;

    protected override void Start()
    {
        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        player_obj = GetLinkedObject("player_obj");
        daenerys_obj = GetLinkedObject("daenerys_obj");
        health_obj = GetLinkedObject("health_obj");
        stamina_obj = GetLinkedObject("stamina_obj");
        lweapon_daenerys_obj = GetLinkedObject("lweapon_daenerys_obj");
        rweapon_daenerys_obj = GetLinkedObject("rweapon_daenerys_obj");

        // Start Idle animation
        anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
        anim_controller_left.PlayAnimation("Idle");
        anim_controller_right = rweapon_daenerys_obj.GetComponent<CompAnimation>();
        anim_controller_right.PlayAnimation("Idle");
    }

    public override void ControlCharacter()
    {
        // Debug method to control Hp
        CheckHealth();

        // First check if you are alive
        health = health_obj.GetComponent<Hp>();
        if (health.GetCurrentHealth() > 0)
        {
            // Check if player is moving to block attacks/abilities
            movement = player_obj.GetComponent<MovementController>();
            if (!movement.IsMoving())
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
            left_weapon = lweapon_daenerys_obj.GetComponent<JaimeWeapon_Left>();
            left_weapon.PrepareAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Daenerys Pressed 2");
            right_weapon = rweapon_daenerys_obj.GetComponent<JaimeWeapon_Right>();
            right_weapon.PrepareAbility();
        }
    }

    public override void SecondaryAbility()
    {
        Debug.Log("Daenerys Secondary Ability");
    }


    public override void GetDamage(float dmg)
    {
        Debug.Log("Daenerys Get Damage");

        health = health_obj.GetComponent<Hp>();
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

}