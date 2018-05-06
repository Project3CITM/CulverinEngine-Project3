using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysCD_Left : CoolDown
{
    public int max_charges = 3;
    private int current_charges = 3;

    public GameObject charge_count_0;
    CompImage charge_img_0;
    public GameObject charge_count_1;
    CompImage charge_img_1;
    public GameObject charge_count_2;
    CompImage charge_img_2;
    public GameObject charge_count_3;
    CompImage charge_img_3;

    public GameObject daenerys_left_cd_text;

    DaenerysController daenerys_controller;

    public override void Start()
    {
        charge_count_0 = GetLinkedObject("charge_count_0");
        charge_count_1 = GetLinkedObject("charge_count_1");
        charge_count_2 = GetLinkedObject("charge_count_2");
        charge_count_3 = GetLinkedObject("charge_count_3");
        daenerys_left_cd_text = GetLinkedObject("daenerys_left_cd_text");

        charge_img_0 = charge_count_0.GetComponent<CompImage>();
        charge_img_1 = charge_count_1.GetComponent<CompImage>();
        charge_img_2 = charge_count_2.GetComponent<CompImage>();
        charge_img_3 = charge_count_3.GetComponent<CompImage>();
                                                                                
        charge_img_0.SetEnabled(false);
        charge_img_1.SetEnabled(false);
        charge_img_2.SetEnabled(false);
        charge_img_3.SetEnabled(false);

        current_charges = 1;

        LinkTextTimer(daenerys_left_cd_text);
        ResetTextTimer();

        //Link to the external daenerys_obj 
        daenerys_controller = GetLinkedObject("daenerys_obj").GetComponent<DaenerysController>();

        base.Start();
    }

    public override void Update()
    {
        base.Update();

        //Manage Seconds Counter
        if (in_cd)
        {
            ManageTextTimer();
        }

        //Reset Seconds Counter
        if (reset_timer)
        {
            ResetTextTimer();
            reset_timer = false;
        }
    }

    public override void OnClick()
    {
        if (daenerys_controller.GetState() == 0
            && characters_manager.changing == false)
        {
            if (in_cd == false)
            {
                if (daenerys_controller.OnLeftClick() == true)
                {
                    ActivateAbility();
                }                
            }
        }
    }

    public override void ActivateAbility()
    {
        //NO CHARGES COST IN GOD MODE
        //if (characters_manager.god_mode == false ||
        //    characters_manager.no_energy == false)
        //{
            //current_charges--;
            //UpdateChargesIcon();
        //}

        act_time = 0.0f;
        
        //if (current_charges == 0)
        //{
        button_cd.Deactivate();
        cd_time = daenerys_controller.left_ability_cd_time;
        in_cd = true;
        prev_seconds = 1000;
        //}
    }

    public int GetCurrentCharges()
    {
        return current_charges;
    }

    void UpdateChargesIcon()
    {
        switch(current_charges)
        {
            case 0:
                {
                    charge_img_0.SetEnabled(true);
                    charge_img_1.SetEnabled(false);
                    break;
                }
            case 1:
                {
                    charge_img_0.SetEnabled(false);
                    charge_img_1.SetEnabled(true);
                    charge_img_2.SetEnabled(false);
                    break;
                }
            case 2:
                {
                    charge_img_1.SetEnabled(false);
                    charge_img_2.SetEnabled(true);
                    charge_img_3.SetEnabled(false);
                    break; 
                }          
            case 3:        
                {          
                    charge_img_2.SetEnabled(false);
                    charge_img_3.SetEnabled(true);
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
}