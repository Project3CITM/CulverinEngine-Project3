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

    public override void ApplyDamage(float damage)
    {
        base.ApplyDamage(damage);

        if (life_state == ENEMY_STATE.ENEMY_DAMAGED)
        {
            //enemy3_Specular_Hit
            GetComponent<CompMaterial>().SetAlbedo("enemy3_Color_Hit.png");
            GetComponent<CompMaterial>().SetNormals("enemy3_Normal_Hit.png");
            GetComponent<CompMaterial>().SetAmbientOcclusion("enemy3_AO_Hit.png");
        }
    }
}

