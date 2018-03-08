using CulverinEditor;
using CulverinEditor.Debug;

public class Enemy_BT : BT
{
    public enum ENEMY_STATE
    {
        ENEMY_ALIVE,
        ENEMY_DAMAGED,
        ENEMY_DEAD
    }

    public float life = 100;
    float current_life;
    public ENEMY_STATE life_state = ENEMY_STATE.ENEMY_ALIVE;

    public float attack_cooldown = 1.0f;
    public float attack_speed = 1.0f;
    public float damaged_attack_speed = 0.6f;
    public float damaged_limit = 0.6f;
    protected float attack_timer = 0.0f;

    public override void Start()
    {
        current_life = life;
        //Enemy starts with the attack loaded!
        attack_timer = attack_cooldown * attack_speed;

        current_action = MakeDecision();
    }

    public override void Update()
    {
        //Update attack cooldown
        attack_timer += Time.DeltaTime();

        base.Update();
    }

    public override Action MakeDecision()
    {
        Debug.Log("Enemy_BT not decision defined!");
        return null;
    }

    public void ApplyDamage(float damage)
    {
        current_life -= damage;

        if (current_life <= 0)
        {
            //Trigger die animation
            state = AI_STATE.AI_DEAD;
            life_state = ENEMY_STATE.ENEMY_DEAD;
        }
        else if (current_life < life * damaged_limit)
        {
            life_state = ENEMY_STATE.ENEMY_DAMAGED;
            attack_speed = damaged_attack_speed;
        }
    }

}