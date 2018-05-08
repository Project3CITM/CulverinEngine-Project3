using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Secondary : CoolDown
{
    public bool jaime_dead = false;
    public GameObject jaime_secondary_cd_text;

    JaimeController jaime_controller;

    public override void Start()
    {
        jaime_dead = false;

        jaime_secondary_cd_text = GetLinkedObject("jaime_secondary_cd_text");

        LinkTextTimer(jaime_secondary_cd_text);
        ResetTextTimer();

        //Link to the external daenerys_obj 
        jaime_controller = GetLinkedObject("jaime_obj").GetComponent<JaimeController>();

        base.Start();
    }

    public override void Update()
    {
        if (!jaime_dead)
        {
            base.Update();

            //Manage Seconds Counter
            if (in_cd)
            {
                ManageTextTimer();
            }

            //Reset Seconds Counter
            if (reset_timer)
            {
                ResetTextTimer();
                reset_timer = false;
            }
        }
    }

    public override void OnClick()
    {
        if (jaime_controller.GetState() == 0
            && characters_manager.changing == false)
        {
            if (in_cd == false)
            {
                if (jaime_controller.OnSecondaryClick())
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        button_cd.Deactivate();
        act_time = 0.0f;
        cd_time = jaime_controller.sec_ability_cd_time;
        prev_seconds = 1000;
        in_cd = true;

        //SET COOLDOWN TO 1 SECOND
        if (characters_manager.god_mode ||
            characters_manager.no_cds)
        {
            cd_time = 1.0f;
        }
    }

    public void Die()
    {
        jaime_dead = true;
        ResetTextTimer();
        fill_image.FillAmount(1.0f);
    }
}