using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Secondary : CoolDown
{
    public bool daenerys_dead = false;
    public GameObject daenerys_secondary_cd_text;

    void Start()
    {
        daenerys_dead = false;

        daenerys_secondary_cd_text = GetLinkedObject("daenerys_secondary_cd_text");
        ResetTextTimer(daenerys_secondary_cd_text);
    }

    public override void Update()
    {
        if (!daenerys_dead)
        {
            base.Update();

            //Manage Seconds Counter
            if (in_cd)
            {
                ManageTextTimer(daenerys_secondary_cd_text);
            }

            //Reset Seconds Counter
            if (reset_timer)
            {
                ResetTextTimer(daenerys_secondary_cd_text);
                reset_timer = false;
            }
        }
    }

    public override void OnClick()
    {
        if (GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>().GetState() == 0
            && GetLinkedObject("player_obj").GetComponent<CharactersManager>().changing == false)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>().OnSecondaryClick())
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        button_cd = GetLinkedObject("daenerys_s_button_obj").GetComponent<CompButton>();
        button_cd.Deactivate();
        act_time = 0.0f;
        prev_seconds = 1000;
        in_cd = true;
    }
}