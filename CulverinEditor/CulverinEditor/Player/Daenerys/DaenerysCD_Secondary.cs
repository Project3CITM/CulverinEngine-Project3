using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Secondary : CoolDown
{
    public bool daenerys_dead = false;

    void Start()
    {
        daenerys_dead = false;
    }

    public override void Update()
    {
        if (!daenerys_dead && in_cd)
        {
            //Manage the Radial Fill Cooldown
            float final_time = cd_time - act_time;
            if (final_time <= 0.0f)
            {
                final_time = 0.0f;
            }

            calc_time = final_time / cd_time;
            GetComponent<CompImage>().FillAmount(calc_time);
            
            //Manage Logical CD time
            act_time += Time.deltaTime;

            //Detect when activate the button again
            if (act_time >= cd_time)
            {
                in_cd = false;
                button_cd = GetComponent<CompButton>();
                button_cd.Activate();
                GetComponent<CompImage>().FillAmount(1.0f);
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
        in_cd = true;
    }
}