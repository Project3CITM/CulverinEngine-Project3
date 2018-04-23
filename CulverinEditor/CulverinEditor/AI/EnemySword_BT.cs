using CulverinEditor;
using CulverinEditor.Debug;

public class EnemySword_BT : Enemy_BT
{

    Material enemy_mat_sword;

    public GameObject sword_name;
    public int texture_type = 0;
    public override void Start()
    {
        GameObject Temp_go = GetLinkedObject("enemies_manager");

        if(Temp_go==null)
            Debug.Log("[error] Gameobject enemies_manager not found");
        else
        {
            EnemiesManager enemy_manager = Temp_go.GetComponent<EnemiesManager>();

            if (enemy_manager == null)
                    Debug.Log("[error] EnemySword_BT: enemies_manager is not detected");
            else
            {
                enemy_manager.AddSwordEnemy(gameObject);
            }
        }

        if (texture_type == 0)
        {
            enemy_mat_sword = GetMaterialByName("Alpha1_SwordEnemy_Material_21_04");
        }
        else if (texture_type == 1)
        {
            enemy_mat_sword = GetMaterialByName("Alpha1_SwordEnemy2_Material_21_04");
        }

        sword_name = GetLinkedObject("sword_name");

        dmg_alpha = 0.0f;

        base.Start();
        base.DeactivateHUD(sword_name);
    }

    public override void Update()
    {

        enemy_mat_sword.SetFloat("dmg_alpha", dmg_alpha);

        if (hp_timer < hp_timer_total && hud_active == true)
        {
            hp_timer += Time.deltaTime;
        }
        else if (hud_active == true)
        {
            base.DeactivateHUD(sword_name);
        }

        bool attack_ready = attack_timer >= attack_cooldown;

        if (attack_ready && current_action.action_type == Action.ACTION_TYPE.GET_HIT_ACTION)
        {
            Debug.Log("GetHitInterrupted BITCH", Department.IA);
            current_action.Interupt();
        }

        base.Update();
    }

    protected override void InCombatDecesion()
    {
        //Attack action
        if (InRange())
        {

            if (!GetComponent<Movement_Action>().LookingAtPlayer())
            {
                current_action.Interupt();
                next_action = GetComponent<FacePlayer_Action>();
                return;
            }

            bool attack_ready = attack_timer >= attack_cooldown;

            if (attack_ready)
            {
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
                state = AI_STATE.AI_IDLE;
                current_action = GetComponent<IdleAttack_Action>();
                current_action.ActionStart();
                return;
            }
        }
        else if(player_detected == true && Disable_Movement_Gameplay_Debbuger==false)
        {
            GetComponent<ChasePlayer_Action>().ActionStart();
            current_action = GetComponent<ChasePlayer_Action>();
            return;
        }
    }

    protected override void OutOfCombatDecesion()
    {

        if (Disable_Movement_Gameplay_Debbuger) return;

        //Investigate
        if (heard_something)
        {
            //Investigate
            GetComponent<Investigate_Action>().ActionStart();
            current_action = GetComponent<Investigate_Action>();
            return;
        }

        //Patrol
        int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        //Reset event list
        GetComponent<SwordGuard_Listener>().ClearEvents();

        if (my_tile_x != origin_path_x || my_tile_y != origin_path_y)
        {
            GetComponent<Movement_Action>().GoTo(origin_path_x, origin_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
        else
        {
            GetComponent<Movement_Action>().GoTo(end_path_x, end_path_y);
            GetComponent<Movement_Action>().ActionStart();
            current_action = GetComponent<Movement_Action>();
            return;
        }
    }

    public override bool ApplyDamage(float damage, ENEMY_GET_DAMAGE_TYPE damage_type)
    {
        base.ActivateHUD(sword_name);
        return base.ApplyDamage(damage, damage_type);
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