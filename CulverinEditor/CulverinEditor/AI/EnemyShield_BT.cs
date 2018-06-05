using CulverinEditor;
using CulverinEditor.Debug;

public class EnemyShield_BT : Enemy_BT
{
    public float shield_block_cd = 4.0f;
    public float shield_block_cd_damaged = 2.5f;
    float shield_block_timer = 0.0f;
    Material enemy_mat_sword;

    public GameObject shield_name;
    public int texture_type = 0;

    public float arrow_pen = 0.0f;
    public float tripleArrow_pen = 0.0f;
    public float fireball_pen = 0.0f;
    public float firebreath_pen = 0.0f;
    CompAudio audio_comp;

    public override void Start()
    {
        GameObject Temp_go = GetLinkedObject("enemies_manager");

        if (Temp_go == null)
            Debug.Log("[error] Gameobject enemies_manager not found (EnemyShield_BT)");
        else
        {
            EnemiesManager enemy_manager = Temp_go.GetComponent<EnemiesManager>();

            if (enemy_manager == null)
                Debug.Log("[error] EnemyShield_BT: enemies_manager is not detected");
            else
                enemy_manager.AddShieldEnemy(gameObject);
        }

        if (texture_type == 0)
        {
            enemy_mat_sword = GetMaterialByName("Alpha1_ShieldEnemy_Material_21_04");
        }
        else if(texture_type == 1)
        {
            enemy_mat_sword = GetMaterialByName("Alpha1_ShieldEnemy2_Material_21_04");
        }

        audio_comp = GetComponent<CompAudio>();
        shield_name = GetLinkedObject("shield_name");

        base.Start();
        base.DeactivateHUD(shield_name);
    }

    public override void Update()
    {
        shield_block_timer += Time.deltaTime;

        enemy_mat_sword.SetFloat("dmg_alpha", dmg_alpha);

        if (hp_timer < hp_timer_total && hud_active == true)
        {
            hp_timer += Time.deltaTime;
        }
        else if(hud_active == true)
        {
            base.DeactivateHUD(shield_name);
        }

        bool attack_ready = attack_timer >= attack_cooldown;

        if (attack_ready && current_action.action_type == Action.ACTION_TYPE.GET_HIT_ACTION)
        {
            current_action.Interupt();
        }

        base.Update();
    }

