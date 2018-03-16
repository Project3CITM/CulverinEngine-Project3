using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class BarrelFall : CulverinBehaviour
{

    public float speed = 1.0f;
    public int target_tile_x = 0;
    public int target_tile_y = 0;
    public float error_margin = 0.01f;
    public float tile_size = 25.4f;
    float target_pos_x = 0.0f;
    float target_pos_y = 0.0f;
    float floor_height = 0.0f;


    bool in_tile = false;

    void OnEnabled()
    {
       
    }

    void Update()
    {
       
         if (in_tile == false)
        {
            target_pos_x = target_tile_x * tile_size;
            target_pos_y = target_tile_y * tile_size;
            bool in_x = false;
            bool in_y = false;

            //Displace barrel in X
            if (Mathf.Abs(target_pos_x - transform.local_position.x) > error_margin)
            {
                float displacement;
                if (target_pos_x - transform.local_position.x < 0)
                {
                    displacement = -1;
                }
                else displacement = 1;

                transform.local_position = new Vector3(transform.local_position.x + (Time.deltaTime * speed * displacement), transform.local_position.y, transform.local_position.z);
            }
            else in_x = true;

            //Displace barrel in Y
            if (Mathf.Abs(target_pos_y - transform.local_position.z) > error_margin)
            {
                float displacement;
                if (target_pos_y - transform.local_position.z < 0)
                {
                    displacement = -1;
                }
                else displacement = 1;

                transform.local_position = new Vector3(transform.local_position.x, transform.local_position.y, transform.local_position.z + (Time.deltaTime * speed* displacement));
            }
            else in_y = true;

            if(in_x && in_y)
            {
                //If arrived to tile
                in_tile = true;
            }
            
        }
        else
        {
            Debug.Log("Barrel Arrived");
        }
    }
   
}
