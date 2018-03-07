using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Right : CoolDown
{
    public override void OnClick()
    {
        if (GetLinkedObject("jaime_obj").GetComponent<CharacterController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("jaime_obj").GetComponent<JaimeController>().OnRightClick())
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Jaime Right CD Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}