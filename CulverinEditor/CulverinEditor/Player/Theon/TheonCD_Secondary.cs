using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Secondary : CoolDown
{
    public bool theon_dead = false;

    void Start()
    {
        theon_dead = false;
    }

    public override void Update()
    {
        if (!theon_dead && in_cd)
        {
            act_time += Time.deltaTime;
            if (act_time >= cd_time)
            {
                in_cd = false;
                button_cd = GetComponent<CompButton>();
                button_cd.Activate();
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
        in_cd = true;
    }
}