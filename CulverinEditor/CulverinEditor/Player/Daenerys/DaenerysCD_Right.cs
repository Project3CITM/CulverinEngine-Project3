using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Right : CoolDown
{
    public override void OnClick()
    {
        if (GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                ActivateAbility();

                // Set State
                GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>().SetState(CharacterController.State.COVER);
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Daenerys Right CD Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}