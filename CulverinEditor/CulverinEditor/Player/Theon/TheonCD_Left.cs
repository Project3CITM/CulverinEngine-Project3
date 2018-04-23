using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Left : CoolDown
{
    public GameObject theon_left_cd_text;

    void Start()
    {
        theon_left_cd_text = GetLinkedObject("theon_left_cd_text");
        ResetTextTimer(theon_left_cd_text);
    }

    public override void Update()
    {
        base.Update();

        //Manage Seconds Counter
        if (in_cd)
        {
            ManageTextTimer(theon_left_cd_text);
        }

        //Reset Seconds Counter
        if (reset_timer)
        {
            ResetTextTimer(theon_left_cd_text);
            reset_timer = false;
        }
    }

    public override void OnClick()
    {
        if (GetLinkedObject("theon_obj").GetComponent<TheonController>().GetState() == 0 
            && GetLinkedObject("player_obj").GetComponent<CharactersManager>().changing == false)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("theon_obj").GetComponent<TheonController>().OnLeftClick() == true)
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetLinkedObject("theon_button_left").GetComponent<CompButton>();
        button_cd.Deactivate();
        act_time = 0.0f;
        prev_seconds = 1000;
        in_cd = true;
    }
}