using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class TheonWeapon_Left : WeaponController
{
    public GameObject theon_obj;
    public GameObject theon_button_left;

    TheonController character;

    public override void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        theon_obj = GetLinkedObject("theon_obj");
        theon_button_left = GetLinkedObject("theon_button_left");
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
        if (character.GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (character.GetCurrentStamina() > stamina_cost)
            {
                //cd = theon_button_left.GetComponent<CoolDown>();
                //Check if the ability is not in cooldown
                //if (!cd.in_cd)
                if(1 == 1)
                {
                    Debug.Log("Theon LW Going to Attack");

                    // First, OnClick of LeftWeapon, then, onClick of Cooldown
                    DoAbility();

                    // Set Attacking Animation
                    character.SetAnimationTransition("ToAttack1", true);

                    // Play the Sound FX
                    PlayFx();
                }
                else
                {
                    Debug.Log("Ability in CD");
                }
            }
            else
            {
                Debug.Log("Not Enough Stamina");
            }
        }
    }

    public override void PrepareAbility()
    {
        Debug.Log("Theon LW Prepare Ability");
        OnClick();
        //button = theon_button_left.GetComponent<CompButton>();
        //button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public override void DoAbility() //Might be virtual
    {
        Debug.Log("Theon LW Attack Left");

        // Decrease stamina -----------
        character = theon_obj.GetComponent<TheonController>();
        character.DecreaseStamina(stamina_cost);

        Debug.Log("Theon LW Going to hit");

        // Attack the enemy in front of you
        //if (GetLinkedObject("player_obj").GetComponent<MovementController>().EnemyInFront())
        //{
        //    // To change => check the specific enemy in front of you
        //    enemy = enemy_obj.GetComponent<EnemyController>();
        //    enemy.Hit(attack_dmg);
        //}
    }

    public override void PlayFx()
    {
        //GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("SwordSlash");
    }
}