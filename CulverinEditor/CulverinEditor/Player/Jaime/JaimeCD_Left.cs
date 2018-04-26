using CulverinEditor;
using CulverinEditor.Debug;

public class JaimeCD_Left : CoolDown
{
    JaimeController jaime_controller;

    public override void Start()
    {
        //Link to the external jaime_obj 
        jaime_controller = GetLinkedObject("jaime_obj").GetComponent<JaimeController>();

        base.Start();
    }

    public override void Update()
    {
        base.Update();
    }

    public override void OnClick()
    {
        if (jaime_controller.GetState() == 0
            && characters_manager.changing == false)
        {
            if (in_cd == false)
            {
                if (jaime_controller.OnLeftClick()) 
                {
                    ActivateAbility();
                }            
            }
        }
    }

    public override void ActivateAbility()
    {
        button_cd.Deactivate();
        act_time = 0.0f;
        in_cd = true;
    }
}