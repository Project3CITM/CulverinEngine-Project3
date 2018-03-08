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

    protected MovementController movement;              // To manage when the player is moving to block attacks/abilities
    protected Hp health;                                // To handle current hp
    protected Stamina stamina;                          // To handle current stamina
    protected Mana mana;                                // To handle current mana
    protected CompAnimation anim_controller;            // Animation component to handle animations
    protected CompImage icon;
    protected CompButton button;
    protected EnemyController enemy;

    protected Position position = Position.CURRENT; 
    protected State state = State.IDLE;

    protected virtual void Start()
    {
        Debug.Log("Start Character");
    }

    public virtual void Update()
    {
        if (position == Position.CURRENT)
        {
            //ControlCharacter();
        }
        else if(position == Position.BEHIND)
        {
            ReduceSecondaryAbilityCoolDown();
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
        Debug.Log("Changing position");
        position = new_position;
    }

    public int GetPosition()
    {
        return (int)position;
    }

    public virtual float GetCurrentStamina()
    {
        stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
        float ret = stamina.GetCurrentStamina();
        return ret;
    }

    public virtual float GetCurrentMana()
    {
        mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
        float ret = mana.GetCurrentMana();
        return ret;
    }

    public virtual void DecreaseStamina(float stamina_cost)
    {
        Debug.Log("Decrease Stamina " + stamina_cost);
        stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
        stamina.DecreaseStamina(stamina_cost);
    }

    public virtual void DecreaseMana(float mana_cost)
    {
        Debug.Log("Decrease Stamina");
        mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
        mana.DecreaseMana(mana_cost);
    }

    public virtual void DecreaseManaPercentage(float mana_cost)
    {
        Debug.Log("Decrease Stamina");
        mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
        mana.DecreaseManaPercentage(mana_cost);
    }

    public void CheckHealth()
    {
        // Debug for check Health control
        if (Input.GetKeyDown(KeyCode.O))
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.GetDamage(10.0f);
        }
        else if (Input.GetKeyDown(KeyCode.P))
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.GetDamage(-10.0f);
        }
    }

    public void PlayFx(string name)
    {
        GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent(name);
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

    public virtual bool IsAnimationStopped(string name)
    {
        return true;
    }

    public virtual void UpdateHUD(bool active)
    {
        Debug.Log("Change Current Character HUD");
    }

    public virtual void ToggleMesh(bool active)
    {
    }

    public virtual float GetSecondaryAbilityCoolDown()
    {
        return 0.0f;
    }

    public virtual void ReduceSecondaryAbilityCoolDown()
    {
    }

    public virtual void ResetCoolDown()
    {
    }
}