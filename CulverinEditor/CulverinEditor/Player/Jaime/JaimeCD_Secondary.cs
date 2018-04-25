using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Secondary : CoolDown
{
    public bool jaime_dead = false;
    public GameObject jaime_secondary_cd_text;


    void Start()
    {
        jaime_dead = false;

        jaime_secondary_cd_text = GetLinkedObject("jaime_secondary_cd_text");
        ResetTextTimer(jaime_secondary_cd_text);
    }

    public override void Update()
    {
        if (!jaime_dead)
        {
            base.Update();

            //Manage Seconds Counter
            if (in_cd)
            {
                ManageTextTimer(jaime_secondary_cd_text);
            }

            //Reset Seconds Counter
            if (reset_timer)
            {
                ResetTextTimer(jaime_secondary_cd_text);
                reset_timer = false;
            }
        }
    }

    public override void OnClick()
    {
        if (GetLinkedObject("jaime_obj").GetComponent<JaimeController>().GetState() == 0
            && GetLinkedObject("player_obj").GetComponent<CharactersManager>().changing == false)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("jaime_obj").GetComponent<JaimeController>().OnSecondaryClick())
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetLinkedObject("jaime_s_button_obj").GetComponent<CompButton>();
        button_cd.Deactivate();
        act_time = 0.0f;
        prev_seconds = 1000;
        in_cd = true;

        //SET COOLDOWN TO 1 SECOND
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().god_mode ||
            GetLinkedObject("player_obj").GetComponent<CharactersManager>().no_cds)
        {
            cd_time = 1.0f;
        }
    }

    public void Die()
    {
        jaime_dead = true;
        ResetTextTimer(jaime_secondary_cd_text);
        GetComponent<CompImage>().FillAmount(1.0f);
    }
}