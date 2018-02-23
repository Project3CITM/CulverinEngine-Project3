using CulverinEditor;
using CulverinEditor.Debug;

public class TestButton2 : CulverinBehaviour
{
    string name = "Y lo";
    void Start()
    {
    }

    void Update()
    {

    }

    void OnlClick()
    {
        Debug.Log(name);
    }

}