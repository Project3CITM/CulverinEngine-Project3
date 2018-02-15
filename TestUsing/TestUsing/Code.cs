using CulverinEditor;
using CulverinEditor.Debug;

public class Code : CulverinBehaviour
{
    Codev1 test;

    void Start()
    {
        test = GetComponent<Codev1>();
    }

    void Update()
    {
        Debug.Log("---");
        test.Test();
        Debug.Log("---");
    }
}