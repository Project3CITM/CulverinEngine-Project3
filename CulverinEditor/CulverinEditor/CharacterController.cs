using CulverinEditor;
using CulverinEditor.Debug;

public class CharacterController : CulverinBehaviour
{
    public enum State
    {
        IDLE = 0,
        ATTACKING,
    }

    //public GameObject health_obj;
    //public Hp health;                           // To handle current hp
    //public GameObject stamina_obj;
    //public Stamina stamina;                     // To handle current stamina
    public GameObject rweapon_obj;
    public WeaponController right_weapon;       // Script that will handle right weapon the player is carrying (with its own progression system, stats...)
    public GameObject lweapon_obj;
    public WeaponController left_weapon;        // Script that will handle left weapon the player is carrying (with its own progression system, stats...)
    //public CompAnimation anim_controller;       // Animation component to handle animations

    State state = State.IDLE;                   // To manage player state
    bool combat_mode = false;                   // True when enemy is in a near tile 
    public string anim_name = "Idle";

    float anim_time = 0.0f; // 

    void Start()
    {
        // Link GameObject variables with Scene GameObjects
        //health_obj = GetLinkedObject("health_obj");
        //stamina_obj = GetLinkedObject("stamina_obj");
        lweapon_obj = GetLinkedObject("lweapon_obj");
        rweapon_obj = GetLinkedObject("rweapon_obj");

        // Start Idle animation
        //anim_controller = GetComponent<CompAnimation>();
        //anim_controller.PlayAnimation(anim_name);
    }

    void Update()
    {
        // Debug method to control Hp
        //CheckHealth();

        // First check if you are alive
        //health = health_obj.GetComponent<Hp>();
        if (/*health.GetCurrentHealth() > 0*/ 1 == 1)
        {       
            /* Player is alive */
            switch (state)
            {
                case State.IDLE:
                    {
                        //Check For Input + It has to check if he's moving to block attack (¿?)
                        CheckAttack();
                        //Debug.Log("IDLE");
                        anim_time = 0.0f;
                        break;
                    }
                case State.ATTACKING:
                    {
                        //Check for end of the Attack animation
                        //anim_controller = GetComponent<CompAnimation>();
                        //if (anim_controller.IsAnimationStopped(anim_name))
                        //{
                        //    anim_controller = GetComponent<CompAnimation>();
                        //    anim_controller.PlayAnimation("Idle");
                        //    state = State.IDLE;
                        //}
                        //else
                        //{
                        //    // Keep playing specific attack animation  until it ends
                        //    Debug.Log("Attacking");
                        //}
                        anim_time += Time.DeltaTime();
                        if(anim_time >= 3.0f)
                        {
                            SetState(State.IDLE);
                            Debug.Log("Changed State to IDLE");
                        }
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }
        else
        {
            Debug.Log("WE ARE GOING DOOOWN");
        }
    }

    public void CheckAttack()
    {
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            Debug.Log("Pressed 1");
            left_weapon = lweapon_obj.GetComponent<WeaponController>();
            left_weapon.PrepareAttack();
        }
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Pressed 2");
            right_weapon = rweapon_obj.GetComponent<WeaponController>();
            right_weapon.PrepareAttack();
        }
    }

    public void SetState(State new_state)
    {
        state = new_state;
    }

    //public float GetCurrentStamina()
    //{
    //    stamina = stamina_obj.GetComponent<Stamina>();
    //    float ret = stamina.GetCurrentStamina();
    //    return ret;
    //}

    //public void DecreaseStamina(float stamina_cost)
    //{
    //    stamina = stamina_obj.GetComponent<Stamina>();
    //    stamina.DecreaseStamina(stamina_cost);        
    //}

    //public void CheckHealth()
    //{
    //    // Debug for check Health control
    //    if (Input.GetKeyDown(KeyCode.O))
    //    {
    //        health = health_obj.GetComponent<Hp>();
    //        health.GetDamage(10.0f);

    //    }
    //    else if (Input.GetKeyDown(KeyCode.P))
    //    {
    //        health = health_obj.GetComponent<Hp>();
    //        health.GetDamage(-10.0f);
    //    }
    //}

    //public void SetAnim(string anim_name)
    //{
    //    anim_controller = GetComponent<CompAnimation>();
    //    anim_controller.PlayAnimation(anim_name);
    //}

    public void SetAnimName(string new_name)
    {
        anim_name = new_name;
    }

}