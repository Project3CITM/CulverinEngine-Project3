using CulverinEditor;

class Enemy_BT : BT
{
    public float    life = 100;
    float           current_life;

    public float    attack_cooldown = 1.0f;
    public float    attack_speed = 1.0f;
    protected float attack_timer = 0.0f;

    void Start()
    {
        current_life = life;
    }
    
    public void ApplyDamage(float damage)
    {
        current_life -= damage;
        if(current_life <= 0)
        {
            //Trigger die animation
            state = AI_STATE.AI_DEAD;
        }
        else if(current_life < life * 0.5f)
        {
            attack_speed = 0.6f;
        }
    }

}
