using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class DaenerysWeapon_Left : WeaponController
{
    public GameObject daenerys_obj;
    public GameObject daenerys_button_left;

    public float mana_cost_percentage = 20f;
    public float damage_percentage = 10f;

    DaenerysController character;
    DaenerysCD_Left cd_left;

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
            float mana_cost = mana_cost_percentage * character.max_mana / 100.0f;
            if (character.GetCurrentMana() > mana_cost)
            {
                cd_left = daenerys_button_left.GetComponent<DaenerysCD_Left>();
                //Check if the ability has enough charges
                if(cd_left.GetCurrentCharges() > 0)
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
                    Debug.Log("Ability doesn't have enough charges");
                }
            }
            else
            {
                Debug.Log("Not Enough Mana");
            }
        }
    }

    public override void PrepareAbility()
    {
        Debug.Log("Daenerys LW Prepare Ability");
        OnClick();
        //button = daenerys_button_left.GetComponent<CompButton>();
        //button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public override void DoAbility() //Might be virtual
    {
        Debug.Log("Daenerys LW Attack Left");

        // Decrease mana -----------
        character = daenerys_obj.GetComponent<DaenerysController>();
        character.DecreaseManaPercentage(mana_cost_percentage);

        Debug.Log("Daenerys LW Going to hit");

        // Attack all enemies in 3 rows in front of you
        //if (GetLinkedObject("player_obj").GetComponent<MovementController>().EnemyInFront())
        //{
        //    // To change => check the specific enemy in front of you
        //    enemy = enemy_obj.GetComponent<EnemyController>();
        //    enemy.HitPercentage(damage_percentage);
        //}
    }

    public override void PlayFx()
    {
        //GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("Dracarys");
    }
}