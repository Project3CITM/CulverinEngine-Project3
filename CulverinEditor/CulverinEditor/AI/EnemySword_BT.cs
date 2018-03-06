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
        base.Update();
    }

    public override Action MakeDecision()
    {
        //Attack action
        //attack_timer += Time.DeltaTime();
        //if (attack_timer > (attack_cooldown * attack_speed))
        {
            Debug.Log("ATTACK");
            state = AI_STATE.AI_ATTACKING;
            current_action = new SwordAttack_Action(attack_speed);
            current_action.ActionStart();
            return current_action;
        }

        return null;
    }
}