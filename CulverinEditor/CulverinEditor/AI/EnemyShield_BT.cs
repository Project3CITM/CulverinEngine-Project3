using CulverinEditor;
using CulverinEditor.Debug;

public class EnemyShield_BT : Enemy_BT
{

    public float shield_block_cooldown = 2.5f;
    float shield_block_timer = 0.0f;

    public override void Start()
    {
        GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>().AddShieldEnemy(gameObject);
        base.Start();
    }

    public override void Update()
    {
        shield_block_timer += Time.deltaTime;

        base.Update();
    }

    public override void MakeDecision()
    {
        Debug.Log("MAKING SHIELD ACTION!");
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
                bool block_ready = shield_block_timer >= shield_block_cooldown;

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
                else if(block_ready)
                {
                    shield_block_timer = 0.0f;
                    state = AI_STATE.AI_BLOCKING;
                    current_action = new ShieldBlock_Action(anim_speed);
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

            //Idle
            if (current_action.action_type != Action.ACTION_TYPE.IDLE_ACTION)
            {
                state = AI_STATE.AI_IDLE;
                current_action = idle_action;
                current_action.ActionStart();
                return;
            }
        }
    }
}

