using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelMove : CulverinBehaviour
{   
    public float final_pos_x;
    public float final_pos_y;
    public float final_pos_z;

    public float movSpeed;

    public float fall_speed;
    public float fall_dist;
   
    public bool stop = false;
    public bool arrive = false;

    Transform trans;

    void Start()
    {
        arrive = false;
        trans = GetComponent<Transform>();
    }
    void Update()
    {
        float dt = Time.deltaTime;

        if (!arrive)
        {
            Vector3 diff = new Vector3(final_pos_x, final_pos_y, final_pos_z) - trans.local_position;
            if (diff.Length < 0.1f)
                arrive = true;

            diff = diff.Normalized;

            trans.local_position += diff * movSpeed * dt;
        }
        else
        {
            Vector3 diff = new Vector3(trans.local_position.x, final_pos_y - fall_dist, trans.local_position.z) - trans.local_position;

            diff = diff.Normalized;
            trans.local_position += diff * fall_speed * dt;
        }



    }

}