    protected override void InCombatDecesion()
    {
        //Attack action
        if (InRange())
        {
            bool attack_ready = attack_timer >= attack_cooldown;

            if (!GetComponent<Movement_Action>().LookingAtPlayer())
            {
                current_action.Interupt();
                next_action = GetComponent<FacePlayer_Action>();
                return;
            }

            else if (shield_block_timer >= shield_block_cd && player.GetComponent<CharactersManager>().GetCurrentCharacterName() == "Jaime")
            {
                MovementController.Direction player_dir = GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerDirection();
                Movement_Action.Direction enemy_dir = GetComponent<Movement_Action>().SetDirection();
                if (player_dir == MovementController.Direction.NORTH && enemy_dir == Movement_Action.Direction.DIR_SOUTH ||
                    player_dir == MovementController.Direction.SOUTH && enemy_dir == Movement_Action.Direction.DIR_NORTH ||
                    player_dir == MovementController.Direction.EAST && enemy_dir == Movement_Action.Direction.DIR_WEST ||
                    player_dir == MovementController.Direction.WEST && enemy_dir == Movement_Action.Direction.DIR_EAST)
                {
                    next_action = GetComponent<ShieldBlock_Action>();
                }
                return;
            }

            else if (attack_ready)
            {
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
        else if (player_detected == true && Disable_Movement_Gameplay_Debbuger == false)
        {
            if (player.GetComponent<CharactersManager>().GetCurrentCharacterName() == "Jaime")
            {
                GetComponent<ChasePlayer_Action>().SetBlocking(false);
                GetComponent<ChasePlayer_Action>().ActionStart();
                current_action = GetComponent<ChasePlayer_Action>();
            }
            else
            {
                GetComponent<ChasePlayer_Action>().SetBlocking(true);
                GetComponent<ChasePlayer_Action>().ActionStart();
                current_action = GetComponent<ChasePlayer_Action>();
            }

            return;
        }
    }

    protected override void OutOfCombatDecesion()
    {

        if (Disable_Movement_Gameplay_Debbuger) return;

        //Investigate
        if (heard_something)
        {
            //Investigate
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
            GetComponent<Movement_Action>().GoTo(origin_path_x, origin_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action.action_type = Action.ACTION_TYPE.MOVE_ACTION;
            current_action = GetComponent<Movement_Action>();
            return;
        }
        else
        {
            GetComponent<Movement_Action>().GoTo(end_path_x, end_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action.action_type = Action.ACTION_TYPE.MOVE_ACTION;
            current_action = GetComponent<Movement_Action>();
            return;
        }
    }

    public override bool ApplyDamage(float damage, ENEMY_GET_DAMAGE_TYPE damage_type)
    {
        base.ActivateHUD(shield_name);

        switch (life_state)
        {
            case ENEMY_STATE.ENEMY_ALIVE:
                if (GetComponent<ShieldBlock_Action>().IsBlocking() == true || GetComponent<ChasePlayer_Action>().IsBlocking() == true)
                {
                    if (damage_type == ENEMY_GET_DAMAGE_TYPE.FIREWALL)
                    {
                        audio_comp.PlayEvent("Enemy3_Hurt");
                        return base.ApplyDamage(damage, damage_type);
                    }
                    else if(damage_type == ENEMY_GET_DAMAGE_TYPE.FIREBALL)
                    {
                        Debug.Log("FIREBALL BLOCKED!", Department.PLAYER, Color.RED);
                        audio_comp.PlayEvent("Enemy3_Hurt");
                        return base.ApplyDamage(damage * fireball_pen, damage_type);
                    }
                    else if(damage_type == ENEMY_GET_DAMAGE_TYPE.ARROW)
                    {
                        Debug.Log("ARROW BLOCKED!", Department.PLAYER, Color.RED);
                        audio_comp.PlayEvent("Enemy3_Hurt");
                        return base.ApplyDamage(damage * arrow_pen, damage_type);
                    }
                    else if(damage_type == ENEMY_GET_DAMAGE_TYPE.FIREBREATH)
                    {
                        Debug.Log("FIREBREATH BLOCKED!", Department.PLAYER, Color.RED);
                        audio_comp.PlayEvent("Enemy3_Hurt");
                        return base.ApplyDamage(damage * firebreath_pen, damage_type);
                    }
                    else
                    {
                        GetComponent<ShieldBlock_Action>().DecreaseBlockTime();
                        audio_comp.PlayEvent("Enemy3_ShieldBlock");
                        base.UpdateHUD();
                        return false;
                    }
                }
                else
                {
                    return base.ApplyDamage(damage, damage_type);
                }
                break;

            case ENEMY_STATE.ENEMY_DAMAGED:
                if (GetComponent<ShieldBlock_Action>().IsBlocking() == true || GetComponent<ChasePlayer_Action>().IsBlocking() == true)
                {
                    if (damage_type == ENEMY_GET_DAMAGE_TYPE.FIREWALL)
                    {
                        audio_comp.PlayEvent("Enemy3_Hurt");
                        return base.ApplyDamage(damage, damage_type);
                    }
                    else if (damage_type == ENEMY_GET_DAMAGE_TYPE.FIREBALL)
                    {
                        Debug.Log("FIREBALL BLOCKED!", Department.PLAYER, Color.RED);
                        audio_comp.PlayEvent("Enemy3_Hurt");
                        return base.ApplyDamage(damage * fireball_pen, damage_type);
                    }
                    else if (damage_type == ENEMY_GET_DAMAGE_TYPE.ARROW)
                    {
                        Debug.Log("ARROW BLOCKED!", Department.PLAYER, Color.RED);
                        audio_comp.PlayEvent("Enemy3_Hurt");
                        return base.ApplyDamage(damage * arrow_pen, damage_type);
                    }
                    else if (damage_type == ENEMY_GET_DAMAGE_TYPE.FIREBREATH)
                    {
                        Debug.Log("FIREBREATH BLOCKED!", Department.PLAYER, Color.RED);
                        audio_comp.PlayEvent("Enemy3_Hurt");
                        return base.ApplyDamage(damage * firebreath_pen, damage_type);
                    }
                    else
                    {
                        GetComponent<ShieldBlock_Action>().DecreaseBlockTime();
                        GetComponent<CompAnimation>().PlayAnimationNode("Block");
                        audio_comp.PlayEvent("Enemy3_ShieldBlock");
                        base.UpdateHUD();
                        return false;
                    }
                }
                else
                {
                    audio_comp.PlayEvent("Enemy3_Hurt");
                    return base.ApplyDamage(damage, damage_type);
                }
                break;

            case ENEMY_STATE.ENEMY_STUNNED:
                audio_comp.PlayEvent("Enemy3_Hurt");
                return base.ApplyDamage(damage, damage_type);
                break;

            case ENEMY_STATE.ENEMY_DEAD:
            default:
                break;
        }
        return true;
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

    public void ResetShieldBlockTimer()
    {
        shield_block_timer = 0.0f;
    }
}