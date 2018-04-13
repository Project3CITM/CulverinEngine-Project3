using CulverinEditor;
using CulverinEditor.Debug;

public class CharacterController : CulverinBehaviour
{
    public enum Position
    {
        CURRENT = 0,
        BEHIND_LEFT,
        BEHIND_RIGHT
    }

    //Enum to control all the states of each character
    public enum State 
    {
        IDLE = 0,
        ATTACKING,
        BLOCKING,
        COVER,
        HIT,
        FIRE_WALL,
        RELOADING,
        STUN,
        FAIL_ATTACK,
        DEAD
    }

    protected MovementController movement;              // To manage when the player is moving to block attacks/abilities
    protected Hp health;                                // To handle current hp
    protected Stamina stamina;                          // To handle current stamina
    protected Mana mana;                                // To handle current mana
    protected CompAnimation anim_controller;            // Animation component to handle animations
    protected CompImage icon;
    protected CompButton button;
    protected CompAudio audio;
    protected EnemyController enemy;

    protected Position position = Position.CURRENT; 
    protected State state = State.IDLE;

    public Vector3 curr_position = Vector3.Zero;
    public Vector3 curr_forward = Vector3.Zero;

    protected virtual void Start()
    {
    }

    public virtual void Update()
    {
        if (position == Position.CURRENT)
        {
            ControlCharacter();
        }

        else if(position == Position.BEHIND_LEFT || position == Position.BEHIND_RIGHT)
        {
            if(gameObject.GetName() == "Theon")
            {
                gameObject.GetComponent<TheonController>().TheonSecondaryAbility();
            }

            //If the character is behind, manage the stamina/mana bar to regen it
            ManageEnergy();
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

    public virtual bool CanWasteStamina(float value)
    {
        stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
        return stamina.CanWasteStamina(value);
    }

    public virtual bool CanWasteMana(float value)
    {
        mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
        return mana.CanWasteMana(value);
        
    }

    public virtual void DecreaseStamina(float stamina_cost)
    {
        stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
        stamina.DecreaseStamina(stamina_cost);
    }

    public virtual void DecreaseMana(float mana_cost)
    {
        mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
        mana.DecreaseMana(mana_cost);
    }

    public virtual void DecreaseManaPercentage(float mana_cost)
    {
        mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
        mana.DecreaseManaPercentage(mana_cost);
    }

    public void PlayFx(string name)
    {
        GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent(name);
    }

    public virtual void ControlCharacter()
    {
    }

    public virtual void ManageEnergy()
    {
    }

    public virtual void CheckAttack()
    {
    }

    public virtual void SecondaryAbility()
    {
    }

    public virtual bool GetDamage(float dmg)
    {
        return true;
    }

    public virtual void SetAnimationTransition(string name, bool value)
    {
    }

    public virtual bool IsAnimationStopped(string name)
    {
        return true;
    }

    public virtual bool IsAnimationRunning(string name)
    {
        return true;
    }


    public virtual void UpdateHUD(bool active, bool left)
    {
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

    public virtual void EnableAbilities(bool active)
    {
    }

    public virtual Vector3 GetSecondaryPosition(Vector3 player_pos)
    {
        Vector3 ret = new Vector3(player_pos.x, player_pos.y - 5.0f, player_pos.z);

        if (position == Position.BEHIND_LEFT)
        {
            if (curr_forward.z <= -2.0f)
            {
                ret.x -= 6.5f;
            }
            else if (curr_forward.z >= 2.0f)
            {
                ret.x += 6.5f;
            }
            else if (curr_forward.x >= 2.0f)
            {
                ret.z -= 6.5f;
            }
            else if (curr_forward.x <= 2.0f)
            {
                ret.z += 6.5f;
            }
            
        }
        else if (position == Position.BEHIND_RIGHT)
        {
            if (curr_forward.z <= -2.0f)
            {
                ret.x += 6.5f;
            }
            else if (curr_forward.z >= 2.0f)
            {
                ret.x -= 6.5f;
            }
            else if (curr_forward.x >= 2.0f)
            {
                ret.z += 6.5f;
            }
            else if (curr_forward.x <= -2.0f)
            {
                ret.z -= 6.5f;
            }
        }

        return ret;
    }
}