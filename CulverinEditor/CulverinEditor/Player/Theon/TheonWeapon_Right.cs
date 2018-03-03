using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class TheonWeapon_Right : WeaponController
{
    public GameObject theon_obj;
    public GameObject theon_button_right;

    TheonController character;

    public override void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        theon_obj = GetLinkedObject("theon_obj");
        theon_button_right = GetLinkedObject("theon_button_right");
        enemy_obj = GetLinkedObject("enemy_obj");
    }

    void Update()
    {
    }

    // This method will be called when the associated button to this weapon is pressed
    public override void OnClick()
    {
        character = theon_obj.GetComponent<TheonController>();
        // Check if player is in Idle State
        if (character.GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (character.GetCurrentStamina() > stamina_cost)
            {
                cd = theon_button_right.GetComponent<CoolDown>();
                //Check if the ability is not in cooldown
                if (!cd.in_cd)
                {
                    Debug.Log("Theon RW Going to Block");

                    // First, OnClick of RightWeapon, then, onClick of Cooldown
                    DoAbility();

                    // Set Animation
                    character.SetAnimationTransition("ToCover", true);
                }
                else
                {
                    Debug.Log("Theon RW Ability in CD");
                }
            }
            else
            {
                Debug.Log("Theon RW Not Enough Stamina");
            }
        }
    }

    public override void PrepareAbility()
    {
        Debug.Log("Theon RW Prepare Block");
        button = theon_button_right.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }
}
