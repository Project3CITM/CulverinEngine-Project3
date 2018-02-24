using CulverinEditor;
using CulverinEditor.Debug;

public class Stamina : CulverinBehaviour
{
    CompImage stamina_bar;
    GameObject this_obj;
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
        Debug.Log("3");
        this_obj = GetLinkedObject("this_obj");
        stamina_bar = this_obj.GetComponent<CompImage>();
        Debug.Log("4");
        Debug.Log(gameObject.GetName());
        Debug.Log(this_obj.GetName());
        stamina_bar.FillAmount(calc_stamina);

        Debug.Log("Decrease Stamina -> Current Stamina: " + curr_stamina.ToString());
    }

    public float GetCurrentStamina()
    {
        return curr_stamina;
    }
}