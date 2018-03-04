using CulverinEditor;
using CulverinEditor.Debug;

public class Hp : CulverinBehaviour
{
    public GameObject this_obj_hp;
    CompImage hp_bar;
    public float max_hp = 100.0f;
    float current_hp = 100.0f;
    float calc_hp = 1.0f;

    void Start()
    {
    }

    void Update()
    {
    }

    public void GetDamage(float dmg)
    {
        this_obj_hp = GetLinkedObject("this_obj_hp");
        current_hp -= dmg;
        if(current_hp < 0)
        {
            current_hp = 0;
        }

        calc_hp = current_hp / max_hp;
        hp_bar = this_obj_hp.GetComponent<CompImage>();
        hp_bar.FillAmount(calc_hp);

        Debug.Log("Get Damage -> Current HP: " + current_hp.ToString());
    }

    public float GetCurrentHealth()
    {
        return current_hp;
    }

    public void SetHP(float curr_health, float max_health)
    {
        current_hp = curr_health;
        max_hp = max_health;

        calc_hp = current_hp / max_hp;
        hp_bar = this_obj_hp.GetComponent<CompImage>();
        hp_bar.FillAmount(calc_hp);

        Debug.Log("Current HP: " + current_hp.ToString());
    }
}