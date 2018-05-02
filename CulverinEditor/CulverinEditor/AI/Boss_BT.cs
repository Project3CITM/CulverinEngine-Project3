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
    public float charge_attack_cooldown = 1.0f;

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

    private bool boss_dead = false;
    public float boss_dead_delay_time = 5.0f;
    private float boss_dead_delay_timer = 0.0f;

    private Movement_Action move;
    private CompAudio audio;
    private BossAttackSwordDown_Action sword_down_attack_action;
    private BossWideAttack_Action wide_attack_action;
    private InfiniteChasePlayer_Action infinite_chase_action;
    private FacePlayer_Action face_player_action;
    private IdleAttack_Action idle_attack_action;
    private GetHit_Action get_hit_action;
    private Engage_Action engage_action;

    private BossHPBar boss_hp_bar_comp;
    private CompMaterial material;
    private EnemiesManager enemies_manager_obj;
    private MovementController movement_controller;

    public override void Start()
    {
        move = GetComponent<Movement_Action>();
        audio = GetComponent<CompAudio>();
        sword_down_attack_action = GetComponent<BossAttackSwordDown_Action>();
        wide_attack_action = GetComponent<BossWideAttack_Action>();
        infinite_chase_action = GetComponent<InfiniteChasePlayer_Action>();
        face_player_action = GetComponent<FacePlayer_Action>();
        idle_attack_action = GetComponent<IdleAttack_Action>();
        get_hit_action = GetComponent<GetHit_Action>();
        engage_action = GetComponent<Engage_Action>();

        hp_bar_boss = GetLinkedObject("hp_bar_boss");
        boss_hp_bar_comp = hp_bar_boss.GetComponent<BossHPBar>();
        material = mesh.GetComponent<CompMaterial>();
        enemies_manager_obj = GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>();
        movement_controller = GetLinkedObject("player_obj").GetComponent<MovementController>();


        boss_hp_bar_comp.ActivateHPBar(false);
        hp_bar_boss.SetActive(false);
        enemies_manager_obj.AddBoss(gameObject);
        rand_gen = new System.Random();

        //Phase1Textures();
        current_action = GetComponent<Idle_Action>();
        GetComponent<CompAnimation>().PlayAnimation("Idle");

        current_hp = total_hp;
        boss_active = false;

        rumble_timer = 0.0f;
        rumble = false;

        boss_dead = false;
        boss_dead_delay_timer = 0.0f;

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

        if (boss_dead)
        {
            boss_dead_delay_timer += Time.deltaTime;
            if (boss_dead_delay_timer >= boss_dead_delay_time)
            {
                if (SceneManager.CheckMultiSceneReady())
                    SceneManager.RemoveSecondaryScene();
                SceneManager.LoadScene("ScoreMenu");
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
                    if (move.LookingAtPlayer() == true)
                    {
                        if (distance_x == 2 && distance_y == 0 || distance_x == 0 && distance_y == 2)
                        {
                            int rand = rand_gen.Next(1, 10);

                            if (rand > 3)
                            {
                                //distance attack
                                Debug.Log("Distance Attack");
                                current_action = sword_down_attack_action;
                                current_action.ActionStart();
                                cooldown = distance_attack_cooldown;
                                return;
                            }
                            else
                            {
                                infinite_chase_action.SetChaseRange(1);
                                current_action = infinite_chase_action;
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
                                current_action = wide_attack_action;
                                current_action.ActionStart();
                                cooldown = aoe_attack_cooldown;
                                return;
                            }
                            else
                            {
                                //distance attack
                                Debug.Log("Distance Attack");
                                current_action = sword_down_attack_action;
                                current_action.ActionStart();
                                cooldown = distance_attack_cooldown;
                                return;
                            }
                        } 
                    }
                    else
                    {
                        current_action = face_player_action;
                        current_action.ActionStart();
                        return;
                    }
                }
                else
                {
                    if (phase == BOSS_STATE.BOSS_PHASE2)
                    {
                        /*Debug.Log("Charge Attack");
                        current_action = GetComponent<ChargeAttack_Action>();
                        current_action.ActionStart();
                        cooldown = charge_attack_cooldown;
                        return;*/

                        infinite_chase_action.SetChaseRange(2);
                        current_action = infinite_chase_action;
                        current_action.ActionStart();
                        return;
                    }
                    else
                    {
                        infinite_chase_action.SetChaseRange(2);
                        current_action = infinite_chase_action;
                        current_action.ActionStart();
                        return;
                    }
                }
            }
            current_action = idle_attack_action;
            idle_attack_action.ActionStart();
        }
    }

    public bool ApplyDamage(float damage)
    {
        if (boss_active)
        {
            audio.PlayEvent("BossHurt");

            current_hp -= damage;
            current_interpolation = current_hp / total_hp;

            boss_hp_bar_comp.SetHPBar(current_interpolation);

            if (current_hp <= 0)
            {
                state = AI_STATE.AI_DEAD;
                phase = BOSS_STATE.BOSS_DEAD;
                next_action = GetComponent<Die_Action>();
                current_action.Interupt();
                audio.PlayEvent("BossDeath");
                StatsScore.BossDead();

                boss_hp_bar_comp.ActivateHPBar(false);
                hp_bar_boss.SetActive(false);

                //todosforme
                enemies_manager_obj.DeleteBoss();

                boss_dead = true;
            }
            else if (phase != BOSS_STATE.BOSS_PHASE2 && current_hp < total_hp * damaged_limit)
            {
                Debug.Log("BOSS PHASE2!!!!!");
                phase = BOSS_STATE.BOSS_PHASE2;

                //Change the boss values
                //move.max_vel = 80;
                //Phase2Textures();
            }

            Debug.Log(current_hp);

            return true;
        }
        else
            return false;
    }

    public void Phase2Textures()
    {
        material.SetAlbedo("enemy1_Color_Hit.png");
        material.SetNormals("enemy1_Normal_Hit.png");
        material.SetAmbientOcclusion("enemy1_AO_Hit.png");
    }

    public void Phase1Textures()
    {
        material.SetAlbedo("enemy1_Color.png");
        material.SetNormals("enemy1_Normal.png");
        material.SetAmbientOcclusion("enemy1_AO.png");
    }

    public bool InCombat()
    {
        return !(current_action.action_type == Action.ACTION_TYPE.IDLE_ACTION);
    }

    public void SetAction(Action.ACTION_TYPE type)
    {
        switch (type)
        {
            case Action.ACTION_TYPE.GET_HIT_ACTION: next_action = get_hit_action; break;
            case Action.ACTION_TYPE.ENGAGE_ACTION: next_action = engage_action; break;

            default: Debug.Log("[error] Unknown action"); break;
        }
    }

    public void Activate()
    {
        next_action = GetComponent<BossEngage_Action>();
        audio.PlayEvent("BossGrowl");
        GetLinkedObject("map_obj").GetComponent<LevelMap>().UpdateMap(21, 12, 1);
        rumble = true;
    }

    public int GetDistanceXToPlayer()
    {
        movement_controller.GetPlayerPos(out int x, out int y);
        int distance_x = Mathf.Abs(x - move.GetCurrentTileX());
        return distance_x;
    }

    public int GetDistanceYToPlayer()
    {
        movement_controller.GetPlayerPos(out int x, out int y);
        int distance_y = Mathf.Abs(y - move.GetCurrentTileY());
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