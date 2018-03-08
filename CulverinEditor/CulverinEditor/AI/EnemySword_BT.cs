using CulverinEditor;
using CulverinEditor.Debug;

public class EnemySword_BT : Enemy_BT
{
    public override void Start()
    {
        base.Start();
    }

    public override void Update()
    {
        Debug.Log(attack_timer);
        base.Update();
    }

    public override Action MakeDecision()
    {
        Debug.Log("MAKING ACTION!");
        //Attack action
        if (attack_timer >= (attack_cooldown * attack_speed))
        {
            attack_timer = 0.0f;
            Debug.Log("ATTACK");
            state = AI_STATE.AI_ATTACKING;
            current_action = new SwordAttack_Action(attack_speed);
            current_action.ActionStart();
            return current_action;
        }
        else
        {
            state = AI_STATE.AI_IDLE;
            current_action = idle_action;
            current_action.ActionStart();
            return idle_action;
        }        
    }
}