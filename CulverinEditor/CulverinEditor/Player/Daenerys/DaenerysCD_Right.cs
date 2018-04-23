using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Right : CoolDown
{
    public GameObject daenerys_right_cd_text;

    void Start()
    {
        daenerys_right_cd_text = GetLinkedObject("daenerys_right_cd_text");
        ResetTextTimer(daenerys_right_cd_text);
    }

    public override void Update()
    {
        base.Update();
        
        //Manage Seconds Counter
        if(in_cd)
        {
            ManageTextTimer(daenerys_right_cd_text);
        }

        //Reset Seconds Counter
        if(reset_timer)
        {
            ResetTextTimer(daenerys_right_cd_text);
            reset_timer = false;
        }
    }

    public override void OnClick()
    {
        if (GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>().GetState() == 0
            && GetLinkedObject("player_obj").GetComponent<CharactersManager>().changing == false)
        {
            if (in_cd == false)
            {               
                if(GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>().OnRightClick() == true)
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetLinkedObject("daenerys_button_right_obj").GetComponent<CompButton>();
        button_cd.Deactivate();
        act_time = 0.0f;
        prev_seconds = 1000;
        in_cd = true;
    }
}