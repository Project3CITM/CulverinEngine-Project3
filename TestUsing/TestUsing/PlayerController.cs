using CulverinEditor;
using CulverinEditor.Debug;

public class PlayerController : CulverinBehaviour
{
    public enum State
    {
        IDLE = 0,
        ATTACKING,
    }

    public enum AttackType
    {
        NONE = 0,
        LEFT_ATTACK,
        RIGHT_ATTACK,
    }

    // HP -------------------------
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float calc_hp = 1.0f;
    public CompImage health_bar;
    public GameObject health_obj;
    // ----------------------------

    // STAMINA --------------------
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;
    public float calc_stamina = 1.0f;
    public float regen_stamina = 5f;
    public CompImage stamina_bar;
    public GameObject stamina_obj;
    // ----------------------------

    // LEFT WEAPON ----------------
    public float lweapon_dmg = 50.0f;
    public float lweapon_cd = 2.0f;
    public float lweapon_cost = 50.0f;
    public GameObject lweapon_obj;
    public CompButton lweapon_button;
    // ----------------------------

    // RIGHT WEAPON ----------------
    public float rweapon_dmg = 100.0f;
    public float rweapon_cd = 4.0f;
    public float rweapon_cost = 80.0f;
    public GameObject rweapon_obj;
    public CompButton rweapon_button;
    // ----------------------------

    // ANIMATIONS ------------------------
    public CompAnimation anim_controller;
    public bool anim_ended = false;
    public string actual_anim = "Idle";
    // -----------------------------------

    // COLLISION DETECTION ------------------------
    public CompRigidBody r_body;
    public bool hit = false;
    // -----------------------------------

    // ENEMY ----------------------
    GameObject enemy_obj;
    // ----------------------------

    State state = State.IDLE;
    AttackType last_attack = AttackType.NONE;
    public float cd_time1 = 0.0f;
    public float cd_time2 = 0.0f;


    void Start()
    {
        anim_controller = GetComponent<CompAnimation>();
        actual_anim = "Idle";
        anim_controller.PlayAnimation(actual_anim);
    }

    void Update()
    {
        // First check if you are alive
        if (curr_hp > 0)
        {
            // Regen Stamina
            UpdateStamina();

            // Handle current HP
            UpdateHealth();

            // Check cooldowns
            UpdateCd();

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
                        anim_controller = GetComponent<CompAnimation>();
                        anim_ended = anim_controller.IsAnimationStopped(actual_anim);

                        if(anim_ended)
                        {
                            SetState(State.IDLE);                 
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

    public void SetState(State new_state)
    {
        state = new_state;
    }

    // ATTACK METHODS -----------------------------------------------

    public void CheckAttack()
    {
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            if (lweapon_cost - curr_stamina < 0)
            {
                Attack(AttackType.LEFT_ATTACK, lweapon_cost);
            }
        }
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            if (rweapon_cost - curr_stamina < 0)
            {
                Attack(AttackType.LEFT_ATTACK, rweapon_cost);
            }
        }
    }

    public void Attack(AttackType attack, float cost)
    {
        if (attack == AttackType.LEFT_ATTACK)
        {
            lweapon_button = lweapon_obj.GetComponent<CompButton>();
            lweapon_button.Clicked();

            anim_controller = GetComponent<CompAnimation>();
            actual_anim = "LeftAttack";
            anim_controller.PlayAnimation(actual_anim);

            cd_time1 = lweapon_cd;
        }

        else if (attack == AttackType.LEFT_ATTACK)
        {
            rweapon_button = rweapon_obj.GetComponent<CompButton>();
            rweapon_button.Clicked();

            anim_controller = GetComponent<CompAnimation>();
            actual_anim = "RightAttack";
            anim_controller.PlayAnimation(actual_anim);

            cd_time2 = rweapon_cd;
        }

        curr_stamina -= cost;
        calc_stamina = curr_stamina / max_stamina;
        stamina_bar = stamina_obj.GetComponent<CompImage>();
        stamina_bar.FillAmount(calc_stamina);

        last_attack = attack;
        SetState(State.ATTACKING);
    }

    public void UpdateCd()
    {
        if(cd_time1 > 0.0f)
        {
            cd_time1 -= Time.DeltaTime();

            if(cd_time1 <= 0.0f)
            {
                lweapon_button = lweapon_obj.GetComponent<CompButton>();
                lweapon_button.Activate();
            }
        }
        if (cd_time2 > 0.0f)
        {
            cd_time2 -= Time.DeltaTime();

            if (cd_time2 <= 0.0f)
            {
                rweapon_button = rweapon_obj.GetComponent<CompButton>();
                rweapon_button.Activate();
            }
        }
    }

    // --------------------------------------------------------------


    // STAMINA METHODS ----------------------------------------------

    public void UpdateStamina()
    {
        if (curr_stamina < max_stamina)
        {
            curr_stamina += regen_stamina;
            if(curr_stamina> max_stamina)
            {
                curr_stamina = max_stamina;
            }
            calc_stamina = curr_stamina / max_stamina;
            stamina_bar = stamina_obj.GetComponent<CompImage>();
            stamina_bar.FillAmount(calc_stamina);
        }
    }

    // --------------------------------------------------------------

    // HEALTH METHODS ----------------------------------------------

    public void UpdateHealth()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            GetDamage(10.0f);
        }
        if (Input.GetKeyDown(KeyCode.M))
        {
            GetDamage(-10.0f);
        }
    }

    public void GetDamage(float dmg)
    {
        curr_hp -= dmg;

        if (curr_hp < 0)
        {
            curr_hp = 0;
        }
        calc_hp = curr_stamina / max_stamina;
        health_bar = health_obj.GetComponent<CompImage>();
        health_bar.FillAmount(calc_hp);
    }

    // --------------------------------------------------------------
}