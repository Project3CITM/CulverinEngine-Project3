using CulverinEditor;
using CulverinEditor.Debug;

public class LeftMana : CulverinBehaviour
{

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
        this_obj_leftmana.GetComponent<CompImage>().FillAmount(mana_bar.GetComponent<Mana>().GetCurrentMana());
        lastmana_value = mana_bar.GetComponent<Mana>().GetCurrentMana();
        current_lastmana = lastmana_value;
    }

    void Update()
    {
        if (lastmana_value != mana_bar.GetComponent<Mana>().GetCurrentMana() && !mana_bar_changed)
        {
            current_lastmana = lastmana_value;
            lastmana_value = mana_bar.GetComponent<Mana>().GetCurrentMana();

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