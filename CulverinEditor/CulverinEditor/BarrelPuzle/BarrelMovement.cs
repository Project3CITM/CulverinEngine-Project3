using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelMovement : CulverinBehaviour
{

    public GameObject instance;
    BarrelManage manage;


    float length;
    Transform trans;
    bool test = false;


    void Start()
    {
        instance = GetLinkedObject("instance");
        manage = instance.GetComponent<BarrelManage>();
        trans = gameObject.GetComponent<Transform>();
     

    }
    void Update()
    {
        instance = GetLinkedObject("instance");
        manage = instance.GetComponent<BarrelManage>();
        trans = gameObject.GetComponent<Transform>();
        length = manage.length;

      
             
       
         Vector3 diff = trans.local_position - new Vector3(0, 0, 0);
        if(diff.Length >= manage.length)
        {              
               trans.local_position = new Vector3(0, 0, 0);
             
        }
        if (!manage.stop)
        {
            trans.local_position = new Vector3(trans.local_position.x + manage.movSpeed*manage.p_dt, trans.local_position.y, trans.local_position.z);
           
        }
        
        else
        {
            trans.local_position = new Vector3(Mathf.Round(trans.local_position.x), trans.local_position.y, trans.local_position.z);
            
        }
       
    
    }
}