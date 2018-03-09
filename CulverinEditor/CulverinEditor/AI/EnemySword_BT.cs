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

    public override void MakeDecision()
    {        
        Debug.Log("MAKING ACTION!");
        bool attack_ready = attack_timer >= (attack_cooldown * attack_speed);
        
        //Attack action
        if(attack_ready && current_action.action_type == Action.ACTION_TYPE.IDLE_ACTION)
        { 
            attack_timer = 0.0f;
            state = AI_STATE.AI_ATTACKING;
            current_action = new Attack_Action(attack_speed);
            current_action.ActionStart();
        }
        //Idle action
        else if(current_action.action_type != Action.ACTION_TYPE.IDLE_ACTION)
        {
            state = AI_STATE.AI_IDLE;
            current_action = idle_action;
            current_action.ActionStart();
        }
    }
}