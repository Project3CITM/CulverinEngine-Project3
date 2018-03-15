using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelScript : CulverinBehaviour
{

    public GameObject parent;
    
    public Transform trans;



    void Start()
    {
        trans = GetComponent<Transform>();
        

    }
    void Update()
    {
        
         //   trans.local_position = parent.GetComponent<Transform>().local_position;

    }

   
}