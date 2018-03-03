using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelMovement : CulverinBehaviour
{

    public GameObject instance;

    public float movSpeed;
    public float length;
    public float stop_time;
    public float stop_length;
    Transform trans;
    float time = 0;
 
    float advanced_distance;
    int rand = 3;
    void Start()
    {
        trans = gameObject.GetComponent<Transform>();

    }
    void Update()
    {      
       
        time += Time.DeltaTime();
        if (time > stop_time)
        {
            Vector3 new_pos = new Vector3(trans.local_position.x + 1, trans.local_position.y, trans.local_position.z);
            trans.local_position = Vector3.MoveTowards(trans.local_position, new_pos, movSpeed * Time.DeltaTime());
            advanced_distance += movSpeed * Time.DeltaTime();
            //if(time >= 2.0f)

        }
       
      
        if (advanced_distance >= stop_length )
        {
            advanced_distance = 0;
            time = 0;
            
        }
         Vector3 diff = trans.local_position - new Vector3(0, 0, 0);
           if(diff.Length > length)
           {
           
               trans.local_position = new Vector3(0, 0, 0);
           }

    
    }
}