using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class DaenerysWeapon_Right : WeaponController
{
    public GameObject daenerys_obj;
    public GameObject daenerys_button_right;

    public float mana_cost_percentage = 20f;
    public float damage_percentage = 10f;

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
            // Check if player has enough mana to perform its attack
            float mana_cost = mana_cost_percentage * character.max_mana / 100.0f;
            if (character.GetCurrentMana() > mana_cost)
            {
                //cd = daenerys_button_right.GetComponent<CoolDown>();
                //Check if the ability is not in cooldown
                //if (!cd.in_cd)
                if(1 == 1)
                {
                    Debug.Log("Daenerys RW Going to Set Fire Wall");

                    // First, OnClick of RightWeapon, then, onClick of Cooldown
                    DoAbility();

                    // Set Animation
                    character.SetAnimationTransition("ToFireWall", true);
                }
                else
                {
                    Debug.Log("Daenerys RW Ability in CD");
                }
            }
            else
            {
                Debug.Log("Daenerys RW Not Enough Mana");
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

    public override void DoAbility() //Might be virtual
    {
        Debug.Log("Daenerys RW Attack Right");

        // Decrease mana -----------
        character = daenerys_obj.GetComponent<DaenerysController>();
        character.DecreaseManaPercentage(mana_cost_percentage);

        Debug.Log("Daenerys RW Going to hit");

        // Set a fire wall in north tile
        //if (GetLinkedObject("player_obj").GetComponent<MovementController>().EnemyInFront())
        //{
        //    // To change => check the specific enemy in front of you
        //    enemy = enemy_obj.GetComponent<EnemyController>();
        //    enemy.HitPercentage(damage_percentage);
        //}
    }
}
