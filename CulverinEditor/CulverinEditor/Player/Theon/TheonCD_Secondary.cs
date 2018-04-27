using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Secondary : CoolDown
{
    public bool theon_dead = false;
    public GameObject theon_secondary_cd_text;

    TheonController theon_controller;

    public override void Start()
    {
        theon_dead = false;

        theon_secondary_cd_text = GetLinkedObject("theon_secondary_cd_text");

        LinkTextTimer(theon_secondary_cd_text);
        ResetTextTimer();

        //Link to the external daenerys_obj 
        theon_controller = GetLinkedObject("theon_obj").GetComponent<TheonController>();

        base.Start();
    }

    public override void Update()
    {
        if (!theon_dead)
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
        if (theon_controller.GetState() == 0
            && characters_manager.changing == false)
        {
            if (in_cd == false)
            {
                if (theon_controller.OnSecondaryClick())
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        button_cd.Deactivate();
        cd_time = theon_controller.sec_ability_cd_time;
        act_time = 0.0f;
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
        theon_dead = true;
        ResetTextTimer();
        fill_image.FillAmount(1.0f);
    }
}