using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Left : CoolDown
{
    public override void Update()
    {
        base.Update();
    }

    public override void OnClick()
    {
        if (GetLinkedObject("jaime_obj").GetComponent<JaimeController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("jaime_obj").GetComponent<JaimeController>().OnLeftClick()) 
                {
                    ActivateAbility();
                }            
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetLinkedObject("jaime_button_left").GetComponent<CompButton>();
        button_cd.Deactivate();
        act_time = 0.0f;
        in_cd = true;
    }
}