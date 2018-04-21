using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.SceneManagement;
using System;

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
    public GameObject hp_bar_boss = null;
    public GameObject boss_door = null;

    public float total_hp = 100;
    protected float current_hp;
    public BOSS_STATE phase = BOSS_STATE.BOSS_PHASE1;

    public float max_anim_speed = 1.5f;
    public float min_anim_speed = 0.5f;

    //distance Attack
    public float distance_attack_cooldown = 1.0f;

    //AOE Attack
    public float aoe_attack_cooldown = 1.0f;

    //Strong Attack
    public float strong_attack_cooldown = 1.0f;

    //Triple Attack
    public float triple_attack_cooldown = 1.0f;

    public float damaged_limit = 0.6f;
    public float cooldown = 0.0f;

    public float rumble_power = 0.5f;
    public int rumble_time = 200;
    public float rumble_growl_delay = 0.1f;
    private bool rumble = false;
    private float rumble_timer = 0.0f;

    System.Random rand_gen = null;

    private bool boss_active;

    public override void Start()
    {
        hp_bar_boss = GetLinkedObject("hp_bar_boss");
        hp_bar_boss.GetComponent<BossHPBar>().ActivateHPBar(false);
        hp_bar_boss.SetActive(false);
        GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>().AddBoss(gameObject);
        rand_gen = new System.Random();

        //Phase1Textures();
        current_action = GetComponent<Idle_Action>();
        GetComponent<CompAnimation>().PlayAnimation("Idle");

        current_hp = total_hp;
        boss_active = false;

        rumble_timer = 0.0f;
        rumble = false;

        base.Start();
    }

    public override void Update()
    {
        if (rumble == true)
        {
            rumble_timer += Time.deltaTime;
            if (rumble_timer >= rumble_growl_delay)
            {
                Input.RumblePlay(rumble_power, rumble_time);
                rumble_timer = 0.0f;
                rumble = false;
            }
        }
        base.Update();
    }

    public override void MakeDecision()
    {
        if (next_action.action_type != Action.ACTION_TYPE.NO_ACTION)
        {
            current_action = next_action;
            current_action.ActionStart();
            next_action = null_action;

            return;
        }

        if (current_action.action_type != Action.ACTION_TYPE.IDLE_ACTION && current_action.action_type != Action.ACTION_TYPE.DIE_ACTION)
        {
            int distance_x = GetDistanceXToPlayer();
            int distance_y = GetDistanceYToPlayer();
            cooldown -= Time.deltaTime;

            bool in_cd = cooldown <= 0.0;

            if (in_cd == true)
            {
                if (distance_x <= 2 && distance_y == 0 || distance_x == 0 && distance_y <= 2)
                {
                    if (GetComponent<Movement_Action>().LookingAtPlayer() == true)
                    {
                        switch (phase)
                        {
                            case BOSS_STATE.BOSS_PHASE1:
                                if (distance_x == 2 && distance_y == 0 || distance_x == 0 && distance_y == 2)
                                {
                                    int rand = rand_gen.Next(1, 10);

                                    if (rand > 3)
                                    {
                                        //distance attack
                                        Debug.Log("Distance Attack");
                                        current_action = GetComponent<BossAttackSwordDown_Action>();
                                        current_action.ActionStart();
                                        cooldown = distance_attack_cooldown;
                                        return;
                                    }
                                    else
                                    {
                                        GetComponent<InfiniteChasePlayer_Action>().SetChaseRange(1);
                                        current_action = GetComponent<InfiniteChasePlayer_Action>();
                                        current_action.ActionStart();
                                        return;
                                    }
                                }
                                else if (distance_x == 1 && distance_y == 0 || distance_x == 0 && distance_y == 1)
                                {
                                    int rand = rand_gen.Next(1, 10);

                                    if (rand > 3)
                                    {
                                        //AOE attack
                                        Debug.Log("AOE Attack");
                                        current_action = GetComponent<BossWideAttack_Action>();
                                        current_action.ActionStart();
                                        cooldown = aoe_attack_cooldown;
                                        return;
                                    }
                                    else
                                    {
                                        //distance attack
                                        Debug.Log("Distance Attack");
                                        current_action = GetComponent<BossAttackSwordDown_Action>();
                                        current_action.ActionStart();
                                        cooldown = distance_attack_cooldown;
                                        return;
                                    }
                                }
                                break;

                            case BOSS_STATE.BOSS_PHASE2:

                                if (distance_x == 2 && distance_y == 0 || distance_x == 0 && distance_y == 2)
                                {
                                        GetComponent<InfiniteChasePlayer_Action>().SetChaseRange(1);
                                        current_action = GetComponent<InfiniteChasePlayer_Action>();
                                        current_action.ActionStart();
                                        return;
                                }
                                else if (distance_x == 1 && distance_y == 0 || distance_x == 0 && distance_y == 1)
                                {
                                    int rand = rand_gen.Next(1, 10);

                                    if (rand > 3)
                                    {
                                        //Strong attack
                                        Debug.Log("Strong Attack");
                                        current_action = GetComponent<BossBasicStrongAttack_Action>();
                                        current_action.ActionStart();
                                        cooldown = strong_attack_cooldown;
                                        return;
                                    }
                                    else
                                    {
                                        //triple attack
                                        Debug.Log("Triple Attack");
                                        current_action = GetComponent<BossGrabAttack_Action>();
                                        current_action.ActionStart();
                                        cooldown = triple_attack_cooldown;
                                        return;
                                    }
                                }
                                break;
                        }
                    }
                    else
                    {
                        current_action = GetComponent<FacePlayer_Action>();
                        current_action.ActionStart();
                        return;
                    }
                }
                else
                {
                    GetComponent<InfiniteChasePlayer_Action>().SetChaseRange(2);
                    current_action = GetComponent<InfiniteChasePlayer_Action>();
                    current_action.ActionStart();
                    return;
                }
            }
            current_action = GetComponent<IdleAttack_Action>();
            GetComponent<IdleAttack_Action>().ActionStart();
        }
    }

    public bool ApplyDamage(float damage)
    {
        if (boss_active)
        {
            GetComponent<CompAudio>().PlayEvent("BossHurt");

            current_hp -= damage;
            current_interpolation = current_hp / total_hp;

            hp_bar_boss.GetComponent<BossHPBar>().SetHPBar(current_interpolation);

        if (current_hp <= 0)
        {
            state = AI_STATE.AI_DEAD;
            phase = BOSS_STATE.BOSS_DEAD;
            next_action = GetComponent<Die_Action>();
            current_action.Interupt();
            GetComponent<CompAudio>().PlayEvent("BossDeath");
            StatsScore.BossDead();

                hp_bar_boss.GetComponent<BossHPBar>().ActivateHPBar(false);
                hp_bar_boss.SetActive(false);

                //todosforme
                GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>().DeleteBoss();

                SceneManager.LoadNewWalkableMap("Map_Level_2");
                SceneManager.LoadScene("Level 2");
            }
            /*else if (phase != BOSS_STATE.BOSS_PHASE2 && current_hp < total_hp * damaged_limit)
            {
                Debug.Log("[yellow] BOSS PHASE2!!!!!");
                phase = BOSS_STATE.BOSS_PHASE2;
                //Phase2Textures();
            }*/

            return true;
        }
        else
            return false;
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

            default: Debug.Log("[error] Unknown action"); break;
        }
    }

    public void Activate()
    {
        next_action = GetComponent<BossEngage_Action>();
        GetComponent<CompAudio>().PlayEvent("BossGrowl");
        GetLinkedObject("map_obj").GetComponent<LevelMap>().UpdateMap(21, 12, 1);
        rumble = true;
    }

    public int GetDistanceXToPlayer()
    {
        GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int x, out int y);
        int distance_x = Mathf.Abs(x - GetComponent<Movement_Action>().GetCurrentTileX());
        return distance_x;
    }

    public int GetDistanceYToPlayer()
    {
        GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int x, out int y);
        int distance_y = Mathf.Abs(y - GetComponent<Movement_Action>().GetCurrentTileY());
        return distance_y;
    }

    void OnTriggerEnter()
    {
        if (boss_active == false)
        {
            Debug.Log("Boss is triggered");
            GetLinkedObject("boss_door").GetComponent<DoorLevel2>().CloseDoor();
            Activate();
            GetComponent<BossSight>().SetEnabled(false);
            boss_active = true;
        }
    }
}