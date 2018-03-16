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
        target_pos_x = target_tile_x * tile_size;
        target_pos_y = target_tile_y * tile_size;
    }

    void Update()
    {
        if(in_tile == false)
        {
            bool in_x = false;
            bool in_y = false;

            //Displace barrel in X
            if (Mathf.Abs(target_pos_x - transform.position.x) > error_margin)
            {
                transform.position.x += Time.deltaTime * speed;
            }
            else in_x = true;

            //Displace barrel in Y
            if (Mathf.Abs(target_pos_y - transform.position.z) > error_margin)
            {
                transform.position.y += Time.deltaTime * speed;
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

        }
    }
   
}
