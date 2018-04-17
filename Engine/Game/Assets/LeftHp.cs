using CulverinEditor;
using CulverinEditor.Debug;

public class LeftHp : CulverinBehaviour
{

    public GameObject this_obj_lasthp;
    public GameObject hp_bar;
    public float lasthp_value = 0.0f;
    public float current_lasthp = 0.0f;
    public float hp_dissapear_vel = 0.1f;
    public bool hp_bar_changed = false;

    void Start()
    {
        this_obj_lasthp = GetLinkedObject("this_obj_lasthp");
        hp_bar = GetLinkedObject("hp_bar");
        hp_bar_changed = false;
        this_obj_lasthp.GetComponent<CompImage>().FillAmount(hp_bar.GetComponent<Hp>().GetCurrentHealth());
        lasthp_value = hp_bar.GetComponent<Hp>().GetCurrentHealth();
        current_lasthp = lasthp_value;
    }

    void Update()
    {
        if (lasthp_value != hp_bar.GetComponent<Hp>().GetCurrentHealth() && !hp_bar_changed)
        {
            Debug.Log("Jimmy");
            current_lasthp = lasthp_value;
            lasthp_value = hp_bar.GetComponent<Hp>().GetCurrentHealth();

            hp_bar_changed = true;
        }

        if (hp_bar_changed)
        {
            current_lasthp -= hp_dissapear_vel;
            if (current_lasthp >= 0)
            {
                this_obj_lasthp.GetComponent<CompImage>().FillAmount((current_lasthp / 100.0f));
            }
            if (current_lasthp <= lasthp_value)
            {
                hp_bar_changed = false;
            }
        }
    }
}