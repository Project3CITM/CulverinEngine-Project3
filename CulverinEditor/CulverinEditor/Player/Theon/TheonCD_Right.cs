using CulverinEditor;
using CulverinEditor.Debug;

public class TheonCD_Right : CoolDown
{
    public override void Update()
    {
        base.Update();

        Debug.Log(act_time, Department.PLAYER, Color.RED);
    }

    public override void OnClick()
    {
        if (GetLinkedObject("theon_obj").GetComponent<TheonController>().GetState() == 0 
            && GetLinkedObject("theon_obj").GetComponent<TheonController>().reloading == false
            && GetLinkedObject("player_obj").GetComponent<CharactersManager>().changing == false)
        {
            if (in_cd == false)
            {
                if (GetLinkedObject("theon_obj").GetComponent<TheonController>().OnRightClick() == true)
                {
                    ActivateAbility();
                }
            }
        }
    }

    public override void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = GetLinkedObject("theon_button_right").GetComponent<CompButton>();
        button_cd.Deactivate();
        act_time = 0.0f;
        in_cd = true;
    }
}