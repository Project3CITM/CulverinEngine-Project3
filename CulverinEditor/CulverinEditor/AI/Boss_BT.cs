using CulverinEditor;
using CulverinEditor.Debug;

public class Boss_BT : BT
{
    public enum BOSS_STATE
    {
        BOSS_PHASE1,
        BOSS_PHASE2,
        BOSS_DEAD
    }

    public GameObject enemies_manager = null;
    public GameObject player = null;
    public GameObject mesh = null;

    public float total_hp = 100;
    protected float current_hp;
    public BOSS_STATE life_state = BOSS_STATE.BOSS_PHASE1;

    public float max_anim_speed = 1.5f;
    public float min_anim_speed = 0.5f;

    public float attack_cooldown = 1.0f;
    public float attack_damage = 1.0f;
    public float damaged_limit = 0.6f;
    protected float attack_timer = 0.0f;

    public override void Start()
    {
        //todosforme
        //GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>().AddSwordEnemy();

        Phase1Textures();
        current_action = GetComponent<Idle_Action>();
        GetComponent<CompAnimation>().PlayAnimationNode("Idle");

        base.Start();
    }

    public override void Update()
    {
        base.Update();
    }

    public override void MakeDecision()
    {
        //Debug.Log("InCombatDecesion");
        //Attack action
        /*if (InRange())
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
        }*/
    }

    public bool ApplyDamage(float damage)
    {
        InterruptAction();

        next_action = GetComponent<GetHit_Action>();

        current_hp -= damage;

        current_interpolation = current_hp / total_hp;

        Debug.Log("[error] Current HP: " + current_hp);

        if (current_hp <= 0)
        {
            //GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
            state = AI_STATE.AI_DEAD;
            life_state = BOSS_STATE.BOSS_DEAD;
            next_action = GetComponent<Die_Action>();
            current_action.Interupt();
            
            //todosforme
            //enemies_manager.GetComponent<EnemiesManager>().DeleteSwordEnemy(GetComponent<EnemySword_BT>().gameObject);
          
        }
        else if (life_state != BOSS_STATE.BOSS_PHASE2 && current_hp < total_hp * damaged_limit)
        {
            life_state = BOSS_STATE.BOSS_PHASE2;
            Phase2Textures();
        }

        return true;
    }

    public void Phase2Textures()
    {
        mesh.GetComponent<CompMaterial>().SetAlbedo("enemy1_Color_Hit.png");
        mesh.GetComponent<CompMaterial>().SetNormals("enemy1_Normal_Hit.png");
        mesh.GetComponent<CompMaterial>().SetAmbientOcclusion("enemy1_AO_Hit.png");
    }

    public void Phase1Textures()
    {
        mesh.GetComponent<CompMaterial>().SetAlbedo("enemy1_Color.png");
        mesh.GetComponent<CompMaterial>().SetNormals("enemy1_Normal.png");
        mesh.GetComponent<CompMaterial>().SetAmbientOcclusion("enemy1_AO.png");
    }

    public bool InCombat()
    {
        return !(current_action.action_type == Action.ACTION_TYPE.IDLE_ACTION);
    }

    public void SetAction(Action.ACTION_TYPE type)
    {
        switch (type)
        {
            case Action.ACTION_TYPE.GET_HIT_ACTION: next_action = GetComponent<GetHit_Action>(); break;
            case Action.ACTION_TYPE.ENGAGE_ACTION: next_action = GetComponent<Engage_Action>(); break;

            default: Debug.Log("Unknown action"); break;
        }
    }


    public void Activate()
    {
        next_action = GetComponent<Engage_Action>();
    }
}