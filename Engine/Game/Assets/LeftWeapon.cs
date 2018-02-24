using CulverinEditor;
using CulverinEditor.Debug;

/* WEAPON TYPES */
// 1 -> Metal Hand
// 2 -> Sword

/* SOUND FX */
//MetalHit -> 1
//SwordSlash -> 2

public class LeftWeapon : CulverinBehaviour
{
    public GameObject player_obj;
    public CharacterController player;
    public AttackTarget attack_collider;
    public GameObject l_button_obj;
    public CompButton button;
    public CoolDownLeft cd;
    public GameObject enemy_obj;
    public EnemyController enemy;
    //public CompAudio sound_fx;

    public int weapon_type = 0;

    // WEAPON STATS -------------
    public float attack_dmg = 10.0f;
    public float cooldown = 4.0f;
    public float stamina_cost = 30.0f;
    // ---------------------------

    void Start()
    {
        // Link GameObject variables with Scene GameObjects
        player_obj = GetLinkedObject("player_obj");
        l_button_obj = GetLinkedObject("l_button_obj");
        enemy_obj = GetLinkedObject("enemy_obj");
    }

    public void Attack()
    {
        Debug.Log("Attack Left");

        // Gen collider and check for hit with enemy
        //attack_collider = GetComponent<AttackTarget>();
        //if(attack_collider == null)
        //{
        //    Debug.Log("NULL");
        //}
        //else
        //{
        //    Debug.Log("not null");
        //}
        //attack_collider.CheckAttackTarget();

        // Decrease stamina
        player_obj = GetLinkedObject("player_obj");
        player = player_obj.GetComponent<CharacterController>();
        player.DecreaseStamina(stamina_cost);

        Debug.Log("SetAnim");
        // Play specific animation
        SetCurrentAnim();

        Debug.Log("PlayFx");
        // Play specific sound
        PlayFx();

        Debug.Log("Going to hit");
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
        if (player.GetState() == 0)
        {
            if (player.GetCurrentStamina() > stamina_cost)
            {
                l_button_obj = GetLinkedObject("l_button_obj");
                cd = l_button_obj.GetComponent<CoolDownLeft>();
                if (!cd.in_cd)
                {
                    Debug.Log("Going to Attack");

                    // First, OnClick of WeaponController, then, onClick of Cooldown
                    Attack();

                    //// Set Attacking State
                    player_obj = GetLinkedObject("player_obj");
                    player_obj.GetComponent<CharacterController>().anim_controller.SetTransition("ToAttack1");
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
        //OnClick(); // Temp call
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
        Debug.Log("Sword Attack"); 
    }

    public void PlayFx()
    {
        // Reproduce specific audio
        //sound_fx = GetComponent<CompAudio>();
        Debug.Log("Sword Slash");  
    }
}