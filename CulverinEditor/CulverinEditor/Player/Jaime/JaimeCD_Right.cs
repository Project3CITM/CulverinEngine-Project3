using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Right : CoolDown
{
    public GameObject jaime_right_cd_text;

    void Start()
    {
        jaime_right_cd_text = GetLinkedObject("jaime_right_cd_text");
        ResetTextTimer(jaime_right_cd_text);
    }

    public override void Update()
    {
        base.Update();

        //Manage Seconds Counter
        if (in_cd)
        {
            ManageTextTimer(jaime_right_cd_text);
        }

        //Reset Seconds Counter
        if (reset_timer)
        {
            ResetTextTimer(jaime_right_cd_text);
            reset_timer = false;
        }
    }

    public override void OnClick()
    {
        if (GetLinkedObject("jaime_obj").GetComponent<JaimeController>().GetState() == 0
            && GetLinkedObject("player_obj").GetComponent<CharactersManager>().changing == false)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("jaime_obj").GetComponent<JaimeController>().OnRightClick())
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetLinkedObject("jaime_button_right").GetComponent<CompButton>();
        button_cd.Deactivate();
        act_time = 0.0f;
        prev_seconds = 1000;
        in_cd = true;
    }
}