using CulverinEditor;
using CulverinEditor.Debug;

public class LeftMana : CulverinBehaviour
{

    public GameObject this_obj_lastmana;
    public GameObject mana_bar;
    public float lasthp_value = 0.0f;
    public float current_lasthp = 0.0f;
    public float hp_dissapear_vel = 0.1f;
    public bool hp_bar_changed = false;

    void Start()
    {
        this_obj_lastmana = GetLinkedObject("this_obj_lastmana");
        mana_bar = GetLinkedObject("mana_bar");
        hp_bar_changed = false;
        this_obj_lastmana.GetComponent<CompImage>().FillAmount(mana_bar.GetComponent<Hp>().GetCurrentHealth());
        lasthp_value = mana_bar.GetComponent<Hp>().GetCurrentHealth();
        current_lasthp = lasthp_value;
    public GameObject this_obj_leftmana;
    public GameObject mana_bar;
    public float lastmana_value = 0.0f;
    public float current_lastmana = 0.0f;
    public float mana_dissapear_vel = 0.1f;
    public bool mana_bar_changed = false;

    void Start()
    {
        this_obj_leftmana = GetLinkedObject("this_obj_leftmana");
        mana_bar = GetLinkedObject("mana_bar");
        mana_bar_changed = false;
        this_obj_leftmana.GetComponent<CompImage>().FillAmount(mana_bar.GetComponent<Hp>().GetCurrentHealth());
        lastmana_value = mana_bar.GetComponent<Hp>().GetCurrentHealth();
        current_lastmana = lastmana_value;
    }

    void Update()
    {
        if (lasthp_value != mana_bar.GetComponent<Hp>().GetCurrentHealth() && !hp_bar_changed)
        {
            Debug.Log("Jimmy");
            current_lasthp = lasthp_value;
            lasthp_value = mana_bar.GetComponent<Hp>().GetCurrentHealth();

            hp_bar_changed = true;
        }

        if (hp_bar_changed)
        {
            current_lasthp -= hp_dissapear_vel;
            if (current_lasthp >= 0)
            {
                this_obj_lastmana.GetComponent<CompImage>().FillAmount((current_lasthp / 100.0f));
            }
            if (current_lasthp <= lasthp_value)
            {
                hp_bar_changed = false;
        if (lastmana_value != mana_bar.GetComponent<Hp>().GetCurrentHealth() && !mana_bar_changed)
        {
            Debug.Log("Jimmy");
            current_lastmana = lastmana_value;
            lastmana_value = mana_bar.GetComponent<Hp>().GetCurrentHealth();

            mana_bar_changed = true;
        }

        if (mana_bar_changed)
        {
            current_lastmana -= mana_dissapear_vel;
            if (current_lastmana >= 0)
            {
                this_obj_leftmana.GetComponent<CompImage>().FillAmount((current_lastmana / 100.0f));
            }
            if (current_lastmana <= lastmana_value)
            {
                mana_bar_changed = false;
            }
        }
    }
}