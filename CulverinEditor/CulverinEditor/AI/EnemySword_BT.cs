using CulverinEditor;
using CulverinEditor.Debug;

public class EnemySword_BT : Enemy_BT
{
    public override void Start()
    {
        GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>().AddSwordEnemy(gameObject);
        base.Start();
    }

    public override void Update()
    {
        base.Update();
    }

    protected override void InCombatDecesion()
    {
        //Attack action
        if (InRange())
        {
            Debug.Log("In Range");
            bool attack_ready = attack_timer >= (attack_cooldown * anim_speed);

            if (attack_ready)
            {
                Debug.Log("Attack");
                attack_timer = 0.0f;
                state = AI_STATE.AI_ATTACKING;
                Attack_Action action = GetComponent<Attack_Action>();
                action.SetDamage(attack_damage);
                action.SetAnimSpeed(anim_speed);
                current_action = action;
                current_action.ActionStart();
                return;
            }
            else
            {
                Debug.Log("Idle");
                state = AI_STATE.AI_IDLE;
                current_action = GetComponent<Idle_Action>();
                current_action.SetAnimSpeed(anim_speed);
                current_action.ActionStart();
                return;
            }
        }
        else
        {
            Debug.Log("Chase");
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

        //Patrol
        if (my_tile_x != origin_path_x || my_tile_y != origin_path_y)
        {
            Debug.Log("Patrol");
            GetComponent<Movement_Action>().GoTo(origin_path_x, origin_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
        else
        {
            Debug.Log("Patrol");
            GetComponent<Movement_Action>().GoTo(end_path_x, end_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
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