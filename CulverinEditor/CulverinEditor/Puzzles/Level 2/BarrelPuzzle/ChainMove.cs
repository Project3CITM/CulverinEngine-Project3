using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class ChainMove : CulverinBehaviour
{
    public float speed = 1.0f;
    public float floor_height = 0.0f;

    private bool moving = false;
    private Vector3 puzzle_ceter = Vector3.Zero;
    private Vector3 target_pos = Vector3.Zero;
    private float puzzle_width = 0;
    private float puzzle_height = 0;

    void Start()
    {
        target_pos = new Vector3(0.0f, floor_height, 0.0f);
        puzzle_ceter = new Vector3(0.0f, floor_height, 0.0f);
        target_pos.y = floor_height;
        puzzle_ceter.y = floor_height;
    }

    void Update()
    {
        if (moving)
        {
            MoveToTile();
        }
    }

    void MoveToTile()
    {
        Vector3 global_pos = transform.GetGlobalPosition();

        bool in_x = false;
        bool in_y = false;

        //Displace barrel in X
        if (Mathf.Abs(target_pos.x - global_pos.x) > 1)
        {
            float displacement;
            if (target_pos.x - global_pos.x < 0)
            {
                displacement = -1;
            }
            else displacement = 1;


            transform.SetGlobalPosition(new Vector3(global_pos.x + (Time.deltaTime * speed * displacement), global_pos.y, global_pos.z));
        }
        else in_x = true;

        //Displace barrel in Y
        if (in_x == true && Mathf.Abs(target_pos.z - global_pos.z) > 1.0f)
        {
            float displacement;
            if (target_pos.z - global_pos.z < 0)
            {
                displacement = -1;
            }
            else displacement = 1;

            transform.SetGlobalPosition(new Vector3(global_pos.x, global_pos.y, global_pos.z + (Time.deltaTime * speed * displacement)));
        }
        else in_y = true;

        if (in_x && in_y)
        {
            transform.SetGlobalPosition(puzzle_ceter);
        }
    }

    public void SetMove(bool active)
    {
        moving = active;
    }

    public void SetData(float new_speed, float new_start_tile_x, float new_start_tile_y, float new_target_tile_x, float new_target_tile_y)
    {
        speed = new_speed;

        target_pos.x = new_target_tile_x * 25.4f;
        target_pos.z = new_target_tile_y * 25.4f;

        puzzle_width = new_target_tile_x - new_start_tile_x;
        puzzle_height = new_target_tile_y - new_start_tile_y;

        puzzle_ceter = new Vector3(target_pos);

        // Add margin before tp
        if (new_start_tile_y == new_target_tile_y)
        {
            if (new_start_tile_x <= new_target_tile_x)
            {
                target_pos.x += 25.4f;
                puzzle_ceter.x -= 25.4f;
            }
            else
            {
                target_pos.x -= 25.4f;
                puzzle_ceter.x += 25.4f;
            }
        }
        else
        {
            if (new_start_tile_y <= new_target_tile_y)
            {
                target_pos.z += 25.4f;
                puzzle_ceter.z -= 25.4f;
            }
            else
            {
                target_pos.z -= 25.4f;
                puzzle_ceter.z += 25.4f;
            }

        }

        puzzle_ceter.x -= puzzle_width * 25.4f;
        puzzle_ceter.z -= puzzle_height * 25.4f;
    }
}