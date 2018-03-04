﻿using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysController : CharacterController
{
    public GameObject daenerys_obj; //Maybe this should be in charactercontroller since we're only having 1 camera which will be the movementcontroller/charactermanager?

    public GameObject rweapon_daenerys_obj;
    public GameObject lweapon_daenerys_obj;
    public GameObject daenerys_icon_obj;
    
    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;

    protected override void Start()
    {
        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        daenerys_obj = GetLinkedObject("daenerys_obj");
        daenerys_icon_obj = GetLinkedObject("daenerys_icon_obj");
        lweapon_daenerys_obj = GetLinkedObject("lweapon_daenerys_obj");
        rweapon_daenerys_obj = GetLinkedObject("rweapon_daenerys_obj");

        //Disable icon
        icon = daenerys_icon_obj.GetComponent<CompImage>();
        icon.SetEnabled(false, daenerys_icon_obj);

        Debug.Log(gameObject.GetName());

        // Start Idle animation
        anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
        anim_controller_left.PlayAnimation("Idle");
        anim_controller_right = rweapon_daenerys_obj.GetComponent<CompAnimation>();
        anim_controller_right.PlayAnimation("Idle");

        ToggleMesh(false);

    }

    public override void ControlCharacter()
    {
        // Debug method to control Hp
        CheckHealth();

        // First check if you are alive
        health = GetLinkedObject("health_obj").GetComponent<Hp>();
        if (health.GetCurrentHealth() > 0)
        {
            // Check if player is moving to block attacks/abilities
            movement = GetLinkedObject("player_obj").GetComponent<MovementController>();
            //if (!movement.IsMoving())
            if(1 == 1)
            {
                /* Player is alive */
                switch (state)
                {
                    case State.IDLE:
                        {
                            //Check For Input + It has to check if she's moving to block attack (¿?)
                            CheckAttack();
                            break;
                        }
                    case State.ATTACKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Attack1"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation until it ends
                                Debug.Log("Daenerys Attacking");
                            }
                            break;
                        }
                    case State.COVER:
                        {
                            //Check for end of the Attack animation
                            anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();

                            if (anim_controller_left.IsAnimationStopped("Cover"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Daenerys Covering");
                            }
                            break;
                        }
                    case State.BLOCKING:
                        {
                            //Check for end of the Attack animation
                            anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Block"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Daenerys Blocking");
                            }
                            break;
                        }
                    case State.HIT:
                        {
                            //Check for end of the Attack animation
                            anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
                            if (anim_controller_left.IsAnimationStopped("Hit"))
                            {
                                state = State.IDLE;
                            }
                            else
                            {
                                // Keep playing specific attack animation  until it ends
                                Debug.Log("Daenerys Hit");
                            }
                            break;
                        }
                    case State.DEAD:
                        {
                            Debug.Log("We are going doown");
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }
        }
    }

    public override void CheckAttack()
    {
        //Left Attack
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            Debug.Log("Daenerys Pressed 1");
            lweapon_daenerys_obj.GetComponent<JaimeWeapon_Left>().PrepareAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Daenerys Pressed 2");
            rweapon_daenerys_obj.GetComponent<JaimeWeapon_Right>().PrepareAbility();
        }
    }

    public override void SecondaryAbility()
    {
        Debug.Log("Daenerys Secondary Ability");
    }


    public override void GetDamage(float dmg)
    {
        Debug.Log("Daenerys Get Damage");

        health = GetLinkedObject("health_obj").GetComponent<Hp>();
        health.GetDamage(dmg);

        // SET HIT ANIMATION
        SetAnimationTransition("ToHit", true);

        SetState(State.HIT);
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller_right = rweapon_daenerys_obj.GetComponent<CompAnimation>();
        anim_controller_right.SetTransition(name, value);

        anim_controller_left = lweapon_daenerys_obj.GetComponent<CompAnimation>();
        anim_controller_left.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active)
    {
        //Update Hp bar
        if (active)
        {
            Debug.Log("Update HP Daenerys");
            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            Debug.Log("Update Stamina Daenerys");
            //Update Stamina
            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            stamina.SetStamina(curr_stamina, max_stamina);
        }

        Debug.Log("Update Child Daenerys");

        //Change current character icon
        icon = daenerys_icon_obj.GetComponent<CompImage>();
        icon.SetEnabled(active, daenerys_icon_obj);
    }

    public override bool IsAnimationStopped(string name)
    {
        anim_controller_right = rweapon_daenerys_obj.GetComponent<CompAnimation>();
        return anim_controller_right.IsAnimationStopped(name);
    }

    public override void ToggleMesh(bool active)
    {
        lweapon_daenerys_obj.GetComponent<CompMesh>().SetEnabled(active, lweapon_daenerys_obj);
        rweapon_daenerys_obj.GetComponent<CompMesh>().SetEnabled(active, rweapon_daenerys_obj);
    }
}