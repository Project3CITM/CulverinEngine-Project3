using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class LeftWeapon : CulverinBehaviour
{
    public GameObject player_obj;
    public CharacterController player;
    public GameObject l_button_obj;
    public CompButton button;
    public CoolDownLeft cd;
    public GameObject enemy_obj;
    public EnemyController enemy;
    public CompAnimation animation_weapon;
    public AIManager enemy_tile;

    // WEAPON STATS -------------
    public float attack_dmg = 10.0f;
    public float stamina_cost = 30.0f;
    // ---------------------------


    void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        l_button_obj = GetLinkedObject("l_button_obj");
        enemy_obj = GetLinkedObject("enemy_obj");
        player_obj = GetLinkedObject("player_obj");
    }

    void Update()
    {
    }

    public void Attack() //Might be virtual
    {
        Debug.Log("Attack Left");

        // Decrease stamina -----------
        player = player_obj.GetComponent<CharacterController>();
        player.DecreaseStamina(stamina_cost);

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
    void OnClick()
    {
        player = player_obj.GetComponent<CharacterController>();
        // Check if player is in Idle State
        if (player.GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (player.GetCurrentStamina() > stamina_cost)
            {
                cd = l_button_obj.GetComponent<CoolDownLeft>();
                //Check if the ability is not in cooldown
                if (!cd.in_cd)
                {
                    Debug.Log("Going to Attack");

                    // First, OnClick of LeftWeapon, then, onClick of Cooldown
                    Attack();

                    // Set Attacking Animation
                    SetAttackAnim();

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

    public void PrepareAttack()
    {
        Debug.Log("Prepare Attack");
        button = l_button_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void SetAttackAnim()
    {
        animation_weapon = GetLinkedObject("lweapon_obj").GetComponent<CompAnimation>(); 
        animation_weapon.SetTransition("ToAttack1");

        animation_weapon = GetLinkedObject("rweapon_obj").GetComponent<CompAnimation>();
        animation_weapon.SetTransition("ToAttack1");
    }

    public void PlayFx()
    {
        player_obj.GetComponent<CompAudio>().PlayEvent("SwordSlash");
    }
}