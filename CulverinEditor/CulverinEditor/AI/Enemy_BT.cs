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
    public GameObject mesh = null;

    public float total_hp = 100;
    protected float current_hp;
    public ENEMY_STATE life_state = ENEMY_STATE.ENEMY_ALIVE;

    public float max_anim_speed = 1.5f;
    public float min_anim_speed = 0.5f;

    public float attack_cooldown = 1.0f;
    public float attack_damage = 1.0f;
    public float damaged_limit = 0.6f;
    protected float attack_timer = 0.0f;
    public int range = 1;

    public bool heard_something = false;

    protected bool in_combat = false;

    //Path Go and Back
    public int origin_path_x;
    public int origin_path_y;

    public int end_path_x;
    public int end_path_y;

    public override void Start()
    {
        in_combat = false;
        player = GetLinkedObject("player");
        current_hp = total_hp;
        //Enemy starts with the attack loaded
        attack_timer = 0.0f;
        mesh = GetLinkedObject("mesh");
        enemies_manager = GetLinkedObject("enemies_manager");
        GetComponent<CompAnimation>().PlayAnimation("Idle");
        //ChangeTexturesToAlive();
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
        if (next_action.action_type == Action.ACTION_TYPE.GET_HIT_ACTION || next_action.action_type == Action.ACTION_TYPE.PUSHBACK_ACTION 
            || next_action.action_type == Action.ACTION_TYPE.STUN_ACTION || next_action.action_type == Action.ACTION_TYPE.SPEARATTACK_ACTION
            || next_action.action_type == Action.ACTION_TYPE.FACE_PLAYER_ACTION || next_action.action_type == Action.ACTION_TYPE.DIE_ACTION
            || next_action.action_type == Action.ACTION_TYPE.SEPARATE_ACTION)
        {
            if (next_action.action_type == Action.ACTION_TYPE.STUN_ACTION)
            {
                life_state = ENEMY_STATE.ENEMY_STUNNED;
            }
            current_action = next_action;
            next_action = null_action;
            current_action.ActionStart();
            return;
        }

        if (in_combat)
        {            
            if (next_action.action_type == Action.ACTION_TYPE.DISENGAGE_ACTION)
            {
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
                in_combat = true;
                current_action = next_action;
                next_action = null_action;
                current_action.ActionStart();
                return;
            }

            OutOfCombatDecesion();
        }
    }

    protected virtual void InCombatDecesion()
    {
        Debug.Log("[error] In Combat Not Defined");
    }

    protected virtual void OutOfCombatDecesion()
    {
        Debug.Log("[error] Out Of Combat Not Defined");
    }

    public virtual bool ApplyDamage(float damage)
    {
        InterruptAction();

        next_action = GetComponent<GetHit_Action>();

        current_hp -= damage;

        current_interpolation = current_hp / total_hp;

        if (current_hp <= 0)
        {
            //GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
            state = AI_STATE.AI_DEAD;
            life_state = ENEMY_STATE.ENEMY_DEAD;
            next_action = GetComponent<Die_Action>();
            current_action.Interupt();
            if (GetComponent<EnemySword_BT>() != null) enemies_manager.GetComponent<EnemiesManager>().DeleteSwordEnemy(GetComponent<EnemySword_BT>().gameObject);
            else if (GetComponent<EnemyShield_BT>() != null) enemies_manager.GetComponent<EnemiesManager>().DeleteShieldEnemy(GetComponent<EnemyShield_BT>().gameObject);
            else if (GetComponent<EnemySpear_BT>() != null) enemies_manager.GetComponent<EnemiesManager>().DeleteLanceEnemy(GetComponent<EnemySpear_BT>().gameObject);
        }
        else if (life_state != ENEMY_STATE.ENEMY_DAMAGED && current_hp < total_hp * damaged_limit)
        {
            life_state = ENEMY_STATE.ENEMY_DAMAGED;
            //ChangeTexturesToDamaged();
        }

        return true;
    }

    public void PushEnemy(Vector3 dir)
    {
        current_action.Interupt();

        if (!GetComponent<Movement_Action>().IsWalkable((uint)(GetComponent<Movement_Action>().GetCurrentTileX() + dir.x), (uint)(GetComponent<Movement_Action>().GetCurrentTileY() + dir.z)))
            next_action = GetComponent<Stun_Action>();
        else
        {
            next_action = GetComponent<PushBack_Action>();
            ((PushBack_Action)next_action).SetPushDirection(dir);
        }
    }

    public bool InRange()
    {
        GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int x, out int y);
        int distance_x = Mathf.Abs(x - GetComponent<Movement_Action>().GetCurrentTileX());
        int distance_y = Mathf.Abs(y - GetComponent<Movement_Action>().GetCurrentTileY());

        if ((distance_x <= range && distance_y == 0) || (distance_y <= range && distance_x == 0))
            return true;
        return false;
    }

    public int GetDistanceInRange()
    {
        int enemy_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int enemy_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        if (GetLinkedObject("player_obj") == null)
        {
            Debug.Log("[error]WILLYYYYYY QUE ESTO ES TO NULL");
        }
        GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int player_tile_x, out int player_tile_y);

        switch (GetComponent<Movement_Action>().GetDirection())
        {
            case Movement_Action.Direction.DIR_WEST:
                if (enemy_tile_x - 2 == player_tile_x 
                    && GetLinkedObject("map").GetComponent<Pathfinder>().IsWalkableTile((uint)enemy_tile_x - 1, (uint)enemy_tile_y) ==true
                    && player_tile_y == enemy_tile_y)
                {
                    Debug.Log("[pink]WEST 2 TILES");
                    return 2;
                }
                else if (enemy_tile_x - 1 == player_tile_x && player_tile_y == enemy_tile_y)
                {
                    Debug.Log("[pink]WEST 1 TILES");
                    return 1;
                }
                break;
            case Movement_Action.Direction.DIR_EAST:
                if (enemy_tile_x + 2 == player_tile_x
                    && GetLinkedObject("map").GetComponent<Pathfinder>().IsWalkableTile((uint)enemy_tile_x + 1, (uint)enemy_tile_y) == true
                    && player_tile_y == enemy_tile_y)
                {
                    Debug.Log("[pink]EAST 2 TILES");
                    return 2;
                }
                else if (enemy_tile_x + 1 == player_tile_x && player_tile_y == enemy_tile_y)
                {
                    Debug.Log("[pink]EAST 1 TILES");
                    return 1;
                }
                break;
            case Movement_Action.Direction.DIR_NORTH:
                if (enemy_tile_y - 2 == player_tile_y 
                    && GetLinkedObject("map").GetComponent<Pathfinder>().IsWalkableTile((uint)enemy_tile_x, (uint)enemy_tile_y - 1) == true
                    && player_tile_x == enemy_tile_x)
                {
                    Debug.Log("[pink]NORTH 2 TILES");
                    return 2;
                }
                else if (enemy_tile_y - 1 == player_tile_y && player_tile_x == enemy_tile_x)
                {
                    Debug.Log("[pink]NORTH 1 TILES");
                    return 1;
                }
                break;
            case Movement_Action.Direction.DIR_SOUTH:
                if (enemy_tile_y + 2 == player_tile_y 
                    && GetLinkedObject("map").GetComponent<Pathfinder>().IsWalkableTile((uint)enemy_tile_x, (uint)enemy_tile_y+1) == true
                    && player_tile_x == enemy_tile_x)
                {
                    Debug.Log("[pink]SOUTH 2 TILES");
                    return 2;
                }
                else if (enemy_tile_y + 1 == player_tile_y && player_tile_x == enemy_tile_x)
                {
                    Debug.Log("[pink]SOUTH 1 TILES");
                    return 1;
                }
                break;
        }
        Debug.Log("[pink]3 TILES");
        return range + 1;
    }

    public void SetAction(Action.ACTION_TYPE type)
    {
        switch(type)
        {
            case Action.ACTION_TYPE.GET_HIT_ACTION: next_action = GetComponent<GetHit_Action>(); break;
            case Action.ACTION_TYPE.ENGAGE_ACTION: next_action = GetComponent<Engage_Action>(); break;
            case Action.ACTION_TYPE.DISENGAGE_ACTION: next_action = GetComponent<Disengage_Action>(); break;
            case Action.ACTION_TYPE.INVESTIGATE_ACTION: next_action = GetComponent<Investigate_Action>(); break;

            default: Debug.Log("[error] Unknown action"); break;
        }
    }

    public bool InCombat()
    {
        return in_combat;
    }

    public virtual void ChangeTexturesToDamaged()
    {
        Debug.Log("[error] Damaged change Textures not defined");
    }

    public virtual void ChangeTexturesToAlive()
    {
        Debug.Log("[error] Alive change Textures not defined");
    }
}