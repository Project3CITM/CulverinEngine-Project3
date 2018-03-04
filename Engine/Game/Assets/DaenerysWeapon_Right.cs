using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class DaenerysWeapon_Right : WeaponController
{
    public GameObject daenerys_obj;
    public GameObject daenerys_button_right;

    DaenerysController character;

    public override void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        daenerys_obj = GetLinkedObject("daenerys_obj");
        daenerys_button_right = GetLinkedObject("daenerys_button_right");
        enemy_obj = GetLinkedObject("enemy_obj");
    }

    void Update()
    {
    }

    // This method will be called when the associated button to this weapon is pressed
    public override void OnClick()
    {
        character = daenerys_obj.GetComponent<DaenerysController>();
        // Check if player is in Idle State
        if (character.GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (character.GetCurrentStamina() > stamina_cost)
            {
                //cd = daenerys_button_right.GetComponent<CoolDown>();
                //Check if the ability is not in cooldown
                //if (!cd.in_cd)
                if(1 == 1)
                {
                    Debug.Log("Daenerys RW Going to Block");

                    // First, OnClick of RightWeapon, then, onClick of Cooldown
                    DoAbility();

                    // Set Animation
                    character.SetAnimationTransition("ToCover", true);
                }
                else
                {
                    Debug.Log("Daenerys RW Ability in CD");
                }
            }
            else
            {
                Debug.Log("Daenerys RW Not Enough Stamina");
            }
        }
    }

    public override void PrepareAbility()
    {
        Debug.Log("Daenerys RW Prepare Block");
        OnClick();
        //button = daenerys_button_right.GetComponent<CompButton>();
        //button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }
}
