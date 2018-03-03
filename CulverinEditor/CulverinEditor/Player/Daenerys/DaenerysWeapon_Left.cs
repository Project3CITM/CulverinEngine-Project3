using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class DaenerysWeapon_Left : WeaponController
{
    public GameObject daenerys_obj;
    public GameObject daenerys_button_left;

    DaenerysController character;

    public override void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        daenerys_obj = GetLinkedObject("daenerys_obj");
        daenerys_button_left = GetLinkedObject("daenerys_button_left");
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
        if (character.GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (character.GetCurrentStamina() > stamina_cost)
            {
                cd = daenerys_button_left.GetComponent<CoolDown>();
                //Check if the ability is not in cooldown
                if (!cd.in_cd)
                {
                    Debug.Log("Daenerys LW Going to Attack");

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
        Debug.Log("Daenerys LW Prepare Ability");
        button = daenerys_button_left.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public override void DoAbility() //Might be virtual
    {
        Debug.Log("Daenerys LW Attack Left");

        // Decrease stamina -----------
        character = daenerys_obj.GetComponent<DaenerysController>();
        character.DecreaseStamina(stamina_cost);

        Debug.Log("Daenerys LW Going to hit");

        // Attack the enemy in front of you
        if (EnemyInFront())
        {
            // To change => check the specific enemy in front of you
            enemy = enemy_obj.GetComponent<EnemyController>();
            enemy.Hit(attack_dmg);
        }
    }

    public override void PlayFx()
    {
        GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("SwordSlash");
    }
}