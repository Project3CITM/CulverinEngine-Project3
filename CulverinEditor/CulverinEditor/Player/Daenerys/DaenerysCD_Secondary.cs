using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Secondary : CoolDown
{
    public override void Update()
    {
        base.Update();
    }

    public override void OnClick()
    {
        if (GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>().GetState() == 0)
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
        //this_obj.GetComponent
        button_cd = GetLinkedObject("daenerys_s_button_obj").GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Daenerys Secondary CD Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}