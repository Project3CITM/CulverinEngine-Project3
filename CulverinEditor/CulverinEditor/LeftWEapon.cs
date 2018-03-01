using CulverinEditor;
using CulverinEditor.Debug;

public class LeftWeapon : CulverinBehaviour
{
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
    }

    void Update()
    {
    }

    public void Attack()
    {
        Debug.Log("Attack Left");

        // Decrease stamina -----------
        player = GetLinkedObject("player_obj").GetComponent<CharacterController>();
        player.DecreaseStamina(stamina_cost);

        Debug.Log("Going to hit");

        // Attack the enemy in front of you
        if (EnemyInFront())
        {
            enemy_obj = GetLinkedObject("enemy_obj"); // To change => check the specific enemy in front of you
            enemy = enemy_obj.GetComponent<EnemyController>();
            enemy.Hit(attack_dmg);
        }
    }

    bool EnemyInFront()
    {
        MovementController move = GetLinkedObject("player_obj").GetComponent<MovementController>();
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

        if(position_front_x == GetLinkedObject("enemy_obj").GetComponent<AIManager>().current_x && 
            position_front_y == GetLinkedObject("enemy_obj").GetComponent<AIManager>().current_y) // To change => we have more than one only enemy
        {
            return true;
        }

        return false;
    }

    // This method will be called when the associated button to this weapon is pressed
    void OnClick()
    {
        player = GetLinkedObject("player_obj").GetComponent<CharacterController>();
        // Check if player is in Idle State
        if (player.GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (player.GetCurrentStamina() > stamina_cost)
            {
                l_button_obj = GetLinkedObject("l_button_obj");
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
        l_button_obj = GetLinkedObject("l_button_obj");
        button = l_button_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void SetAttackAnim()
    {
        GameObject leftweapon = GetLinkedObject("lweapon_obj");
        animation_weapon = leftweapon.GetComponent<CompAnimation>(); 
        animation_weapon.SetTransition("ToAttack1");

        GameObject rightweapon = GetLinkedObject("rweapon_obj");
        animation_weapon = rightweapon.GetComponent<CompAnimation>();
        animation_weapon.SetTransition("ToAttack1");
    }

    public void PlayFx()
    {
        GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("SwordSlash");
    }
}