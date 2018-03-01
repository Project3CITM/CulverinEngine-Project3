using CulverinEditor;
using CulverinEditor.Debug;

public class RightWeapon : CulverinBehaviour
{
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
    }

    void Update()
    {
    }

    public void Block()
    {
        Debug.Log("Block Right");

        // Decrease stamina -----------
        player = GetLinkedObject("player_obj").GetComponent<CharacterController>();
        player.DecreaseStamina(stamina_cost);
    }

    // This method will be called when the associated button to this weapon is pressed
    void OnClick()
    {
        player = GetLinkedObject("player_obj").GetComponent<CharacterController>();
        // Check if player is in Idle State
        if (player.GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (player.GetCurrentStamina() > stamina_cost)
            {
                r_button_obj = GetLinkedObject("r_button_obj");
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
        r_button_obj = GetLinkedObject("r_button_obj");
        button = r_button_obj.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }


    public void SetBlockAnim()
    {
        GameObject leftweapon = GetLinkedObject("lweapon_obj");
        animation_weapon = leftweapon.GetComponent<CompAnimation>();
        animation_weapon.SetTransition("ToCover");

        GameObject rightweapon = GetLinkedObject("rweapon_obj");
        animation_weapon = rightweapon.GetComponent<CompAnimation>();
        animation_weapon.SetTransition("ToCover");
    }
}
