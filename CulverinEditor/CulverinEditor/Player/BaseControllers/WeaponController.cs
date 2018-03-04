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
    public GameObject enemy_obj;

    protected EnemyController enemy;
    protected CompButton button;
    protected CoolDown cd;
    protected CompAnimation animation_weapon;
    protected AIManager enemy_tile;

    // WEAPON STATS -------------
    public float attack_dmg = 10.0f;
    public float stamina_cost = 10.0f;
    // ---------------------------

    public virtual void Start()
    {
        Debug.Log("Start Weapon");
    }

    // This method will be called when the associated button to this weapon is pressed
    public virtual void OnClick()
    {
        Debug.Log("On Click Weapon");
    }

    public virtual void PrepareAbility()
    {
        Debug.Log("Prepare Ability Weapon");
    }

    public virtual void DoAbility()
    {
        Debug.Log("Do Ability Weapon");
    }

    public virtual void PlayFx()
    {
        Debug.Log("Play Fx Weapon");
    }
}