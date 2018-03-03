using CulverinEditor;
using CulverinEditor.Debug;

/* WEAPON TYPES */
// 1 -> Metal Hand
// 2 -> Sword

/* SOUND FX */
//MetalHit -> 1
//SwordSlash -> 2

public class WeaponController : CulverinBehaviour
{
    public GameObject player_obj;
    public AttackTarget attack_collider;
    public GameObject button_obj;
    public CompButton button;
    public CoolDown cd;
    public GameObject enemy_obj;
    public EnemyController enemy;
    //public CompAudio sound_fx;

    public virtual void Start()
    {
        // Link GameObject variables with Scene GameObjects
        player_obj = GetLinkedObject("player_obj");
        button_obj = GetLinkedObject("button_obj");
        enemy_obj = GetLinkedObject("enemy_obj");
    }

    public virtual void DoAbility()
    {
        Debug.Log("Doing Ability");
    }

    // This method will be called when the associated button to this weapon is pressed
    public virtual void OnClick()
    {
    }

    public void PrepareAbility()
    {
        Debug.Log("Prepare Ability");
        button_obj = GetLinkedObject("button_obj");
        button = button_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
        //OnClick(); // Temp call
    }
}