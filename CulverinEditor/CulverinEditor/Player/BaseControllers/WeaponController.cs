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
    public GameObject enemy_obj;

    protected EnemyController enemy;
    protected CompButton button;
    protected CoolDown cd;
    protected CompAnimation animation_weapon;
    protected AIManager enemy_tile;

    // WEAPON STATS -------------
    public float attack_dmg = 10.0f;
    public float stamina_cost = 10.0f;
    //public float cd_time = 3.0f;
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

    protected bool EnemyInFront()
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

        if (position_front_x == enemy_obj.GetComponent<AIManager>().current_x &&
            position_front_y == enemy_obj.GetComponent<AIManager>().current_y) // To change => we have more than one only enemy
        {
            return true;
        }

        return false;
    }
}