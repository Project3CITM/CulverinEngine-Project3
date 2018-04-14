using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;
using CulverinEditor.Map;
public class EnemySpear_BT : Enemy_BT
{
    public GameObject current_map;
    Movement_Action mov;
    System.Random rand;
    Material enemy_mat_sword;

    public GameObject spear_icon;
    public GameObject spear_name;

    public override void Start()
    {
        this.range = 2;

        current_map = GetLinkedObject("current_map");
        if (current_map == null)
            Debug.Log("[error] Map GameObject in Enemy spear is NULL");

        rand = new System.Random();

        mov = GetComponent<Movement_Action>();
        if (mov == null)
            Debug.Log("[error] Script movement action in Enemy spear is NULL");

        GameObject Temp_go = GetLinkedObject("enemies_manager");

        if (Temp_go == null)
            Debug.Log("[error] Gameobject enemies_manager not found");
        else
        {
            EnemiesManager enemy_manager = Temp_go.GetComponent<EnemiesManager>();

            if (enemy_manager == null)
                Debug.Log("[error] EnemySpear_BT: enemies_manager is not detected");
            else
            {
                enemy_manager.AddLanceEnemy(gameObject);
            }
        }

        enemy_mat_sword = GetMaterialByName("EnemyWithSpear");

        spear_icon = GetLinkedObject("spear_icon");
        spear_name = GetLinkedObject("spear_name");

        base.Start();
        base.DeactivateHUD(spear_icon, spear_name);
    }

    public override void Update()
    {
        enemy_mat_sword.SetFloat("dmg_alpha", dmg_alpha);

        if (hp_timer < hp_timer_total && hud_active == true)
        {
            hp_timer += Time.deltaTime;
        }
        else if (hud_active == true)
        {
            base.DeactivateHUD(spear_icon, spear_name);
        }

        bool attack_ready = attack_timer >= attack_cooldown;

        if (attack_ready && current_action.action_type == Action.ACTION_TYPE.GET_HIT_ACTION)
        {
            Debug.Log("GetHitInterrupted BITCH", Department.IA);
            current_action.Interupt();
        }

        base.Update();
    }

    protected override void InCombatDecesion()
    {
        int tiles_to_player = GetDistanceInRange();

        if (tiles_to_player == 1)
        {
            if (!GetComponent<FacePlayer_Action>().IsFaced())
            {
                current_action.Interupt();
                next_action = GetComponent<FacePlayer_Action>();
                return;
            }

            //We need the direction to know if behind of the enemy there is
            //a tile wakable to make our separate or not.
            Movement_Action.Direction current_dir = GetComponent<Movement_Action>().SetDirection();

            uint next_tile_x = 0;
            uint next_tile_y = 0;

            if (CanISeparate(current_dir, out next_tile_x, out next_tile_y))
            {
                int attack_type_value = rand.Next(1, 10);
                RandomAttack(attack_type_value, next_tile_x, next_tile_y);
                return;
            }
            else
            {
                bool attack_ready = attack_timer >= attack_cooldown;
                if (attack_ready)
                {
                    attack_timer = 0.0f;
                    state = AI_STATE.AI_ATTACKING;
                    SpearAttack_Action action = GetComponent<SpearAttack_Action>();
                    action.IsMeleeAttack(true);
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
        }
        else if (tiles_to_player == 2)
        {
            if (!GetComponent<FacePlayer_Action>().IsFaced())
            {
                current_action.Interupt();
                next_action = GetComponent<FacePlayer_Action>();
                return;
            }

            bool attack_ready = attack_timer >= attack_cooldown;
            if (attack_ready)
            {
                attack_timer = 0.0f;
                state = AI_STATE.AI_ATTACKING;
                SpearAttack_Action action = GetComponent<SpearAttack_Action>();
                action.IsMeleeAttack(false);
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

        if (player_detected == true)
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
            GetComponent<Investigate_Action>().ActionStart();
            current_action = GetComponent<Investigate_Action>();
            return;
        }

        //Patrol
        int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        //Reset event list
        GetComponent<SpearGuard_Listener>().ClearEvents();

        if (my_tile_x != origin_path_x || my_tile_y != origin_path_y)
        {
            GetComponent<Movement_Action>().GoTo(origin_path_x, origin_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
        else
        {
            GetComponent<Movement_Action>().GoTo(end_path_x, end_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
    }

    public override bool ApplyDamage(float damage)
    {
        base.ActivateHUD(spear_icon, spear_name);
        return base.ApplyDamage(damage);
    }

    public override void ChangeTexturesToDamaged()
    {
        GetComponent<CompMaterial>().SetAlbedo("enemy1_Color_Hit.png");
        GetComponent<CompMaterial>().SetNormals("enemy1_Normal_Hit.png");
        GetComponent<CompMaterial>().SetAmbientOcclusion("enemy1_AO_Hit.png");
    }


    bool CanISeparate(Movement_Action.Direction dir, out uint return_next_tile_x, out uint return_next_tile_y)
    {
        //This function is to calculate the tile behind the enemy
        //and see if the enemy should attack or he can separate and then attack
        int tile_x_enemy = GetComponent<Movement_Action>().GetCurrentTileX();
        int tile_y_enemy = GetComponent<Movement_Action>().GetCurrentTileY();

        bool can_i_separate_b = false;

        uint next_tile_x = 0;
        uint next_tile_y = 0;
        //This switch is only to set the direction opposite of the player
        //to see if it is possible walk
        switch (dir)
        {
            case Movement_Action.Direction.DIR_NORTH:
                next_tile_x = (uint)tile_x_enemy;
                next_tile_y = (uint)tile_y_enemy + 1;
                break;
            case Movement_Action.Direction.DIR_WEST:
                next_tile_x = (uint)tile_x_enemy + 1;
                next_tile_y = (uint)tile_y_enemy;
                break;
            case Movement_Action.Direction.DIR_SOUTH:
                next_tile_x = (uint)tile_x_enemy;
                next_tile_y = (uint)tile_y_enemy - 1;
                break;
            case Movement_Action.Direction.DIR_EAST:
                next_tile_x = (uint)tile_x_enemy - 1;
                next_tile_y = (uint)tile_y_enemy;
                break;
        }

        //We want to return if it is possible and also if we already did the calcules
        //to know what tile should we move we can take advantage of

        can_i_separate_b = GetLinkedObject("map").GetComponent<Pathfinder>().IsWalkableTile(next_tile_x, next_tile_y);

        return_next_tile_x = next_tile_x;
        return_next_tile_y = next_tile_y;

        return can_i_separate_b;
    }

    void RandomAttack(int rand_number, uint next_tile_x, uint next_tile_y)
    {
        bool attack_ready = attack_timer >= attack_cooldown;

        if (attack_ready)
        {
            attack_timer = 0.0f;
            state = AI_STATE.AI_ATTACKING;
            SpearAttack_Action action = GetComponent<SpearAttack_Action>();

            int distance = GetDistanceInRange();


            action.IsMeleeAttack(true);


            action.SetDamage(attack_damage);
            current_action = action;
            current_action.ActionStart();

            GetComponent<Separate_Action>().SetTileDestinySeparate(next_tile_x, next_tile_y);
            next_action = GetComponent<Separate_Action>();
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
}



