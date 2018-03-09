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

    public GameObject enemies_manager = null;
    public GameObject player = null;

    public float total_hp = 100;
    float current_hp;
    public ENEMY_STATE life_state = ENEMY_STATE.ENEMY_ALIVE;

    public float max_anim_speed = 1.5f;
    public float min_anim_speed = 0.5f;

    public float attack_cooldown = 1.0f;
    public float anim_speed = 1.0f;
    public float attack_damage = 1.0f;
    public float damaged_limit = 0.6f;
    protected float attack_timer = 0.0f;

    public uint range = 1;

    public override void Start()
    {
        current_hp = total_hp;
        //Enemy starts with the attack loaded!
        attack_timer = attack_cooldown * anim_speed;

        base.Start();
    }

    public override void Update()
    {
        //Update attack cooldown
        attack_timer += Time.DeltaTime();

        base.Update();
    }

    public override void MakeDecision()
    {
        Debug.Log("Enemy_BT not decision defined!");
    }

    public void ApplyDamage(float damage)
    {
        current_hp -= damage;

        if (current_hp <= 0)
        {
            //Trigger die animation
            state = AI_STATE.AI_DEAD;
            life_state = ENEMY_STATE.ENEMY_DEAD;
        }
        else if (current_hp < total_hp * damaged_limit)
        {
            life_state = ENEMY_STATE.ENEMY_DAMAGED;

            float current_interpolation = current_hp / total_hp;
            anim_speed = min_anim_speed + (max_anim_speed - min_anim_speed) * current_interpolation;
        }
    }

    public bool InRange()
    {
        //get player tile & calc if in range
        //GetLinkedObject("player").GetComponent<CharactersManager>();
        return true;
    }

}