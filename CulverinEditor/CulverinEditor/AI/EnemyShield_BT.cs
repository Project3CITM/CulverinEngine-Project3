using CulverinEditor;
using CulverinEditor.Debug;

public class EnemyShield_BT : Enemy_BT
{
    public float shield_block_cd = 4.0f;
    public float shield_block_cd_damaged = 2.5f;
    float shield_block_timer = 0.0f;
    
    public override void Start()
    {
        player = GetLinkedObject("player_obj");
        GameObject Temp_go = GetLinkedObject("enemies_manager");

        if (Temp_go == null) Debug.Log("[error]Gameobject enemies_manager not found (EnemyShield_BT)");
        else
        {

            EnemiesManager enemy_manager = Temp_go.GetComponent<EnemiesManager>();

            if (enemy_manager == null) Debug.Log("[error]EnemyShield_BT: enemies_manager is not detected");
            else
            {
                enemy_manager.AddShieldEnemy(gameObject);
            }
        }
        base.Start();
    }

    public override void Update()
    {
        shield_block_timer += Time.deltaTime;

        base.Update();
    }

    protected override void InCombatDecesion()
    {
        //Attack action
        if (InRange())
        {

            if (!GetComponent<FacePlayer_Action>().IsFaced())
            {
                current_action.Interupt();
                next_action = GetComponent<FacePlayer_Action>();
                return;
            }

            bool attack_ready = attack_timer >= (attack_cooldown * anim_speed);

            if (attack_ready)
            {
                Debug.Log("Attack");
                attack_timer = 0.0f;
                state = AI_STATE.AI_ATTACKING;
                Attack_Action action = GetComponent<Attack_Action>();
                action.SetDamage(attack_damage);
                current_action = action;
                current_action.ActionStart();
                return;
            }
            else
            {
                state = AI_STATE.AI_IDLE;
                current_action = GetComponent<IdleAttack_Action>();
                current_action.ActionStart();
                return;
            }
        }
        else if (player_detected == true)
        {
            GetComponent<ChasePlayer_Action>().ActionStart();
            current_action = GetComponent<ChasePlayer_Action>();
            return;
        }
    }

    protected override void OutOfCombatDecesion()
    {
        //Investigate
        if (heard_something)
        {
            //Investigate
            Debug.Log("Investigate");
            GetComponent<Investigate_Action>().ActionStart();
            current_action = GetComponent<Investigate_Action>();
            return;
        }

        //If none of them -> patrol
        int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        //Reset event list
        GetComponent<ShieldGuard_Listener>().ClearEvents();

        //Patrol
        if (my_tile_x != origin_path_x || my_tile_y != origin_path_y)
        {
            Debug.Log("Patrol origin");
            GetComponent<Movement_Action>().GoTo(origin_path_x, origin_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
        else
        {
            Debug.Log("Patrol end");
            GetComponent<Movement_Action>().GoTo(end_path_x, end_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
    }

    public override void ApplyDamage(float damage)
    {
        switch (life_state)
        {
            case ENEMY_STATE.ENEMY_ALIVE:
                if (shield_block_timer >= shield_block_cd)
                {
                    MovementController.Direction player_dir = player.GetComponent<MovementController>().GetPlayerDirection();
                    Movement_Action.Direction enemy_dir = GetComponent<Movement_Action>().SetDirection();
                    if (player_dir == MovementController.Direction.NORTH && enemy_dir == Movement_Action.Direction.DIR_SOUTH ||
                        player_dir == MovementController.Direction.SOUTH && enemy_dir == Movement_Action.Direction.DIR_NORTH ||
                        player_dir == MovementController.Direction.EAST && enemy_dir == Movement_Action.Direction.DIR_WEST ||
                        player_dir == MovementController.Direction.WEST && enemy_dir == Movement_Action.Direction.DIR_EAST)
                    {
                        shield_block_timer = 0.0f;
                        GetComponent<CompAnimation>().SetTransition("ToBlock");
                        GetComponent<CompAudio>().PlayEvent("Enemy3_ShieldBlock");
                        Debug.Log("Attack blocked");
                    }
                    else
                        base.ApplyDamage(damage);
                }
                else
                    base.ApplyDamage(damage);

                break;

            case ENEMY_STATE.ENEMY_DAMAGED:
                if (shield_block_timer >= shield_block_cd_damaged)
                {
                    MovementController.Direction player_dir = player.GetComponent<MovementController>().GetPlayerDirection();
                    Movement_Action.Direction enemy_dir = GetComponent<Movement_Action>().SetDirection();
                    if (player_dir == MovementController.Direction.NORTH && enemy_dir == Movement_Action.Direction.DIR_SOUTH ||
                        player_dir == MovementController.Direction.SOUTH && enemy_dir == Movement_Action.Direction.DIR_NORTH ||
                        player_dir == MovementController.Direction.EAST && enemy_dir == Movement_Action.Direction.DIR_WEST ||
                        player_dir == MovementController.Direction.WEST && enemy_dir == Movement_Action.Direction.DIR_EAST)
                    {
                        shield_block_timer = 0.0f;
                        GetComponent<CompAnimation>().SetTransition("ToBlock");
                        GetComponent<CompAudio>().PlayEvent("Enemy3_ShieldBlock");
                        Debug.Log("Attack blocked");
                    }
                    else
                        base.ApplyDamage(damage);
                }
                else
                    base.ApplyDamage(damage);

                break;

            case ENEMY_STATE.ENEMY_STUNNED:
                base.ApplyDamage(damage);
                break;

            case ENEMY_STATE.ENEMY_DEAD:
            default:
                break;
        }
    }

    public override void ChangeTexturesToDamaged()
    {
        GetComponent<CompMaterial>().SetAlbedo("enemy1_Color_Hit.png");
        GetComponent<CompMaterial>().SetNormals("enemy1_Normal_Hit.png");
        GetComponent<CompMaterial>().SetAmbientOcclusion("enemy1_AO_Hit.png");
    }

    public override void ChangeTexturesToAlive()
    {
        GetComponent<CompMaterial>().SetAlbedo("enemy1_Color.png");
        GetComponent<CompMaterial>().SetNormals("enemy1_Normal.png");
        GetComponent<CompMaterial>().SetAmbientOcclusion("enemy1_AO.png");
    }
}