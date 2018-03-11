using CulverinEditor;
using CulverinEditor.Debug;

public class Mana : CulverinBehaviour
{
    public GameObject this_obj_mana;
    CompImage mana_bar;
    public float regen = 1.0f;
    public float max_mana = 100.0f;
    float curr_mana = 100.0f;
    float calc_mana = 100.0f;

    void Start()
    {
        this_obj_mana = GetLinkedObject("this_obj_mana");
    }

    void Update()
    {
        if (curr_mana < max_mana)
        {
            curr_mana += regen;
            if (curr_mana > max_mana)
            {
                curr_mana = max_mana;
            }
            calc_mana = curr_mana / max_mana;
            mana_bar = this_obj_mana.GetComponent<CompImage>();
            mana_bar.FillAmount(calc_mana);
        }
    }

    public void DecreaseMana(float cost)
    {
        if (curr_mana > cost)
        {
            curr_mana -= cost;
        }
        calc_mana = curr_mana / max_mana;
        mana_bar = this_obj_mana.GetComponent<CompImage>();
        mana_bar.FillAmount(calc_mana);
    }

    public void DecreaseManaPercentage(float cost_percentage)
    {
        float cost = cost_percentage * max_mana / 100.0f;

        if (curr_mana > cost)
        {
            curr_mana -= cost;
        }
        calc_mana = curr_mana / max_mana;
        mana_bar = this_obj_mana.GetComponent<CompImage>();
        mana_bar.FillAmount(calc_mana);
    }

    public float GetCurrentMana()
    {
        return curr_mana;
    }

    public void SetMana(float curr_man, float max_man)
    {
        curr_mana = curr_man;
        max_mana = max_man;

        calc_mana = curr_mana / max_mana;
        mana_bar = this_obj_mana.GetComponent<CompImage>();
        mana_bar.FillAmount(calc_mana);
    }
}