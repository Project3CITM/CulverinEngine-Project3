using CulverinEditor;
using CulverinEditor.Debug;

public class Stamina : CulverinBehaviour
{
    public GameObject this_obj_stamina;
    CompImage stamina_bar;
    public GameObject other_bar_lastamina;
    public float regen = 1.0f;
    public float max_stamina = 100.0f;
    float curr_stamina = 100.0f;
    float calc_stamina = 100.0f;

    void Start()
    {
        this_obj_stamina = GetLinkedObject("this_obj_stamina");
        other_bar_lastamina = GetLinkedObject("other_bar_lastamina");
    }

    void Update()
    {
        if(curr_stamina < max_stamina)
        {
            curr_stamina += regen;
            if(curr_stamina > max_stamina)
            {
                curr_stamina = max_stamina;
            }
            calc_stamina = curr_stamina / max_stamina;
            stamina_bar = this_obj_stamina.GetComponent<CompImage>();
            stamina_bar.FillAmount(calc_stamina);
        }
    }

    public void DecreaseStamina(float cost)
    {
        other_bar_lastamina.GetComponent<Leftamina>().lastamina_value = curr_stamina;
        if (curr_stamina > cost)
        {
            curr_stamina -= cost;
        }
        calc_stamina = curr_stamina / max_stamina;
        stamina_bar = this_obj_stamina.GetComponent<CompImage>();
        stamina_bar.FillAmount(calc_stamina);
        other_bar_lastamina.GetComponent<Leftamina>().stamina_bar_changed = false;
     
    }

    public float GetCurrentStamina()
    {
        return curr_stamina;
    }

    public void SetStamina(float curr_stam, float max_stam)
    {
        curr_stamina = curr_stam;
        max_stamina = max_stam;

        calc_stamina = curr_stamina / max_stamina;
        stamina_bar = this_obj_stamina.GetComponent<CompImage>();
        stamina_bar.FillAmount(calc_stamina);
    }

    public bool CanWasteStamina(float value)
    {
        if(curr_stamina >= value)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}