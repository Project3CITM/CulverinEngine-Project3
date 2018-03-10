using CulverinEditor;
using CulverinEditor.Debug;

public class Enemy_BT : BT
{
    public enum ENEMY_STATE
    {
        ENEMY_ALIVE,
        ENEMY_DAMAGED,
        ENEMY_DEAD
    }

    public GameObject enemies_manager = null;
    public GameObject player = null;

    CompAnimation anim = null;

    public float total_hp = 100;
    float current_hp;
    public ENEMY_STATE life_state = ENEMY_STATE.ENEMY_ALIVE;

    public float max_anim_speed = 1.5f;
    public float min_anim_speed = 0.5f;

    public float attack_cooldown = 1.0f;
    public float attack_damage = 1.0f;
    public float damaged_limit = 0.6f;
    protected float attack_timer = 0.0f;
    protected float current_interpolation = 1.0f;

    protected bool hit = false;

    public uint range = 1;

    public override void Start()
    {
        player = GetLinkedObject("player");
        anim = GetComponent<CompAnimation>();
        current_hp = total_hp;
        //Enemy starts with the attack loaded!
        attack_timer = attack_cooldown * anim_speed;

        base.Start();
    }

    public override void Update()
    {
        //Update attack cooldown
        attack_timer += Time.deltaTime;

        base.Update();
    }

    public override void MakeDecision()
    {
        Debug.Log("Enemy_BT not decision defined!");
    }

    public void ApplyDamage(float damage)
    {
        if (current_action.action_type == Action.ACTION_TYPE.SHIELD_BLOCK_ACTION) return;

        InterruptAction();

        hit = true;

        current_hp -= damage;

        if (current_hp <= 0)
        {
            anim.SetClipsSpeed(anim_speed);

            if (state == AI_STATE.AI_ATTACKING)
                anim.SetTransition("ToDie");
            else
                anim.SetTransition("ToHitAndDie");

            //Trigger die animation
            state = AI_STATE.AI_DEAD;
            life_state = ENEMY_STATE.ENEMY_DEAD;
        }
        else if (current_hp < total_hp * damaged_limit)
        {
            life_state = ENEMY_STATE.ENEMY_DAMAGED;

            /*GetComponent<CompMaterial>().SetAlbedo();
            GetComponent<CompMaterial>().SetNormals();
            GetComponent<CompMaterial>().SetAmbientOclusion();*/

            current_interpolation = current_hp / total_hp;
            anim_speed = min_anim_speed + (max_anim_speed - min_anim_speed) * current_interpolation;
            anim.SetClipsSpeed(anim_speed);
        }
    }

    public bool InRange()
    {
        int x, y;
        player.GetComponent<MovementController>().GetPlayerPos(out x, out y);
        int distance = Mathf.Abs(x - GetComponent<Movement_Action>().GetCurrentTileX()) + Mathf.Abs(y - GetComponent<Movement_Action>().GetCurrentTileY());
        if (distance <= range)
            return true;
        return false;
    }

    public float GetCurrentInterpolation()
    {
        return current_interpolation;
    }

}