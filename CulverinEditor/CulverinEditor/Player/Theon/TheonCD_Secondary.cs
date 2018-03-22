using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Secondary : CoolDown
{
    public override void Update()
    {
        base.Update();
    }

    public override void OnClick()
    {
        Debug.Log("FUCK THIS SHIT");
        if (GetLinkedObject("theon_obj").GetComponent<TheonController>().GetState() == 0)
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