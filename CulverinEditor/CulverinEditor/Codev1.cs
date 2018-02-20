using CulverinEditor;
using CulverinEditor.Debug;

public class Codev1 : CulverinBehaviour
{
    void Start()
    {
        GetComponent<CompButton>().Clicked();
        GetComponent<CompButton>().Deactivate();
    }

    void OnClick()
    {
        Debug.Log("button down");
    }

}