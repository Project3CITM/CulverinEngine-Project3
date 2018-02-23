using CulverinEditor;
using CulverinEditor.Debug;

public class Stamina : CulverinBehaviour
{
    CompImage stamina_bar;
    float regen = 0.0005f;
    float max_stamina = 100.0f;
    float curr_stamina = 100.0f;
    float calc_stamina = 100.0f;

    void Start()
    {
    }

    void Update()
    {

    }

    public void DecreaseStamina(float cost)
    {
        if (curr_stamina > cost)
        {
            curr_stamina -= cost;
        }
        calc_stamina = curr_stamina / max_stamina;
        stamina_bar = GetComponent<CompImage>();
        stamina_bar.FillAmount(calc_stamina);

        Debug.Log("Decrease Stamina -> Current Stamina: " + curr_stamina.ToString());
    }

    public float GetCurrentStamina()
    {
        return curr_stamina;
    }
}