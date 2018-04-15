using CulverinEditor;
using CulverinEditor.Debug;

public class LeftHp : CulverinBehaviour
{
    public GameObject this_obj_lasthp;
    public GameObject hp_bar;
    float lasthp_value = 0.0f;
    float current_lasthp = 0.0f;
    public float hp_dissapear_vel = 0.1f;
    bool hp_bar_changed = false;
    public float time_to_below_hp = 0.3f;
    public float current_temp_hp_time = 0.0f;
    bool hp_ready_to_below = false;

    void Start()
    {
        this_obj_lasthp = GetLinkedObject("this_obj_lasthp");
        hp_bar = GetLinkedObject("hp_bar");
        hp_bar_changed = false;
        hp_ready_to_below = false;
        current_temp_hp_time = 0.0f;
        this_obj_lasthp.GetComponent<CompImage>().FillAmount(hp_bar.GetComponent<Hp>().GetCurrentHealth());
        lasthp_value = hp_bar.GetComponent<Hp>().GetCurrentHealth();
        current_lasthp = lasthp_value;
    }

    void Update()
    {
        if (lasthp_value != hp_bar.GetComponent<Hp>().GetCurrentHealth() && !hp_bar_changed)
        {
            current_lasthp = lasthp_value;
            lasthp_value = hp_bar.GetComponent<Hp>().GetCurrentHealth();

            hp_bar_changed = true;
        }

        if (hp_bar_changed && hp_ready_to_below)
        {
            current_lasthp -= hp_dissapear_vel;
            if (current_lasthp >= 0)
            {
                this_obj_lasthp.GetComponent<CompImage>().FillAmount((current_lasthp / 100.0f));
            }
            if (current_lasthp <= lasthp_value)
            {
                hp_bar_changed = false;
                hp_ready_to_below = false;
            }
        }

        if(hp_bar_changed)
        {
            current_temp_hp_time += Time.deltaTime;
            if(current_temp_hp_time >= time_to_below_hp)
            {
                hp_ready_to_below = true;
            }
        }
    }
}