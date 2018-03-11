using CulverinEditor;
using CulverinEditor.Debug;

public class EnemySword_BT : Enemy_BT
{
    public GameObject mesh;

    public override void Start()
    {
        mesh = GetLinkedObject("mesh");
        mesh.GetComponent<CompAnimation>().PlayAnimation("Patrol");
        mesh.GetComponent<CompAnimation>().SetTransition("ToUnshade");
        //GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>().AddSwordEnemy(gameObject);
        base.Start();
    }

    public override void Update()
    {
        base.Update();
    }

    public override void MakeDecision()
    {
        if (next_action.action_type != Action.ACTION_TYPE.NO_ACTION)
        {
            current_action = next_action;
            current_action.ActionStart();
            next_action = new Action();
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

        if(life_state == ENEMY_STATE.ENEMY_DAMAGED)
        {
            //enemy1_Specular_Hit
            GetComponent<CompMaterial>().SetAlbedo("enemy1_Color_Hit.png");
            GetComponent<CompMaterial>().SetNormals("enemy1_Normal_Hit.png");
            GetComponent<CompMaterial>().SetAmbientOcclusion("enemy1_AO_Hit.png");
        }
    }
}