using CulverinEditor;
using CulverinEditor.Debug;

public class ActivateEnemies : CulverinBehaviour
{
    public GameObject sword_enemy;
    public GameObject spear_enemy;
    public GameObject shield_enemy;

    private bool sword_active;
    private bool spear_active;
    private bool shield_active;

    void Start()
    {
        sword_active = false;
        spear_active = false;
        shield_active = false;

        sword_enemy = GetLinkedObject("sword_enemy");
        spear_enemy = GetLinkedObject("spear_enemy");
        shield_enemy = GetLinkedObject("shield_enemy");
    }

    void Update()
    {
        if(Input.GetKeyDown(KeyCode.I))
        {
            sword_active = !sword_active;
            sword_enemy.SetActive(sword_active);
        }
        if (Input.GetKeyDown(KeyCode.O))
        {
            spear_active = !spear_active;
            spear_enemy.SetActive(spear_active);
        }
        if (Input.GetKeyDown(KeyCode.P))
        {
            shield_active = !shield_active;
            shield_enemy.SetActive(shield_active);
        }
    }
}
