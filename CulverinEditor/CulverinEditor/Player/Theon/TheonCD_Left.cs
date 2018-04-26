using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Left : CoolDown
{
    public GameObject theon_left_cd_text;

    TheonController theon_controller;

    public override void Start()
    {
        theon_left_cd_text = GetLinkedObject("theon_left_cd_text");

        LinkTextTimer(theon_left_cd_text);
        ResetTextTimer();

        //Link to the external daenerys_obj 
        theon_controller = GetLinkedObject("theon_obj").GetComponent<TheonController>();

        base.Start();
    }

    public override void Update()
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

    public override void OnClick()
    {
        if (theon_controller.GetState() == 0 
            && characters_manager.changing == false)
        {
            if (in_cd == false)
            {
                if (theon_controller.OnLeftClick() == true)
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
    }
}