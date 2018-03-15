using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelMove : CulverinBehaviour
{
    
    GameObject parent_obj;
    Transform trans;

    public float final_pos_x;
    public float final_pos_y;
    public float final_pos_z;

    public float movSpeed;

    public float fall_speed;
    public float fall_dist;

    bool arrive = false;

    public bool stop = false;
    void Start()
    {
        arrive = false;
        trans = GetComponent<Transform>();
    }
    void Update()
    {
        float dt = Time.deltaTime;

        Vector3 diff = trans.local_position - new Vector3(final_pos_x, final_pos_y, final_pos_z);
        diff = diff.Normalized;

        trans.local_position = diff * movSpeed * dt;



    }

}