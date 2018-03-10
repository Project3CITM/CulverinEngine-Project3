using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Right : CoolDown
{
    public override void OnClick()
    {
        if (GetLinkedObject("theon_obj").GetComponent<TheonController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("theon_obj").GetComponent<TheonController>().OnRightClick() == true)
                {
                    ActivateAbility();

                    // Set Attacking State
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetLinkedObject("theon_button_right").GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Theon Right CD Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}