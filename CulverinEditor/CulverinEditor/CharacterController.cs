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
        HIT,
        DEAD
    }

    public enum Position
    {
        CURRENT = 0,
        BEHIND
    }

    public GameObject player_obj;

    public MovementController movement;             // To manage when the player is moving to block attacks/abilities
    //public GameObject health_obj;
    public Hp health;                               // To handle current hp
    //public GameObject stamina_obj;
    public Stamina stamina;                         // To handle current stamina
    //public GameObject rweapon_obj;
    public WeaponController right_weapon;                // Script that will handle right weapon the player is carrying (with its own progression system, stats...)
    public CompAnimation anim_controller_right;     // Animation component to handle animations
    //public GameObject lweapon_obj;
    public WeaponController left_weapon;                  // Script that will handle left weapon the player is carrying (with its own progression system, stats...)
    public CompAnimation anim_controller_left;      // Animation component to handle animations

    protected State state = State.IDLE;                       // To manage player state
    protected Position position = Position.CURRENT;           // To manage player position

    public virtual void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        //player_obj = GetLinkedObject("player_obj");
        //health_obj = GetLinkedObject("health_obj");
        //stamina_obj = GetLinkedObject("stamina_obj");
        //lweapon_obj = GetLinkedObject("lweapon_obj");
        //rweapon_obj = GetLinkedObject("rweapon_obj");

        //// Start Idle animation
        //anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
        //anim_controller_left.PlayAnimation("Idle");
        //anim_controller_right = rweapon_obj.GetComponent<CompAnimation>();
        //anim_controller_right.PlayAnimation("Idle");
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

    public virtual void ControlCharacter() //Might be virtual
    {
    }

    public virtual void CheckAttack()
    {
        //Left Attack
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            Debug.Log("Pressed 1");
            //left_weapon = lweapon_obj.GetComponent<WeaponController>();
            //left_weapon.PrepareAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Pressed 2");
            //right_weapon = rweapon_obj.GetComponent<WeaponController>();
            //right_weapon.PrepareAbility();
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
    
    public void SetPosition(Position new_position)
    {
        position = new_position;
    }

    public int GetPosition()
    {
        return (int)position;
    }

    public virtual float GetCurrentStamina()
    {
        //stamina = stamina_obj.GetComponent<Stamina>();
        //float ret = stamina.GetCurrentStamina();
        //return ret;
        return 0;
    }

    public virtual void DecreaseStamina(float stamina_cost)
    {
        //Debug.Log("Decrease Stamina");
        //stamina = stamina_obj.GetComponent<Stamina>();
        //stamina.DecreaseStamina(stamina_cost);
    }

    public virtual void CheckHealth()
    {
        // Debug for check Health control
        //if (Input.GetKeyDown(KeyCode.O))
        //{
        //    health = health_obj.GetComponent<Hp>();
        //    health.GetDamage(10.0f);
        //}
        //else if (Input.GetKeyDown(KeyCode.P))
        //{
        //    health = health_obj.GetComponent<Hp>();
        //    health.GetDamage(-10.0f);
        //}
    }

    public virtual void GetDamage(float dmg)
    {
        //if (state == State.COVER)
        //{
        //    anim_controller_right = rweapon_obj.GetComponent<CompAnimation>();
        //    anim_controller_right.SetTransition("ToBlock");

        //    anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
        //    anim_controller_left.SetTransition("ToBlock");

        //    player_obj.GetComponent<CompAudio>().PlayEvent("MetalHit");

        //    SetState(State.BLOCKING);
        //}
        //else
        //{
        //    health = health_obj.GetComponent<Hp>();
        //    health.GetDamage(dmg);

        //    // SET HIT ANIMATION
        //    anim_controller_right = rweapon_obj.GetComponent<CompAnimation>();
        //    anim_controller_right.SetTransition("ToHit");

        //    anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
        //    anim_controller_left.SetTransition("ToHit");
        //}
    }

    public virtual void SetAnimationTransition(string name, bool value)
    {
        //anim_controller_right = rweapon_obj.GetComponent<CompAnimation>();
        //anim_controller_right.SetTransition(name, value);

        //anim_controller_left = lweapon_obj.GetComponent<CompAnimation>();
        //anim_controller_left.SetTransition(name, value);
    }
}