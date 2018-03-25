using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class LightFlickering : CulverinBehaviour
{
    int intensity;
    CompLight light;
    void Start()
    {
        light = GetComponent<CompLight>();
    }
    void Update()
    {
        intensity = light.GetIntensity();
        Debug.Log(intensity.ToString());
    }


}