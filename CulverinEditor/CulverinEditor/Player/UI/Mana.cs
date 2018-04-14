using CulverinEditor;
using CulverinEditor.Debug;

public class Mana : CulverinBehaviour
{
    public GameObject this_obj_mana;
    public GameObject other_obj_lastmana;
    CompImage mana_bar;
    public float regen = 1.0f;
    public float max_mana = 100.0f;
    float curr_mana = 100.0f;
    float calc_mana = 100.0f;
    bool not_enough_mana = false;
    float flickering_time = 0.0f;
    void Start()
    {
        this_obj_mana = GetLinkedObject("this_obj_mana");
        other_obj_lastmana = GetLinkedObject("other_obj_lastmana");
        not_enough_mana = false;
        flickering_time = 0.0f;
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

        if (not_enough_mana)
        {
            flickering_time += Time.deltaTime;
            if (flickering_time >= 0.2)
            {
                this_obj_mana.GetComponent<CompImage>().DeactivateRender();
            }
            if (flickering_time >= 0.4)
            {
                this_obj_mana.GetComponent<CompImage>().ActivateRender();
            }
            if (flickering_time >= 0.6)
            {
                this_obj_mana.GetComponent<CompImage>().DeactivateRender();
            }
            if (flickering_time >= 0.8)
            {
                this_obj_mana.GetComponent<CompImage>().ActivateRender();
                not_enough_mana = false;
            }
        }
    }

    public void DecreaseMana(float cost)
    {
        other_obj_lastmana.GetComponent<LeftMana>().lastmana_value = curr_mana;
        if (curr_mana > cost)
        {
            curr_mana -= cost;
        }
        calc_mana = curr_mana / max_mana;
        mana_bar = this_obj_mana.GetComponent<CompImage>();
        mana_bar.FillAmount(calc_mana);
        other_obj_lastmana.GetComponent<LeftMana>().mana_bar_changed = false;
    }

    public void DecreaseManaPercentage(float cost_percentage)
    {
        float cost = cost_percentage * max_mana / 100.0f;
        other_obj_lastmana.GetComponent<LeftMana>().lastmana_value = curr_mana;
        if (curr_mana > cost)
        {
            curr_mana -= cost;
        }
        calc_mana = curr_mana / max_mana;
        mana_bar = this_obj_mana.GetComponent<CompImage>();
        mana_bar.FillAmount(calc_mana);
        other_obj_lastmana.GetComponent<LeftMana>().mana_bar_changed = false;
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

    public bool CanWasteMana(float value)
    {
        if (curr_mana >= value)
        {
            return true;
        }
        else
        {
            other_obj_lastmana.GetComponent<LeftMana>().lastmana_value = curr_mana;
            other_obj_lastmana.GetComponent<LeftMana>().current_lastmana = curr_mana;
            other_obj_lastmana.GetComponent<LeftMana>().mana_bar_changed = false;
            flickering_time = 0.0f;
            not_enough_mana = true;
            return false;
        }
    }
}