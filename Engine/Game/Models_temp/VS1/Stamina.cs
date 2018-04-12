using CulverinEditor;
using CulverinEditor.Debug;

public class Stamina : CulverinBehaviour
{
    CompImage stamina_bar;
    GameObject this_obj_stamina;
    float regen = 1.0f;
    float max_stamina = 100.0f;
    float curr_stamina = 100.0f;
    float calc_stamina = 100.0f;

    void Start()
    {
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
            this_obj_stamina = GetLinkedObject("this_obj_stamina");
            stamina_bar = this_obj_stamina.GetComponent<CompImage>();
            stamina_bar.FillAmount(calc_stamina);
        }
    }

    public void DecreaseStamina(float cost)
    {
        if (curr_stamina > cost)
        {
            curr_stamina -= cost;
        }
        calc_stamina = curr_stamina / max_stamina;
        this_obj_stamina = GetLinkedObject("this_obj_stamina");
        stamina_bar = this_obj_stamina.GetComponent<CompImage>();
        stamina_bar.FillAmount(calc_stamina);

        Debug.Log("Decrease Stamina -> Current Stamina: " + curr_stamina.ToString());
    }

    public float GetCurrentStamina()
    {
        return curr_stamina;
    }
}