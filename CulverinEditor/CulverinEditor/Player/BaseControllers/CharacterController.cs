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

    public GameObject player_obj;

    protected MovementController movement;             // To manage when the player is moving to block attacks/abilities
    protected Hp health;                               // To handle current hp
    protected Stamina stamina;                         // To handle current stamina
    protected WeaponController right_weapon;           // Script that will handle right weapon the player is carrying (with its own progression system, stats...)
    protected CompAnimation anim_controller_right;     // Animation component to handle animations
    protected WeaponController left_weapon;            // Script that will handle left weapon the player is carrying (with its own progression system, stats...)
    protected CompAnimation anim_controller_left;      // Animation component to handle animations

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
        return 0;
    }

    public virtual void DecreaseStamina(float stamina_cost)
    {
        Debug.Log("Decrease Stamina Character");
    }

    public virtual void CheckHealth()
    {
        Debug.Log("Check Health Character");
    }

    public virtual void GetDamage(float dmg)
    {
        Debug.Log("Get Damage Character");
    }

    public virtual void SetAnimationTransition(string name, bool value)
    {
        Debug.Log("Animation Transition Character");
    }
}