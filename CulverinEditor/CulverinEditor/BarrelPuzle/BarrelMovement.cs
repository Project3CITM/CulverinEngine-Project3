using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelMovement : CulverinBehaviour
{



    public float movSpeed;
    public float length;
    Transform trans;
    float time = 0;
    bool can_stop = true;
    void Start()
    {
        trans = gameObject.GetComponent<Transform>();

    }
    void Update()
    {      
        Debug.Log("hi");
        time += Time.DeltaTime();
        if (time > 1.0f)
        {
            Vector3 new_pos = new Vector3(trans.local_position.x + 1, trans.local_position.y, trans.local_position.z);
            trans.local_position = Vector3.MoveTowards(trans.local_position, new_pos, movSpeed * Time.DeltaTime());

            if(time >= 2.0f)
            can_stop = true;
        }
        if (can_stop)
        {
            time = 0;
            can_stop = false;
        }
         Vector3 diff = trans.local_position - new Vector3(0, 0, 0);
           if(diff.Length > length)
           {
               trans.local_position = new Vector3(0, 0, 0);
           }

    
    }
}