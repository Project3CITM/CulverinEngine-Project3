using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Right : CoolDown
{
    public override void OnClick()
    {
        if (GetLinkedObject("theon_obj").GetComponent<CharacterController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                ActivateAbility();

                // Set Attacking State
                GetLinkedObject("theon_obj").GetComponent<CharacterController>().SetState(CharacterController.State.COVER);
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Theon Right CD Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}