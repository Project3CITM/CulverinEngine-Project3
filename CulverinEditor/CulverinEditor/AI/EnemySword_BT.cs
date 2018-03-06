using CulverinEditor;

class EnemySword_BT : Enemy_BT
{
    public override Action MakeDecision()
    {
        //Attack action
        attack_timer += Time.DeltaTime();
        if (attack_timer > (attack_cooldown * attack_speed))
        {
            state = AI_STATE.AI_ATTACKING;
            return new SwordAttack_Action(attack_speed);
        }
        
        return null;
    }
}


