using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class DamageFeedback: CulverinBehaviour
{
    Material mat;
    bool test;
    void Start()
    {
        mat = GetMaterialByName("Final Tex Material");
        test = false;
    }
    void Update()
    {
        mat.SetBool("test", test);

        if (Input.GetKeyDown(KeyCode.Q))
            test = !test;

      
    }


}