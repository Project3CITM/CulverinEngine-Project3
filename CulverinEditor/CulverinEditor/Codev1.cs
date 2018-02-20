using CulverinEditor;
using CulverinEditor.Debug;

public class Codev1 : CulverinBehaviour
{
    void OnTriggerEnter()
    {
        Debug.Log(GetComponent<CompCollider>().GetCollidedObject().tag);
    }

    void OnTriggerLost()
    {
        Debug.Log("lost");
    }
}