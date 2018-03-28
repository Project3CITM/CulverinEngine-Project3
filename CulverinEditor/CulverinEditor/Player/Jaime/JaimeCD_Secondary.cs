using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Secondary : CoolDown
{
    public override void Update()
    {
        base.Update();
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
        in_cd = true;
    }
}