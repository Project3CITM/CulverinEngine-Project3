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
    bool not_enough_stamina = false;
    int flickering = 0;
    float flickering_time = 0.0f;
    void Start()
    {
        this_obj_stamina = GetLinkedObject("this_obj_stamina");
        other_bar_lastamina = GetLinkedObject("other_bar_lastamina");
        not_enough_stamina = false;
        flickering = 0;
        flickering_time = 0.0f;
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

        if(not_enough_stamina)
        {
            flickering_time += Time.deltaTime;
            if(flickering_time >= 0.2)
            {
                this_obj_stamina.GetComponent<CompImage>().DeactivateRender();
            }
            if (flickering_time >= 0.4)
            {
                this_obj_stamina.GetComponent<CompImage>().ActivateRender();
            }
            if (flickering_time >= 0.6)
            {
                this_obj_stamina.GetComponent<CompImage>().DeactivateRender();
            }
            if (flickering_time >= 0.8)
            {
                this_obj_stamina.GetComponent<CompImage>().ActivateRender();
                not_enough_stamina = false;
            }
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
            other_bar_lastamina.GetComponent<Leftamina>().lastamina_value = curr_stamina;
            other_bar_lastamina.GetComponent<Leftamina>().current_lastamina = curr_stamina;
            other_bar_lastamina.GetComponent<Leftamina>().stamina_bar_changed = false;
            flickering_time = 0.0f;
            not_enough_stamina = true;
            return false;
        }
    }
}