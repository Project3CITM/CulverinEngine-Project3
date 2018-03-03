using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class JaimeWeapon_Right : WeaponController
{
    public GameObject jaime_obj;
    public GameObject jaime_button_right;

    JaimeController character;

    public override void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        jaime_obj = GetLinkedObject("jaime_obj");
        jaime_button_right = GetLinkedObject("jaime_button_right");
        enemy_obj = GetLinkedObject("enemy_obj");
    }

    void Update()
    {
    }

    // This method will be called when the associated button to this weapon is pressed
    public override void OnClick()
    {
        character = jaime_obj.GetComponent<JaimeController>();
        // Check if player is in Idle State
        if (character.GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (character.GetCurrentStamina() > stamina_cost)
            {
                cd = jaime_button_right.GetComponent<CoolDown>();
                //Check if the ability is not in cooldown
                if (!cd.in_cd)
                {
                    Debug.Log("Jaime RW Going to Block");

                    // First, OnClick of RightWeapon, then, onClick of Cooldown
                    DoAbility();

                    // Set Animation
                    character.SetAnimationTransition("ToCover", true);
                }
                else
                {
                    Debug.Log("Jaime RW Ability in CD");
                }
            }
            else
            {
                Debug.Log("Jaime RW Not Enough Stamina");
            }
        }
    }

    public override void PrepareAbility()
    {
        Debug.Log("Jaime RW Prepare Block");
        button = jaime_button_right.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }
}
