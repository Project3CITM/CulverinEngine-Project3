using CulverinEditor;
using CulverinEditor.Debug;

public class PlayerController2 : CulverinBehaviour
{
    public enum State
    {
        IDLE = 0,
        ATTACKING,
    }

    public GameObject health_obj;
    public Hp health;                           // To handle current hp
    public GameObject stamina_obj;
    public Stamina stamina;                     // To handle current stamina
    public GameObject rweapon_obj;          
    public WeaponController right_weapon;       // Script that will handle right weapon the player is carrying (with its own progression system, stats...)
    public GameObject lweapon_obj;          
    public WeaponController left_weapon;        // Script that will handle left weapon the player is carrying (with its own progression system, stats...)
    public CompAnimation anim_controller;       // Animation component to handle animations

    State state = State.IDLE;                   // To manage player state
    bool combat_mode = false;                   // True when enemy is in a near tile 

    void Start()
    {
    }

    void Update()
    {
        // First check if you are alive
        health_obj = GetLinkedObject("health_obj");
        health = health_obj.GetComponent<Hp>();
        if (health.GetCurrentHealth() > 0)
        {
            /* Player is alive */
            switch (state)
            {
                case State.IDLE:
                    {
                        //Check For Input
                        CheckAttack();
                        break;
                    }
                case State.ATTACKING:
                    {
                        //Check for end of the Attack animation
                        if (!attack_anim)
                        {
                            state = State.IDLE;
                        }
                        else
                        {
                            // Keep playing specific attack animation  until it ends
                            /*    if(anim_controller.HasEnded())
                                {
                                    attack_anim = false;
                                }*/
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
            lweapon_obj = GetLinkedObject("lweapon_obj");
            left_weapon = lweapon_obj.GetComponent<WeaponController>();
            left_weapon.button.Clicked();

        }
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            rweapon_obj = GetLinkedObject("rweapon_obj");
            right_weapon = rweapon_obj.GetComponent<WeaponController>();
            right_weapon.button.Clicked();
        }
    }

    public void SetState(State new_state)
    {
        state = new_state;
    }

    public float GetCurrentStamina()
    {
        stamina_obj = GetLinkedObject("stamina_obj");
        stamina = stamina_obj.GetComponent<Stamina>();
        float ret = stamina.GetCurrentStamina();
        return ret;
    }

}