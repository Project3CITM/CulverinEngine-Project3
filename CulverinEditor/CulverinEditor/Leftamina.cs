using CulverinEditor;
using CulverinEditor.Debug;

public class Leftamina : CulverinBehaviour
{

    public GameObject this_obj_lastamina;
    public GameObject stamina_bar;
    public float lastamina_value = 0.0f;
    public float current_lastamina = 0.0f;
    public float dissapear_vel = 0.1f;
    public bool stamina_bar_changed = false;

    void Start()
    {
        this_obj_lastamina = GetLinkedObject("this_obj_lastamina");
        stamina_bar = GetLinkedObject("stamina_bar");
        stamina_bar_changed = false;
        this_obj_lastamina.GetComponent<CompImage>().FillAmount(stamina_bar.GetComponent<Stamina>().GetCurrentStamina());
        lastamina_value = stamina_bar.GetComponent<Stamina>().GetCurrentStamina();
        current_lastamina = lastamina_value;
    }

    void Update()
    {
        if(lastamina_value != stamina_bar.GetComponent<Stamina>().GetCurrentStamina() && !stamina_bar_changed)
        {
            current_lastamina = lastamina_value;
            lastamina_value = stamina_bar.GetComponent<Stamina>().GetCurrentStamina();
          
            stamina_bar_changed = true;
        }

        if(stamina_bar_changed)
        {
            current_lastamina -= dissapear_vel;
            if (current_lastamina >= 0)
            {
                this_obj_lastamina.GetComponent<CompImage>().FillAmount((current_lastamina/100.0f));
            }
            if(current_lastamina <= lastamina_value)
            {
                stamina_bar_changed = false;
            }
        }
    }
}