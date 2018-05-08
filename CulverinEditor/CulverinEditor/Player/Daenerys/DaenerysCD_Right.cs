using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Right : CoolDown
{
    public GameObject daenerys_right_cd_text;

    DaenerysController daenerys_controller;

    public override void Start()
    {
        daenerys_right_cd_text = GetLinkedObject("daenerys_right_cd_text");

        LinkTextTimer(daenerys_right_cd_text);
        ResetTextTimer();

        //Link to the external daenerys_obj 
        daenerys_controller = GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>();

        base.Start();
    }

    public override void Update()
    {
        base.Update();
        
        //Manage Seconds Counter
        if(in_cd)
        {
            ManageTextTimer();
        }

        //Reset Seconds Counter
        if(reset_timer)
        {
            ResetTextTimer();
            reset_timer = false;
        }
    }

    public override void OnClick()
    {
        if (daenerys_controller.GetState() == 0
            && characters_manager.changing == false)
        {
            if (in_cd == false)
            {               
                if(daenerys_controller.OnRightClick() == true)
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
        cd_time = daenerys_controller.right_ability_cd_time;
        prev_seconds = 1000;
        in_cd = true;
    }
}