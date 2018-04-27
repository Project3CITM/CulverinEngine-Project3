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
        GRABBED,
        DEAD
    }

    //PLAYER
    protected GameObject player_obj;
    protected CharactersManager characters_manager;
    protected MovementController movement;              // To manage when the player is moving to block attacks/abilities
    protected CompMesh left_arm;
    protected CompMesh right_arm;

    //ICON
    protected CompRectTransform icon_trans;
    protected CompImage icon_img;
    protected CompImage icon_hp_img;
    protected CompImage icon_stamina_img;
    protected CompImage icon_mana_img;

    //BARS
    protected Hp health;                                // To handle current hp

    protected Stamina stamina;                          // To handle current stamina
    protected CompImage stamina_img;
    protected CompImage leftamina_img;

    protected Mana mana;                                // To handle current mana
    protected CompImage mana_img;
    protected CompImage left_mana_img;

    //BUTTONS
    protected CompButton left_button;
    protected CompText left_counter;
    protected CompImage left_button_img;
    protected CompImage left_button_idle_img;


    protected CompButton right_button;
    protected CompText right_counter;
    protected CompImage right_button_img;
    protected CompImage right_button_idle_img;

    protected CompButton sec_button;
    protected CompText sec_counter;
    protected CompImage sec_button_img;
    protected CompRectTransform sec_button_trans;
    protected CompImage sec_button_idle_img;
    protected CompRectTransform sec_button_idle_trans;


    //OTHERS
    protected CompAnimation anim_controller;            // Animation component to handle animations
    protected CompAudio audio;
    protected DamageFeedback damage_feedback;
    protected EnemiesManager enemy_manager;

    protected Position position = Position.CURRENT; 
    protected State state = State.IDLE;

    public Vector3 curr_position = Vector3.Zero;
    public Vector3 curr_forward = Vector3.Zero;

    protected void LinkComponents(GameObject icon_obj, GameObject icon_hp_obj, GameObject icon_stamina_obj, GameObject icon_mana_obj,
                                  GameObject left_button_obj, GameObject right_button_obj, GameObject sec_button_obj, GameObject sec_button_idle_obj,
                                  GameObject left_counter_obj, GameObject right_counter_obj, GameObject sec_counter_obj,
                                  GameObject left_arm_obj, GameObject right_arm_obj,
                                  GameObject left_button_idle_obj, GameObject right_button_idle_obj)
    {
        //LEFT ARM
        left_arm = left_arm_obj.GetComponent<CompMesh>();

        //RIGHT ARM
        right_arm = right_arm_obj.GetComponent<CompMesh>();

        //LEFT BUTTON
        left_button = left_button_obj.GetComponent<CompButton>();
        left_button_img = left_button_obj.GetComponent<CompImage>();
        left_counter = left_counter_obj.GetComponent<CompText>();

        left_button_idle_img = left_button_idle_obj.GetComponent<CompImage>();

        //RIGHT BUTTON
        right_button = right_button_obj.GetComponent<CompButton>();
        right_button_img = right_button_obj.GetComponent<CompImage>();
        right_counter = right_counter_obj.GetComponent<CompText>();

        right_button_idle_img = right_button_idle_obj.GetComponent<CompImage>();

        //SECONDARY BUTTON
        sec_button = sec_button_obj.GetComponent<CompButton>();
        sec_button_img = sec_button_obj.GetComponent<CompImage>();
        sec_counter = sec_counter_obj.GetComponent<CompText>();
        sec_button_trans = sec_button_obj.GetComponent<CompRectTransform>();

        sec_button_idle_img = sec_button_idle_obj.GetComponent<CompImage>();
        sec_button_idle_trans = sec_button_idle_obj.GetComponent<CompRectTransform>();

        //HUD ICON
        icon_trans = icon_obj.GetComponent<CompRectTransform>();
        icon_img = icon_obj.GetComponent<CompImage>();
        icon_hp_img = icon_hp_obj.GetComponent<CompImage>();
        if (icon_mana_obj != null)
        {
            icon_mana_img = icon_mana_obj.GetComponent<CompImage>();
        }
        if (icon_stamina_obj != null)
        {
            icon_stamina_img = icon_stamina_obj.GetComponent<CompImage>();
        }
    }

    protected virtual void Start()
    {
        player_obj = GetLinkedObject("player_obj");

        health = GetLinkedObject("health_obj").GetComponent<Hp>();

        stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
        stamina_img = stamina.GetComponent<CompImage>();
        leftamina_img = GetLinkedObject("leftamina_bar").GetComponent<CompImage>();

        mana = GetLinkedObject("mana_obj").GetComponent<Mana>();
        mana_img = mana.GetComponent<CompImage>();
        left_mana_img = GetLinkedObject("letfmana_bar").GetComponent<CompImage>();

        audio = player_obj.GetComponent<CompAudio>();
        movement = player_obj.GetComponent<MovementController>();
        anim_controller = GetComponent<CompAnimation>();
        damage_feedback = player_obj.GetComponent<DamageFeedback>();
        characters_manager = player_obj.GetComponent<CharactersManager>();
        enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();
    }

    public virtual void Update()
    {
        if (position == Position.CURRENT && state != State.GRABBED)
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
        return stamina.CanWasteStamina(value);
    }

    public virtual bool CanWasteMana(float value)
    {
        return mana.CanWasteMana(value);       
    }

    public virtual void DecreaseStamina(float stamina_cost)
    {
        stamina.DecreaseStamina(stamina_cost);
    }

    public virtual void DecreaseMana(float mana_cost)
    {
        mana.DecreaseMana(mana_cost);
    }

    public virtual void DecreaseManaPercentage(float mana_cost)
    {
        mana.DecreaseManaPercentage(mana_cost);
    }

    public void PlayFx(string name)
    {
        audio.PlayEvent(name);
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
        //Button Interaction
        left_button.SetInteractivity(active);
        right_button.SetInteractivity(active);

        //Button Images
        left_button_img.SetRender(active);
        left_button_idle_img.SetRender(active);
        right_button_img.SetRender(active);
        right_button_idle_img.SetRender(active);

        //Cooldown Text Renders
        left_counter.SetRender(active);
        right_counter.SetRender(active);
        sec_counter.SetRender(!active);
    }

    public virtual Vector3 GetFlameBreathPosition(Vector3 player_pos)
    {
        Vector3 ret = new Vector3(player_pos.x, player_pos.y + 4, player_pos.z);
        if (Mathf.Round(curr_forward.z) <= -2.0f)
        {
            ret.x -= 7; ret.z -= 2;
        }
        if (Mathf.Round(curr_forward.z) >= 2.0f)
        {
            ret.x += 7; ret.z += 2;
        }
        if (Mathf.Round(curr_forward.x) >= 2.0f)
        {
            ret.x -= 2; ret.z -= 7;
        }
        if (Mathf.Round(curr_forward.x) <= -2.0f)
        {
            ret.x += 2; ret.z += 7;
        }

        return ret;
    }

    public virtual Vector3 GetSecondaryPosition(Vector3 player_pos)
    {
        Vector3 ret = new Vector3(player_pos.x, player_pos.y - 5.0f, player_pos.z);

        if (position == Position.BEHIND_LEFT)
        {
            if (Mathf.Round(curr_forward.z) <= -2.0f)
            {
                ret.x -= 5.2f;
            }
            else if (Mathf.Round(curr_forward.z) >= 2.0f)
            {
                ret.x += 5.2f;
            }
            else if (Mathf.Round(curr_forward.x) >= 2.0f)
            {
                ret.z -= 5.2f;
            }
            else if (Mathf.Round(curr_forward.x) <= -2.0f)
            {
                ret.z += 5.2f;
            }
            
        }
        else if (position == Position.BEHIND_RIGHT)
        {
            if (Mathf.Round(curr_forward.z) <= -2.0f)
            {
                ret.x += 5.2f;
            }
            else if (Mathf.Round(curr_forward.z) >= 2.0f)
            {
                ret.x -= 5.2f;
            }
            else if (Mathf.Round(curr_forward.x) >= 2.0f)
            {
                ret.z += 5.2f;
            }
            else if (Mathf.Round(curr_forward.x) <= -2.0f)
            {
                ret.z -= 5.2f;
            }
        }

        return ret;
    }

    public virtual Vector3 GetSecondaryForward(Vector3 curr_forward)
    {
        Vector3 ret;

        ret = new Vector3(curr_forward);
        if (position == Position.BEHIND_LEFT)
        {
            if (Mathf.Round(curr_forward.z) <= -2.0f)
            {
                ret.x = 0.18f;
                ret.z = -1.99f;
            }
            else if (Mathf.Round(curr_forward.z) >= 2.0f)
            {
                ret.x = -0.18f;
                ret.z = 1.99f;
            }
            else if (Mathf.Round(curr_forward.x) >= 2.0f)
            {
                ret.x = 1.99f;
                ret.z = 0.18f;
            }
            else if (Mathf.Round(curr_forward.x) <= -2.0f)
            {
                ret.x = -1.99f;
                ret.z = -0.18f;
            }

        }
        else if (position == Position.BEHIND_RIGHT)
        {
            if (Mathf.Round(curr_forward.z) <= -2.0f)
            {
                ret.x = -0.18f;
                ret.z = -1.99f;
            }
            
            else if (Mathf.Round(curr_forward.z) >= 2.0f)
            {
                ret.x = 0.18f;
                ret.z = 1.99f;
            }
            else if (Mathf.Round(curr_forward.x) >= 2.0f)
            {
                ret.x = 1.99f;
                ret.z = -0.18f;
            }
            else if (Mathf.Round(curr_forward.x) <= -2.0f)
            {
                ret.x = -1.99f;
                ret.z = 0.18f;
            }
        }

        return ret;
    }

    public virtual Vector3 GetSecondaryForwardToEnemy(Vector3 player_pos, Vector3 enemy_pos)
    {
        return new Vector3(enemy_pos - player_pos).Normalized;
    }
}