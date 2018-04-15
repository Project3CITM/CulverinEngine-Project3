using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Secondary : CoolDown
{
    public bool theon_dead = false;
    public GameObject theon_secondary_cd_text;


    void Start()
    {
        theon_dead = false;

        theon_secondary_cd_text = GetLinkedObject("theon_secondary_cd_text");
        ResetTextTimer(theon_secondary_cd_text);
    }

    public override void Update()
    {
        if (!theon_dead)
        {
            base.Update();

            //Manage Seconds Counter
            if (in_cd)
            {
                ManageTextTimer(theon_secondary_cd_text);
            }

            //Reset Seconds Counter
            if (reset_timer)
            {
                ResetTextTimer(theon_secondary_cd_text);
                reset_timer = false;
            }
        }
    }

    public override void OnClick()
    {
        if (GetLinkedObject("theon_obj").GetComponent<TheonController>().GetState() == 0
            && GetLinkedObject("player_obj").GetComponent<CharactersManager>().changing == false)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("theon_obj").GetComponent<TheonController>().OnSecondaryClick())
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetLinkedObject("theon_s_button_obj").GetComponent<CompButton>();
        button_cd.Deactivate();
        act_time = 0.0f;
        prev_seconds = 1000;
        in_cd = true;
    }
}