using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class LeftWeaponJaime : WeaponController
{
    GameObject jaime_obj;
    public JaimeController character;
    public CompAnimation animation_weapon;
    public AIManager enemy_tile;

    // WEAPON STATS -------------
    public float attack_dmg = 10.0f;
    public float stamina_cost = 30.0f;
    // ---------------------------


    public override void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        jaime_obj = GetLinkedObject("jaime_obj");
        button_obj = GetLinkedObject("button_obj");
        enemy_obj = GetLinkedObject("enemy_obj");
    }

    void Update()
    {
    }

    public override void DoAbility() //Might be virtual
    {
        Debug.Log("Attack Left");

        // Decrease stamina -----------
        character = jaime_obj.GetComponent<JaimeController>();
        character.DecreaseStamina(stamina_cost);

        Debug.Log("Going to hit");

        // Attack the enemy in front of you
        if (EnemyInFront())
        {
            // To change => check the specific enemy in front of you
            enemy = enemy_obj.GetComponent<EnemyController>();
            enemy.Hit(attack_dmg);
        }
    }

    bool EnemyInFront()
    {
        MovementController move = player_obj.GetComponent<MovementController>();
        int direction = (int)move.curr_dir;
        int position_front_x = move.curr_x;
        int position_front_y = move.curr_y;

        switch (direction)
        {
            case (int)MovementController.Direction.NORTH:
                {
                    position_front_y -= 1;
                    break;
                }

            case (int)MovementController.Direction.SOUTH:
                {
                    position_front_y += 1;
                    break;
                }

            case (int)MovementController.Direction.EAST:
                {
                    position_front_x += 1;
                    break;
                }

            case (int)MovementController.Direction.WEST:
                {
                    position_front_x -= 1;
                    break;
                }

             default:
                {
                    break;
                }

        }

        if(position_front_x == enemy_obj.GetComponent<AIManager>().current_x && 
            position_front_y == enemy_obj.GetComponent<AIManager>().current_y) // To change => we have more than one only enemy
        {
            return true;
        }

        return false;
    }

    // This method will be called when the associated button to this weapon is pressed
    public override void OnClick()
    {
        character = jaime_obj.GetComponent<JaimeController>();
        // Check if player is in Idle State
        if (character.GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (character.GetCurrentStamina() > stamina_cost)
            {
                cd = button_obj.GetComponent<CoolDown>();
                //Check if the ability is not in cooldown
                if (!cd.in_cd)
                {
                    Debug.Log("Going to Attack");

                    // First, OnClick of LeftWeapon, then, onClick of Cooldown
                    Attack();

                    // Set Attacking Animation
                    character.SetAnimationTransition("ToAttack1", true);

                    // Play the Sound FX
                    PlayFx();
                }
                else
                {
                    Debug.Log("Ability in CD");
                }
            }
            else
            {
                Debug.Log("Not Enough Stamina");
            }
        }
    }

    public void Attack() //Might be virtual
    {
        Debug.Log("Attack Left");

        // Decrease stamina -----------

        character = player_obj.GetComponent<JaimeController>();
        character.DecreaseStamina(stamina_cost);

        Debug.Log("Going to hit");

        // Attack the enemy in front of you
        if (EnemyInFront())
        {
            // To change => check the specific enemy in front of you
            enemy = enemy_obj.GetComponent<EnemyController>();
            enemy.Hit(attack_dmg);
        }
    }

    public void PlayFx()
    {
        player_obj.GetComponent<CompAudio>().PlayEvent("SwordSlash");
    }
}