using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Secondary : CoolDown
{
    public bool daenerys_dead = false;
    public GameObject daenerys_secondary_cd_text;

    DaenerysController daenerys_controller;

    public override void Start()
    {
        daenerys_dead = false;

        daenerys_secondary_cd_text = GetLinkedObject("daenerys_secondary_cd_text");

        LinkTextTimer(daenerys_secondary_cd_text);
        ResetTextTimer();

        //Link to the external daenerys_obj 
        daenerys_controller = GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>();

        base.Start();
    }

    public override void Update()
    {
        if (!daenerys_dead)
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
        if (daenerys_controller.GetState() == 0
            && characters_manager.changing == false)
        {
            if (in_cd == false)
            {
                if (daenerys_controller.OnSecondaryClick())
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
        daenerys_dead = true;
        ResetTextTimer();
        fill_image.FillAmount(1.0f);
    }
}