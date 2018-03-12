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
    protected float current_hp;
    public ENEMY_STATE life_state = ENEMY_STATE.ENEMY_ALIVE;

    public float max_anim_speed = 1.5f;
    public float min_anim_speed = 0.5f;

    public float attack_cooldown = 1.0f;
    public float attack_damage = 1.0f;
    public float damaged_limit = 0.6f;
    protected float attack_timer = 0.0f;
    protected float current_interpolation = 1.0f;
    public uint range = 1;

    public bool engage_combat = false;
    public bool disengage_combat = false;
    protected bool in_combat = false;
    protected bool alive = true;

    public override void Start()
    {
        in_combat = false;
        player = GetLinkedObject("player");
        anim = GetComponent<CompAnimation>();
        current_hp = total_hp;
        Debug.Log("Current HP (Start): " + current_hp);
        Debug.Log("Total HP (Start): " + total_hp);
        //Enemy starts with the attack loaded!
        attack_timer = attack_cooldown * anim_speed;

        Debug.Log("Enemy");
        base.Start();
    }

    public override void Update()
    {
        //Update attack cooldown
        attack_timer += Time.deltaTime;

        base.Update();
    }

    public override void MakeDecision()
    {}

    protected bool ChangeCombatState()
    {
        if (engage_combat == true && in_combat == false)
        {
            Debug.Log("Engage");
            engage_combat = false;
            in_combat = true;
            current_action = GetComponent<Engage_Action>();
            current_action.ActionStart();
            return true;
        }
        else if (disengage_combat == true && in_combat == true)
        {
            Debug.Log("Disengage");
            disengage_combat = false;
            in_combat = false;
            current_action = GetComponent<Disengage_Action>();
            current_action.ActionStart();
            return true;
        }
        return false;
    }

    public virtual void ApplyDamage(float damage)
    {
        if (current_action.action_type == Action.ACTION_TYPE.SHIELD_BLOCK_ACTION) return;

        InterruptAction();

        next_action = GetComponent<GetHit_Action>();
        next_action.SetAnimSpeed(anim_speed);

        current_hp -= damage;

        Debug.Log("Current HP: " + current_hp);

        if (current_hp <= 0)
        {
            anim.SetClipsSpeed(anim_speed);
            alive = false;
            state = AI_STATE.AI_DEAD;
            life_state = ENEMY_STATE.ENEMY_DEAD;
        }
        else if (current_hp < total_hp * damaged_limit)
        {
            life_state = ENEMY_STATE.ENEMY_DAMAGED;

            current_interpolation = current_hp / total_hp;
            anim_speed = min_anim_speed + (max_anim_speed - min_anim_speed) * current_interpolation;
            anim.SetClipsSpeed(anim_speed);
        }
    }

    public void Push(Vector3 dir)
    {
        current_action.Interupt();
        next_action = new PushBack_Action(anim_speed);
    }

    public bool InRange()
    {
        player.GetComponent<MovementController>().GetPlayerPos(out int x, out int y);
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