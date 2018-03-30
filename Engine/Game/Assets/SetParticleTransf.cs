using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class SetParticleTransf : CulverinBehaviour
{
    public string name = "";
    public GameObject obj;

    CompCollider col;

    void Start()
    {
        col = GetComponent<CompCollider>();
    }

    void OnContact()
    {
      // GameObje col.GetCollidedObject
    }

}

