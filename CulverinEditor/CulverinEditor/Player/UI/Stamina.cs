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
    public float stamina_flickering_time = 0.15f;
    float flickering_time = 0.0f;
    bool wasted_stamina = false;
    float wasted_stamina_time = 0.0f;
    public float wait_for_stamina_recovery = 0.2f;
    void Start()
    {
        this_obj_stamina = GetLinkedObject("this_obj_stamina");
        other_bar_lastamina = GetLinkedObject("other_bar_lastamina");
        not_enough_stamina = false;
        wasted_stamina = false;
        flickering_time = 0.0f;
        wasted_stamina_time = 0.0f;
    }

    void Update()
    {
        if (!wasted_stamina)
        {
            if (curr_stamina < max_stamina)
            {
                curr_stamina += regen;
                if (curr_stamina > max_stamina)
                {
                    curr_stamina = max_stamina;
                }
                calc_stamina = curr_stamina / max_stamina;
                stamina_bar = this_obj_stamina.GetComponent<CompImage>();
                stamina_bar.FillAmount(calc_stamina);
            }
        }
        else
        {
            wasted_stamina_time += Time.deltaTime;
            if (wasted_stamina_time >= wait_for_stamina_recovery)
            {
                wasted_stamina = false;
            }
        }

        if (not_enough_stamina)
        {
            flickering_time += Time.deltaTime;
            if (flickering_time >= stamina_flickering_time)
            {
                this_obj_stamina.GetComponent<CompImage>().DeactivateRender();
            }
            if (flickering_time >= stamina_flickering_time * 2)
            {
                this_obj_stamina.GetComponent<CompImage>().ActivateRender();
            }
            if (flickering_time >= stamina_flickering_time * 3)
            {
                this_obj_stamina.GetComponent<CompImage>().DeactivateRender();
            }
            if (flickering_time >= stamina_flickering_time * 4)
            {
                this_obj_stamina.GetComponent<CompImage>().ActivateRender();
                not_enough_stamina = false;
            }
        }
    }

    public void DecreaseStamina(float cost)
    {
        //Costs are 0 in GOD MODE
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().god_mode ||
            GetLinkedObject("player_obj").GetComponent<CharactersManager>().no_energy)
        {
            cost = 0;
        }

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
        //Costs are 0 in GOD MODE
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().god_mode ||
            GetLinkedObject("player_obj").GetComponent<CharactersManager>().no_energy)
        {
            value = 0;
        }

        if (curr_stamina >= value)
        {
            wasted_stamina = true;
            wasted_stamina_time = 0.0f;
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