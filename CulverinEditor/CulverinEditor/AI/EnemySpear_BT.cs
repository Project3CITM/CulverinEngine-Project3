
using CulverinEditor;
using CulverinEditor.Debug;

public class EnemySpear_BT : Enemy_BT
{
    public override void Start()
    {
        GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>().AddLanceEnemy(gameObject);
        range = 2;
        base.Start();
    }

    public override void Update()
    {
        base.Update();
    }

    public override void MakeDecision()
    {
        Debug.Log("MAKING LANCE ACTION!");

        if (hit == true)
        {
            hit = false;
            current_action = new GetHit_Action(anim_speed);
            current_action.ActionStart();
            return;
        }
        //Behaviour tree structure
        if (player_detected)
        {
            if (InRange())
            {
                Debug.Log("In Range");
                bool attack_ready = attack_timer >= (attack_cooldown * anim_speed);

                //Attack action
                if (attack_ready && current_action.action_type == Action.ACTION_TYPE.IDLE_ACTION)
                {
                    Debug.Log("Attack");
                    attack_timer = 0.0f;
                    state = AI_STATE.AI_ATTACKING;
                    current_action = new Attack_Action(anim_speed, attack_damage);
                    current_action.ActionStart();
                    return;
                }
                else
                {
                    state = AI_STATE.AI_IDLE;
                    current_action = idle_action;
                    current_action.ActionStart();
                    return;
                }
            }
            else
            {
                Debug.Log("Not In Range");
                // Chase
                Debug.Log("Chase Player Action");
                GetComponent<ChasePlayer_Action>().ActionStart();
                current_action = GetComponent<ChasePlayer_Action>();
                return;
            }
        }
        else
        {
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

            //Patrol
            if (my_tile_x != origin_path_x || my_tile_y != origin_path_y)
            {
                Debug.Log("BT decision move defined!");
                GetComponent<Movement_Action>().GoTo(my_tile_x, my_tile_y, origin_path_x, origin_path_y);
                GetComponent<Movement_Action>().ActionStart();
                current_action = GetComponent<Movement_Action>();
                return;
            }
            else
            {
                Debug.Log("BT decision move defined!");
                GetComponent<Movement_Action>().GoTo(my_tile_x, my_tile_y, end_path_x, end_path_y);
                GetComponent<Movement_Action>().ActionStart();
                current_action = GetComponent<Movement_Action>();
                return;
            }
        }
    }

    public override void ApplyDamage(float damage)
    {
        base.ApplyDamage(damage);

        if (life_state == ENEMY_STATE.ENEMY_DAMAGED)
        {
            //enemy2_Specular_Hit
            GetComponent<CompMaterial>().SetAlbedo("enemy2_Color_Hit.png");
            GetComponent<CompMaterial>().SetNormals("enemy2_Normal_Hit.png");
            GetComponent<CompMaterial>().SetAmbientOcclusion("enemy2_AO_Hit.png");
        }
    }
}

