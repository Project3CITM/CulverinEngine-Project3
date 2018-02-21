using CulverinEditor;
using CulverinEditor.Debug;

public class Stamina : CulverinBehaviour
{
    GameObject stamina_obj;
    CompImage stamina_bar;
    float regen = 0.005f;
    float max_stamina = 1.0f;
    float actual_stamina = 1.0f;
    float stamina_cost = 0.3f;

    void Start()
    {
        stamina_bar = stamina_obj.GetComponent<CompImage>();
    }

    void Update()
    {
        if(actual_stamina<max_stamina)
        {
            actual_stamina += regen;
            //if(actual_stamina > max_stamina)
            //{
            //    actual_stamina = max_stamina;
            //}
        }
        GetDamage(stamina_cost);
    }

    void GetDamage(float cost)
    {
        if (actual_stamina > cost)
        {
            actual_stamina -= stamina_cost;
        }
        stamina_bar.FillAmount(actual_stamina);
    }
}