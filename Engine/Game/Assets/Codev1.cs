using CulverinEditor;
using CulverinEditor.Debug;

public class Codev1 : CulverinBehaviour
{
    int a = 3;

    void Start()
    {

    }

    void Update()
    {
        Debug.Log(a.ToString());
    }

    public void Test()
    {
        //Debug.Log(a.ToString());
        
        gameObject.SetTag("uned");
        Debug.Log(gameObject.GetTag());
    }

}