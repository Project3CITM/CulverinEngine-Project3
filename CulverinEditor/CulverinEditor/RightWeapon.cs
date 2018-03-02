using CulverinEditor;
using CulverinEditor.Debug;

// The concept is to have WeaponController as a base class, and override for all the weapons

public class RightWeapon : CulverinBehaviour
{
    public GameObject player_obj;
    public CharacterController player;
    public GameObject r_button_obj;
    public CompButton button;
    public CoolDownRight cd;
    public GameObject enemy_obj;
    public EnemyController enemy;
    public CompAnimation animation_weapon;

    // WEAPON STATS -------------
    public float attack_dmg = 0.0f;
    public float stamina_cost = 10.0f;
    // ---------------------------

    void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        r_button_obj = GetLinkedObject("r_button_obj");
        enemy_obj = GetLinkedObject("enemy_obj");
        player_obj = GetLinkedObject("player_obj");
    }

    void Update()
    {
    }

    public void Block() //Might be virtual (not Block())
    {
        Debug.Log("Block Right");

        // Decrease stamina -----------
        player = player_obj.GetComponent<CharacterController>();
        player.DecreaseStamina(stamina_cost);
    }

    // This method will be called when the associated button to this weapon is pressed
    void OnClick()
    {
        player = player_obj.GetComponent<CharacterController>();
        // Check if player is in Idle State
        if (player.GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (player.GetCurrentStamina() > stamina_cost)
            {
                cd = r_button_obj.GetComponent<CoolDownRight>();
                //Check if the ability is not in cooldown
                if (!cd.in_cd)
                {
                    Debug.Log("Going to Block");

                    // First, OnClick of RightWeapon, then, onClick of Cooldown
                    Block();

                    // Set Animation
                    SetBlockAnim();

                    // Play the Sound FX
                    //PlayFx();
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

    public void PrepareAttack()
    {
        Debug.Log("Prepare Block");
        button = r_button_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }


    public void SetBlockAnim()
    {
        animation_weapon = GetLinkedObject("lweapon_obj").GetComponent<CompAnimation>();
        animation_weapon.SetTransition("ToCover");

        animation_weapon = GetLinkedObject("rweapon_obj").GetComponent<CompAnimation>();
        animation_weapon.SetTransition("ToCover");
    }
}
