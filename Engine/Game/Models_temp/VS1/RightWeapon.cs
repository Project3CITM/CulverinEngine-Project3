using CulverinEditor;
using CulverinEditor.Debug;

/* WEAPON TYPES */
// 1 -> Metal Hand
// 2 -> Sword

/* SOUND FX */
//MetalHit -> 1
//SwordSlash -> 2

public class RightWeapon : CulverinBehaviour
{
    public GameObject rweapon_obj_anim;
    public GameObject player_obj;
    public CharacterController player;
    public AttackTarget attack_collider;
    public GameObject r_button_obj;
    public CompButton button;
    public CoolDownRight cd;
    public GameObject enemy_obj;
    public EnemyController enemy;
    public CompAnimation animation_weapon;
    //public CompAudio sound_fx;

    // WEAPON STATS -------------
    public float attack_dmg = 0.0f;
    public float cooldown = 2.0f;
    public float stamina_cost = 10.0f;
    // ---------------------------

    int step_counter = -1;

    void Start()
    {

        step_counter = -1;
    }

    public void Attack()
    {
        Debug.Log("Attack Right");

        // Decrease stamina
        player_obj = GetLinkedObject("player_obj");
        player = player_obj.GetComponent<CharacterController>();
        player.DecreaseStamina(stamina_cost);

        // Play specific sound
        PlayFx();

        Debug.Log("Going to block");

        // Temp Method
        enemy_obj = GetLinkedObject("enemy_obj");
        enemy = enemy_obj.GetComponent<EnemyController>();
        enemy.Hit(attack_dmg);
    }

    // This method will be called when the associated button to this weapon is pressed
    void OnClick()
    {
        // Check if player has enough stamina to perform its attack
        player_obj = GetLinkedObject("player_obj");
        player = player_obj.GetComponent<CharacterController>();
        if(player.GetState() == 0)
        {
            if (player.GetCurrentStamina() > stamina_cost)
            {
                r_button_obj = GetLinkedObject("r_button_obj");
                cd = r_button_obj.GetComponent<CoolDownRight>();
                if (!cd.in_cd)
                {
                    Debug.Log("Going to Attack");

                    // First, OnClick of WeaponController, then, onClick of Cooldown
                    Attack();

                    // Set Animation
                    rweapon_obj_anim = GetLinkedObject("rweapon_obj_anim");
                    animation_weapon = rweapon_obj_anim.GetComponent<CompAnimation>();
                    animation_weapon.SetTransition("ToCover");

                    GameObject lweapon_obj_anim = GetLinkedObject("lweapon_obj_anim");
                    animation_weapon = lweapon_obj_anim.GetComponent<CompAnimation>();
                    animation_weapon.SetTransition("ToCover");
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
        Debug.Log("Prepare Block");
        r_button_obj = GetLinkedObject("r_button_obj");
        button = r_button_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void AttackHit()
    {
        // Get the GameObject from the collider hit
        if (enemy_obj != null)
        {
            enemy_obj = GetLinkedObject("enemy_obj");
            enemy = enemy_obj.GetComponent<EnemyController>();
            enemy.Hit(attack_dmg);
        }

    }

    public void SetCurrentAnim()
    {
        //player = player_obj.GetComponent<CharacterController>();
        Debug.Log("Hand Block");
    }

    public void PlayFx()
    {
        // Reproduce specific audio
        //sound_fx = GetComponent<CompAudio>();
        Debug.Log("Metal Hit");
    }
}
