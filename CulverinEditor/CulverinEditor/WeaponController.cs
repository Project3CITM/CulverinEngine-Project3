using CulverinEditor;
using CulverinEditor.Debug;

//SwordSlash
//MetalHit

public class WeaponController : CulverinBehaviour
{
    //public GameObject player_obj;
    //public PlayerController player;
    //public GameObject button_obj;
    //public CompButton button;
    //public GameObject enemy_obj;
    //public EnemyController enemy;
    public CompAudio sound_fx;

    // WEAPON STATS -------------
    public float attack_dmg = 0.0f;
    public float cooldown = 0.0f;
    public float stamina_cost = 0.0f;
    // ---------------------------

    void Start()
    {
        //player_obj = GetLinkedObject("player_obj");
        //player = player_obj.GetComponent<PlayerController>();
        //button = button_obj.GetComponent<CompButton>();

        //enemy_obj = GetLinkedObject("enemy_obj");
        //enemy = enemy_obj.GetComponent<EnemyController>();
        sound_fx = GetComponent<CompAudio>();
    }

    void Update()
    {

    }

    public void Attack()
    {
        //Gen collider and check for hit with enemy
        //...
        /*if(hit)
        {
            //Get the GameObject from the collider hit
            enemy = enemy_obj.GetComponent<EnemyController>();
            enemy.Hit(attack_dmg);
        }*/

        //Decrease stamina
        //player.stamina.DecreaseStamina(stamina_cost);

        //Play specific animation
        //...
        //player.SetAttackAnim(true);

        //Reproduce specific audio
        Debug.Log("Attack");
        sound_fx = GetComponent<CompAudio>();
        sound_fx.PlayEvent("SwordSlash");

        //enemy.Hit(10);

        //button.Deactivate();
    }


    void OnClick()
    {
        //if (player.stamina.GetCurrentStamina() > stamina_cost)
        //{
            Attack();

            //Deactive button and apply cooldown
            //button.Deactivate();
            //...

            //player.SetState(PlayerController.State.ATTACKING);
       // }
        //else
        //{
        //    Debug.Log("Not Enough Stamina");
        //}
    }
}