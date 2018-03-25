using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class LightFlickering : CulverinBehaviour
{
    float intensity;
    CompLight light;
    public float max;
    public float period;
    public float random_factor;

    float time = 0;
    void Start()
    {
        light = GetComponent<CompLight>();
        time = 0;
        intensity = light.GetIntensity();
    }
    void Update()
    {
        time += Time.deltaTime;

        float function = ((Mathf.Sin(2 * Mathf.PI * time * period) +  Mathf.Sin(2 * Mathf.PI * 1*33 *time * period)) / 2);
        float new_intensity = intensity +  function * max;
   
        light.SetIntensity(new_intensity);
     
    }


}