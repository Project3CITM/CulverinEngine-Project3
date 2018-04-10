using CulverinEditor;
using CulverinEditor.Debug;

public class Boss_BT : Enemy_BT
{
    public override void Start()
    {
        GameObject Temp_go = GetLinkedObject("enemies_manager");

        if (Temp_go == null)
        {
            Debug.Log("[error]Gameobject enemies_manager not found");
        }
        else
        {
            EnemiesManager enemy_manager = Temp_go.GetComponent<EnemiesManager>();

            if (enemy_manager == null)
            {
                Debug.Log("[error]EnemySword_BT: enemies_manager is not detected");
            }
            else
            {
                //enemy_manager.AddSwordEnemy(gameObject);
            }
        }
        base.Start();
    }

    public override void Update()
    {
        base.Update();
    }

    protected override void InCombatDecesion()
    {
        //Debug.Log("InCombatDecesion");
        //Attack action
        if (InRange())
        {

            if (!GetComponent<FacePlayer_Action>().IsFaced())
            {
                current_action.Interupt();
                next_action = GetComponent<FacePlayer_Action>();
                return;
            }

            bool attack_ready = attack_timer >= (attack_cooldown * anim_speed);

            if (attack_ready)
            {
                Debug.Log("Attack");
                attack_timer = 0.0f;
                state = AI_STATE.AI_ATTACKING;
                Attack_Action action = GetComponent<Attack_Action>();
                action.SetDamage(attack_damage);
                current_action = action;
                current_action.ActionStart();
                return;
            }
            else
            {
                //Debug.Log("IdleAttack");
                state = AI_STATE.AI_IDLE;
                current_action = GetComponent<IdleAttack_Action>();
                current_action.ActionStart();
                return;
            }
        }
        else if (player_detected == true)
        {
            GetComponent<ChasePlayer_Action>().ActionStart();
            current_action = GetComponent<ChasePlayer_Action>();
            return;
        }
    }

    protected override void OutOfCombatDecesion()
    {
        current_action = GetComponent<Idle_Action>();
    }

    public override bool ApplyDamage(float damage)
    {
        return base.ApplyDamage(damage);
    }

    public override void ChangeTexturesToDamaged()
    {
        mesh.GetComponent<CompMaterial>().SetAlbedo("enemy1_Color_Hit.png");
        mesh.GetComponent<CompMaterial>().SetNormals("enemy1_Normal_Hit.png");
        mesh.GetComponent<CompMaterial>().SetAmbientOcclusion("enemy1_AO_Hit.png");
    }

    public override void ChangeTexturesToAlive()
    {
        mesh.GetComponent<CompMaterial>().SetAlbedo("enemy1_Color.png");
        mesh.GetComponent<CompMaterial>().SetNormals("enemy1_Normal.png");
        mesh.GetComponent<CompMaterial>().SetAmbientOcclusion("enemy1_AO.png");
    }
}