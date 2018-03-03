using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class JaimeWeapon_Left : WeaponController
{
    public GameObject jaime_obj;
    public GameObject jaime_button_left;

    JaimeController character;

    public override void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        jaime_obj = GetLinkedObject("jaime_obj");
        jaime_button_left = GetLinkedObject("jaime_button_left");
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
        if (character.GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (character.GetCurrentStamina() > stamina_cost)
            {
                cd = jaime_button_left.GetComponent<CoolDown>();
                //Check if the ability is not in cooldown
                if (!cd.in_cd)
                {
                    Debug.Log("Going to Attack");

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
        Debug.Log("Prepare Ability");
        button = jaime_button_left.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public override void DoAbility() //Might be virtual
    {
        Debug.Log("Attack Left");

        // Decrease stamina -----------

        character = player_obj.GetComponent<JaimeController>();
        character.DecreaseStamina(stamina_cost);

        Debug.Log("Going to hit");

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
        player_obj.GetComponent<CompAudio>().PlayEvent("SwordSlash");
    }
}