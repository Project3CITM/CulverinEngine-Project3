using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Left : CoolDown
{
    public override void OnClick()
    {
        if (GetLinkedObject("player_obj").GetComponent<CharacterController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                ActivateAbility();

                // Set Attacking State
                GetLinkedObject("player_obj").GetComponent<CharacterController>().SetState(CharacterController.State.ATTACKING);
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetComponent<CompButton>(); 
        button_cd.Deactivate();

        Debug.Log("Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}