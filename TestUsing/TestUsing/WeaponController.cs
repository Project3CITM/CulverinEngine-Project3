using CulverinEditor;
using CulverinEditor.Debug;

//SwordSlash
//MetalHit

public class WeaponController : CulverinBehaviour
{
    public GameObject player_obj;
    public PlayerController player;
    public AttackTarget attack_collider;
    public GameObject button_obj;
    public CompButton button;
    public EnemyController enemy;
    public CompAudio sound_fx;

    // WEAPON STATS -------------
    public float attack_dmg = 0.0f;
    public float cooldown = 0.0f;
    public float stamina_cost = 0.0f;
    // ---------------------------

    void Start()
    {
        player = player_obj.GetComponent<PlayerController>();
        attack_collider = GetComponent<AttackTarget>();
        button = button_obj.GetComponent<CompButton>();
        sound_fx = GetComponent<CompAudio>();
    }

    public void Attack()
    {
        //Gen collider and check for hit with enemy
        //...
        if (attack_collider != null)
        {
            attack_collider.CheckAttackTarget();
        }

        //Decrease stamina
        player.stamina.DecreaseStamina(stamina_cost);

        //Play specific animation
        //...
        player.SetAttackAnim(true);

        //Reproduce specific audio
        sound_fx.PlayEvent("Attack");
    }

    public void AttackHit()
    {
        //Get the GameObject from the collider hit
        if (enemy != null)
        {
            enemy.Hit(attack_dmg);
        }
    }


    void OnClick()
    {
        if (player.stamina.GetCurrentStamina() > stamina_cost)
        {
            Attack();

            //Deactive button and apply cooldown
            button.Deactivate();
            //...

            player.SetState(PlayerController.State.ATTACKING);
        }
        else
        {
            Debug.Log("Not Enough Stamina");
        }
    }
}