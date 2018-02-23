using CulverinEditor;
using CulverinEditor.Debug;

//SwordSlash
//MetalHit

public class WeaponController : CulverinBehaviour
{
    public GameObject player_obj;
    public CharacterController player;
    public AttackTarget attack_collider;
    public GameObject button_obj;
    public CompButton button;
    public CoolDown cd;
    public EnemyController enemy;
    public CompAudio sound_fx;

    // WEAPON STATS -------------
    public float attack_dmg = 0.0f;
    public float cooldown = 0.0f;
    public float stamina_cost = 0.0f;
    // ---------------------------

    void Start()
    {
        // Link GameObject variables with Scene GameObjects
        player_obj = GetLinkedObject("player_obj");
        button_obj = GetLinkedObject("button_obj");
    }

    public void Attack()
    {
        // Gen collider and check for hit with enemy
        //...
        if (attack_collider != null)
        {
            attack_collider.CheckAttackTarget();
        }

        // Decrease stamina
        player = player_obj.GetComponent<CharacterController>();
        player.DecreaseStamina(stamina_cost);

        // Play specific animation
        player = player_obj.GetComponent<CharacterController>();
        player.SetAnim("Attack"); // Differentiate between Attack1 and Attack2
        player.SetAnimName("Attack");

        // Reproduce specific audio
        sound_fx = GetComponent<CompAudio>();
        sound_fx.PlayEvent("SwordSlash"); // Differentiate between Sound of Attack1 and Sound of Attack2
    }

    // This method will be called when the associated button to this weapon is pressed
    void OnClick()
    {
        // Check if player has enough stamina to perform its attack
        player = player_obj.GetComponent<CharacterController>();
        if (player.GetCurrentStamina() > stamina_cost)
        {
            cd = button_obj.GetComponent<CoolDown>();
            if (!cd.in_cd)
            {
                // First, OnClick of WeaponController, then, onClick of Cooldown
                Attack();

                // Set Attacking State
                player = player_obj.GetComponent<CharacterController>();
                player.SetState(CharacterController.State.ATTACKING);
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

    public void PrepareAttack()
    {
        button = button_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void AttackHit()
    {
        // Get the GameObject from the collider hit
        if (enemy != null)
        {
            enemy.Hit(attack_dmg);
        }
    }
}