using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Right : CoolDown
{
    public GameObject jaime_right_cd_text;

    JaimeController jaime_controller;

    public override void Start()
    {
        jaime_right_cd_text = GetLinkedObject("jaime_right_cd_text");

        LinkTextTimer(jaime_right_cd_text);
        ResetTextTimer();

        //Link to the external daenerys_obj 
        jaime_controller = GetLinkedObject("jaime_obj").GetComponent<JaimeController>();

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
        if (jaime_controller.GetState() == 0
            && characters_manager.changing == false)
        {
            if (in_cd == false)
            {
                if (jaime_controller.OnRightClick())
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