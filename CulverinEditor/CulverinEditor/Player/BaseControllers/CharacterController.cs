using CulverinEditor;
using CulverinEditor.Debug;

public class CharacterController : CulverinBehaviour
{
    public enum Position
    {
        CURRENT = 0,
        BEHIND
    }

    //Enum to control all the states of each character
    public enum State 
    {
        IDLE = 0,
        ATTACKING,
        BLOCKING,
        COVER,
        HIT,
        DEAD
    }
    public GameObject player_obj; //Parent GO that manage the movement of the player

    public GameObject health_obj;
    public GameObject stamina_obj;

    protected MovementController movement;             // To manage when the player is moving to block attacks/abilities
    protected Hp health;                               // To handle current hp
    protected Stamina stamina;                         // To handle current stamina
    protected WeaponController right_weapon;           // Script that will handle right weapon the player is carrying (with its own progression system, stats...)
    protected CompAnimation anim_controller_right;     // Animation component to handle animations
    protected WeaponController left_weapon;            // Script that will handle left weapon the player is carrying (with its own progression system, stats...)
    protected CompAnimation anim_controller_left;      // Animation component to handle animations
    protected CompImage icon;

    protected Position position = Position.CURRENT; 
    protected State state = State.IDLE;

    protected virtual void Start()
    {
        Debug.Log("Start Character");
    }

    void Update()
    {
        if (position == Position.CURRENT)
        {
            ControlCharacter();
        }
        else if(position == Position.BEHIND)
        {
            // Secondary Ability is controlled by CharactersManager
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
        stamina = stamina_obj.GetComponent<Stamina>();
        float ret = stamina.GetCurrentStamina();
        return ret;
    }


    public virtual void DecreaseStamina(float stamina_cost)
    {
        Debug.Log("Decrease Stamina");
        stamina = stamina_obj.GetComponent<Stamina>();
        stamina.DecreaseStamina(stamina_cost);
    }

    public void CheckHealth()
    {
        Debug.Log("Check Health");

        // Debug for check Health control
        if (Input.GetKeyDown(KeyCode.O))
        {
            health = health_obj.GetComponent<Hp>();
            health.GetDamage(10.0f);
        }
        else if (Input.GetKeyDown(KeyCode.P))
        {
            health = health_obj.GetComponent<Hp>();
            health.GetDamage(-10.0f);
        }
    }

    public virtual void ControlCharacter()
    {
        Debug.Log("Control Character");
    }

    public virtual void CheckAttack()
    {
        Debug.Log("Check Attack Character");
    }

    public virtual void SecondaryAbility()
    {
        Debug.Log("Secondary Ability Character");
    }

    public virtual void GetDamage(float dmg)
    {
        Debug.Log("Get Damage Character");
    }

    public virtual void SetAnimationTransition(string name, bool value)
    {
        Debug.Log("Animation Transition Character");
    }

    public virtual void UpdateHUD(bool active)
    {
        Debug.Log("Change Current Character Icon");
    }
}