using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Left : CoolDown
{
    public override void OnClick()
    {
        if (GetLinkedObject("daenerys_obj").GetComponent<CharacterController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                ActivateAbility();

                // Set Attacking State
                GetLinkedObject("daenerys_obj").GetComponent<CharacterController>().SetState(CharacterController.State.ATTACKING);
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Daenerys Left CD Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}