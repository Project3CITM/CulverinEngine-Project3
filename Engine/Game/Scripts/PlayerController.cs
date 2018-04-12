using CulverinEditor;
using CulverinEditor.Debug;

//To check string variables and Log functionality
public class PlayerController : CulverinBehaviour
{
    enum STATES
    {
        ACTIVE = 0,
        TIRED,
    }

    //PLAYER STATS ----
    int hp = 100;
    STATES state = STATES.ACTIVE;
    //StaminaController stamina; -> Script to manage the stamina
    int max_stamina = 5;
    int current_stamina = 0; /* Auxiliar var for now */
    float restore_time = 2.0f; /* Auxiliar var for now */
    //WeaponController current_weapon; -> Script that will handle wich weapon the player is carrying (with its own progression system, stats...)
    //-----------------

    void Start()
    {
        // Refill stamina at max
        current_stamina = max_stamina;
    }

    void Update()
    {
        CheckDamage(); // Auxiliar method

        if (hp > 0)
        {
            CheckStamina(); // Auxiliar method

            /* Player is alive */
            switch (state)
            {
                case STATES.ACTIVE:
                    {
                        CheckAttack();

                        if (current_stamina == 0)
                        {
                            Debug.Log("I'M TIRED");
                            state = STATES.TIRED;
                        }

                        break;
                    }


                case STATES.TIRED:
                    {
                        //Posible tired animations ?¿

                        if (current_stamina > 0) ;
                        {
                            Debug.Log("I'M READY");
                            state = STATES.ACTIVE;
                        }
                        break;
                    }

                default:
                    break;
            }
        }
        else
        {
            Debug.Log("WE ARE GOING DOOOWN");
        }
    }

    void WeakAttack()
    {
        Debug.Log("PAM");
        //stamina.DecreaseStamina(current_weapon.AttackCost(Attack1));
        current_stamina -= 1;
    }

    void StrongAttack()
    {
        Debug.Log("HADOOOUKEN");
        //stamina.DecreaseStamina(current_weapon.AttackCost(Attack2));
        current_stamina -= 2;
    }

    void CheckAttack()
    {
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            WeakAttack();
        }
        else if (Input.GetKeyDown(KeyCode.Num2))
        {

            StrongAttack();
        }
    }

    void CheckStamina()
    {
        restore_time -= Time.DeltaTime();
        if (restore_time <= 0)
        {
            if (current_stamina < max_stamina)
            {
                //Restore 1 stam point
                current_stamina++;
            }
        }
    }

    void CheckDamage()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            Debug.Log("ARGHHH!!");
            hp -= 10;
        }
    }
}