using CulverinEditor;
using CulverinEditor.Debug;

public class Enemy_BT : BT
{
    public enum ENEMY_STATE
    {
        ENEMY_ALIVE,
        ENEMY_DAMAGED,
        ENEMY_STUNNED,
        ENEMY_DEAD
    }

    public GameObject enemies_manager = null;
    public GameObject player = null;

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

    protected bool in_combat = false;

    public override void Start()
    {
        in_combat = false;
        player = GetLinkedObject("player");
        current_hp = total_hp;
        Debug.Log("Current HP (Start): " + current_hp);
        Debug.Log("Total HP (Start): " + total_hp);
        //Enemy starts with the attack loaded!
        attack_timer = attack_cooldown * anim_speed;
        base.Start();
    }

    public override void Update()
    {
        //Update attack cooldown
        attack_timer += Time.deltaTime;

        if (Input.GetKeyDown(KeyCode.P))
            PushEnemy(new Vector3(0, 0, -1));

        base.Update();
    }

    public override void MakeDecision()
    {
        Debug.Log("Make Decision");
        Debug.Log("Next Action:" + next_action.action_type);

        if (current_hp <= 0)
        {
            current_action = GetComponent<Die_Action>();
            current_action.ActionStart();
            return;
        }

        if (next_action.action_type == Action.ACTION_TYPE.GET_HIT_ACTION || next_action.action_type == Action.ACTION_TYPE.PUSHBACK_ACTION || next_action.action_type == Action.ACTION_TYPE.STUN_ACTION)
        {
            Debug.Log(next_action.action_type);
            current_action = next_action;
            next_action = null_action;
            current_action.ActionStart();
            return;
        }

        if (in_combat)
        {            
            if (next_action.action_type == Action.ACTION_TYPE.DISENGAGE_ACTION)
            {
                Debug.Log("Disengage");
                in_combat = false;
                current_action = GetComponent<Disengage_Action>();
                current_action.ActionStart();
                return;
            }

            InCombatDecesion();
        }
        else
        {
            if (next_action.action_type == Action.ACTION_TYPE.ENGAGE_ACTION)
            {
                Debug.Log("Engage");
                in_combat = true;
                current_action = next_action;
                next_action = null_action;
                current_action.ActionStart();
                return;
            }

            Debug.Log("Out combat decisiom");
            OutOfCombatDecesion();
        }
    }

    protected virtual void InCombatDecesion()
    {
        Debug.Log("In Combat Not Defined");
    }

    protected virtual void OutOfCombatDecesion()
    {
        Debug.Log("Out Of Combat Not Defined");
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
            GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
            state = AI_STATE.AI_DEAD;
            life_state = ENEMY_STATE.ENEMY_DEAD;
        }
        else if (current_hp < total_hp * damaged_limit)
        {
            life_state = ENEMY_STATE.ENEMY_DAMAGED;
            current_interpolation = current_hp / total_hp;
            anim_speed = min_anim_speed + (max_anim_speed - min_anim_speed) * current_interpolation;
            GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
        }
    }

    public void PushEnemy(Vector3 dir)
    {
        current_action.Interupt();

        if (!GetComponent<Movement_Action>().IsWalkable((uint)(GetComponent<Movement_Action>().GetCurrentTileX() + dir.x), (uint)(GetComponent<Movement_Action>().GetCurrentTileY() + dir.z)))
            next_action = new Stun_Action(anim_speed);
        else
            next_action = new PushBack_Action(dir, anim_speed);
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

    public void SetAction(Action.ACTION_TYPE type)
    {
        switch(type)
        {
            case Action.ACTION_TYPE.GET_HIT_ACTION: next_action = GetComponent<GetHit_Action>(); break;
            case Action.ACTION_TYPE.ENGAGE_ACTION: next_action = GetComponent<Engage_Action>(); break;
            case Action.ACTION_TYPE.DISENGAGE_ACTION: next_action = GetComponent<Disengage_Action>(); break;
            case Action.ACTION_TYPE.INVESTIGATE_ACTION: next_action = GetComponent<Investigate_Action>(); break;

            default: Debug.Log("Unknown action"); break;
        }
    }

    public bool InCombat()
    {
        return in_combat;
    }
}