using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;
using CulverinEditor.Map;
public class EnemySpear_BT : Enemy_BT
{
    public GameObject current_map;
    Movement_Action mov;
    System.Random rand;

    public override void Start()
    {

        this.range = 2;

        current_map = GetLinkedObject("current_map");
        if (current_map == null) Debug.Log("[error]Map GameObject in Enemy spear is NULL");

        rand = new System.Random();

        mov = GetComponent<Movement_Action>();
        if (mov == null) Debug.Log("[error]Script movement action in Enemy spear is NULL");

        GameObject Temp_go = GetLinkedObject("enemies_manager");

        if (Temp_go == null) Debug.Log("[error]Gameobject enemies_manager not found");
        else
        {

            EnemiesManager enemy_manager = Temp_go.GetComponent<EnemiesManager>();

            if (enemy_manager == null) Debug.Log("[error]EnemySpear_BT: enemies_manager is not detected");
            else
            {
                enemy_manager.AddLanceEnemy(gameObject);
            }
        }
        base.Start();
    }

    public override void Update()
    {
        //Debug.Log("[green] Spear at Tile: " + GetComponent<Movement_Action>().tile.GetTileX() + "," + GetComponent<Movement_Action>().tile.GetTileY());
        base.Update();
    }

    protected override void InCombatDecesion()
    {
        int inrange_i = GetDistanceInRange();
        Debug.Log("[error]Tiles to player:" + inrange_i);
        if (inrange_i == 1)
        {

            if (!GetComponent<FacePlayer_Action>().IsFaced())
            {

                Debug.Log("[pink]FACE PLAYER ACTION IS ACTIVE!!");
                current_action.Interupt();
                next_action = GetComponent<FacePlayer_Action>();
                return;
            }

            //We need the direction to know if behind of the enemy there is
            //a tile wakable to make our separate or not.
            Movement_Action.Direction current_dir = GetComponent<Movement_Action>().SetDirection();

            Debug.Log("[pink]Direction looking:" + current_dir);

            uint next_tile_x = 0;
            uint next_tile_y = 0;

            if (CanISeparate(current_dir, out next_tile_x, out next_tile_y))
            {
                int attack_type_value = rand.Next(1, 10);
                Debug.Log("[green] random: " + attack_type_value);
                RandomAttack(attack_type_value, next_tile_x, next_tile_y);

                //Debug.Log("[error]Separate in random");
                //state = AI_STATE.AI_ATTACKING;
                //Separate_Action action = GetComponent<Separate_Action>();
                ////We set the destiny of the separation
                //action.SetTileDestinySeparate(next_tile_x, next_tile_y);
                //current_action = action;
                //current_action.ActionStart();


                //next_action = GetComponent<SpearAttack_Action>();
                return;
            }
            else
            {
                bool attack_ready = attack_timer >= (attack_cooldown * anim_speed);
                if (attack_ready)
                {
                    Debug.Log("[error]Spear Attack Can't separate");
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
        else if (inrange_i == 2)
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

                Debug.Log("[error]Spear Attack at 2 tiles of distance");
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
                //Debug.Log("IdleAttack");
                state = AI_STATE.AI_IDLE;
                current_action = GetComponent<IdleAttack_Action>();
                current_action.ActionStart();
                return;
            }
        }

        if (player_detected == true)
        {
            //Debug.Log("Chase");
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

        //Patrol
        int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        //Reset event list
        GetComponent<SpearGuard_Listener>().ClearEvents();

        if (my_tile_x != origin_path_x || my_tile_y != origin_path_y)
        {
            Debug.Log("Patrol-origin");
            GetComponent<Movement_Action>().GoTo(origin_path_x, origin_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
        else
        {
            Debug.Log("Patrol-end");
            GetComponent<Movement_Action>().GoTo(end_path_x, end_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
    }

    public override bool ApplyDamage(float damage)
    {
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

        Debug.Log("Tile X pos:" + tile_x_enemy);
        Debug.Log("Tile Y pos:" + tile_y_enemy);

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

        Debug.Log("Tile X pos:" + next_tile_x);
        Debug.Log("Tile Y pos:" + next_tile_y);

        //We want to return if it is possible and also if we already did the calcules
        //to know what tile should we move we can take advantage of

        can_i_separate_b = GetLinkedObject("map").GetComponent<Pathfinder>().IsWalkableTile(next_tile_x, next_tile_y);

        return_next_tile_x = next_tile_x;
        return_next_tile_y = next_tile_y;

        return can_i_separate_b;
    }

    void RandomAttack(int rand_number, uint next_tile_x, uint next_tile_y)
    {
        bool attack_ready = attack_timer >= (attack_cooldown * anim_speed);

        if (attack_ready)
        {
            Debug.Log("[blue] RANDFOM ATTACK");
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
            //Debug.Log("IdleAttack");
            Debug.Log("[blue] RANDFOM IDLE ATTACK");
            state = AI_STATE.AI_IDLE;
            current_action = GetComponent<IdleAttack_Action>();
            current_action.ActionStart();
            return;
        }
    }
}

       
        //{
        //    Debug.Log("[blue] RANDFOM                          SEPARATE");
        //    state = AI_STATE.AI_ATTACKING;
        //    Separate_Action action = GetComponent<Separate_Action>();
        //    //We set the destiny of the separation
        //    action.SetTileDestinySeparate(next_tile_x, next_tile_y);
        //    current_action = action;
        //    current_action.ActionStart();
        //    //next_action = GetComponent<SpearAttack_Action>();
        //    return;
        //}



