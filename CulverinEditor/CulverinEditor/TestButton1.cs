using CulverinEditor;
using CulverinEditor.Debug;

public class TestButton1 : CulverinBehaviour
{
    string name = "Sae";
    void Start()
    {
    }

    void Update()
    {

    }

    void OnClick()
    {
        Debug.Log(name);
    }
}