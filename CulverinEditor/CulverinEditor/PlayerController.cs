using CulverinEditor;
using CulverinEditor.Debug;

public class PlayerController : CulverinBehaviour
{
    public enum State
    {
        IDLE = 0,
        ATTACKING,
        MOVING
    }

    //public GameObject health_obj;
    /*public Hp health;                  */     // To handle current hp
    //public GameObject stamina_obj; 
    /*public Stamina stamina;            */     // To handle current stamina
    //public GameObject rweapon_obj;          // GO with weapon script
    public WeaponController right_weapon;   // Script that will handle right weapon the player is carrying (with its own progression system, stats...)
    //public GameObject lweapon_obj;          // GO with weapon script
    //public WeaponController left_weapon;    // Script that will handle left weapon the player is carrying (with its own progression system, stats...)
    //public CompAnimation anim_controller;   // Animation component to handle animations
    //public MovementController movement;     // Script that will handle player movement (tiles, ismoving...)

    State state = State.IDLE;               // To manage player state
    //bool attack_anim = false;               // True when reproducing attack animation 
    //bool combat_mode = false;               // True when enemy is in a near tile 
    int hp = 100;

    public CompAudio sound_fx;

    void Start()
    {
        state = State.IDLE;
        //health = health_obj.GetComponent<Hp>();
        //stamina = stamina_obj.GetComponent<Stamina>();
        //rweapon_obj = GetLinkedObject("rweapon_obj");
        right_weapon = GetComponent<WeaponController>();
        if(right_weapon == null)
        {
            Debug.Log("NULL OBJECT");
        }
        //lweapon_obj = GetLinkedObject("lweapon_obj");
        //left_weapon = lweapon_obj.GetComponent<WeaponController>();
        //anim_controller = GetComponent<CompAnimation>();
    }

    void Update()
    {
        Debug.Log("Idle");
        CheckAttack();

        // First check if you are alive
        //if (/*health.GetCurrentHealth() > 0*/ hp > 0)
        //{
        //    /* Player is alive */
        //    switch(state)
        //    {
        //        case State.IDLE:
        //            {
        //                //Check For Input
        //                Debug.Log("Idle");
        //                CheckAttack();
        //                break;
        //            }
        //        case State.ATTACKING:
        //            {
        //                //Check for end of the Attack animation
        //                if(/*!attack_anim*/ 1 == 1)
        //                {
        //                    state = State.IDLE;
        //                }
        //                else
        //                {
        //                    // Keep playing specific attack animation  until it ends
        //                    //if(anim_controller.HasEnded())
        //                    //{
        //                    //    attack_anim = false;
        //                    //}
        //                }
        //                break;
        //            }
        //        default:
        //            {
        //                break;
        //            }
        //    }
        //}
        //else
        //{
        //    Debug.Log("WE ARE GOING DOOOWN");
        //}
    }

    public void CheckAttack()
    {
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            Debug.Log("1");
            //sound_fx.PlayEvent("SwordSlash");
            //Debug.Log("CheckAttack");
            right_weapon = GetComponent<WeaponController>();
            //Debug.Log("GetComponent");
            right_weapon.Attack();
            //left_weapon.button.Clicked();
        }
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("2");
            //sound_fx.PlayEvent("SwordSlash");
            //Debug.Log("CheckAttack");
            //right_weapon = GetComponent<WeaponController>();
            //Debug.Log("GetComponent");
            //right_weapon.Attack();
            //right_weapon.button.Clicked();
        }
    }

    public void SetState(State new_state)
    {
        state = new_state;
    }

    //public void SetAttackAnim(bool active)
    //{
    //    attack_anim = active;
    //}
}