using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

//Attach this script to the tank parent object if you want to see it rotate
public class TestMovement : CulverinBehaviour
{
    public enum Direction
    {
        NORTH = 0,
        EAST,
        SOUTH,
        WEAST
    }


    public Direction curr_dir = Direction.NORTH;
    public int start_direction = 1;
    private float movSpeed = 0.0f;
    Vector3 endPosition;
    Vector3 endRotation;
    int distanceToMove = 10;
    int[,] array2Da;
    int curr_x = 0;
    int curr_y = 0;
    int map_width = 0;
    int map_height = 0;
    public bool blocked_camera = false;

    //2D coordinates, y=z in 3D coordinates
    public float speedH = 2.0f;
    public float speedV = 2.0f;

    private float yaw = 0.0f;
    private float pitch = 0.0f;

    void Start()
    {
        curr_dir = (Direction)start_direction;
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();
        array2Da = new int[map_width, map_height];
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                array2Da[x, y] = 0;
            }
        }
        //array2Da[0,0] = 1;
        //Debug.Log(array2Da[0,0].ToString());
        endPosition = GetComponent<Transform>().local_position;
        endRotation = transform.local_rotation;

        string map = Map.GetMapString();
        Debug.Log(map);
        int t = 0;
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                array2Da[x, y] = int.Parse(map[t].ToString());
                t += 1;
            }
        }

        //Search player position
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                if (array2Da[x, y] == 2)
                {
                    curr_x = x;
                    curr_y = y;
                    array2Da[x, y] = 0;
                }
            }
        }
    }

    void Update()
    {
        int tile_mov_x = 0;
        int tile_mov_y = 0;

        if (Input.GetMouseButtonRepeat(2) && !blocked_camera)
        {
            Debug.Log("Trying to rotate");
            //float rot_x = Input.GetMouseYAxis();
            //float rot_y = Input.GetMouseXAxis();
            yaw += speedH * Input.GetMouseXAxis();
            pitch -= speedV * Input.GetMouseYAxis();


            transform.local_rotation = new Vector3(pitch, yaw, 0);
            //if (rot_x != 0)
            //{
            //    transform.local_rotation = new Vector3(transform.local_rotation.x - rot_x, transform.local_rotation.y, 0);
            //}

            //if (rot_y != 0)
            //{
            //    transform.local_rotation = new Vector3(transform.local_rotation.x, transform.local_rotation.y + rot_y, 0);
            //}
        }
        else
        {
            //Quaternion rot_step = Quaternion.RotateTowards(Quaternion.FromEulerAngles(transform.rotation), Quaternion.FromEulerAngles(endRotation), movSpeed * 20 * Time.DeltaTime());
            //transform.local_rotation = rot_step.ToEulerAngles();
        }
        if (Input.GetMouseButtonUp(2))
        {
            Vector3 rot_north = Vector3.Zero;
            Vector3 rot_east = new Vector3(0, 90, 0);
            Vector3 rot_south = new Vector3(0, 180, 0);
            Vector3 rot_weast = new Vector3(0, 270, 0);

            float curr_rot_y = transform.local_rotation.y;

            if (curr_rot_y < 45 || curr_rot_y > 315)
            {
                endRotation = rot_north;
                //curr_dir = Direction.NORTH;
            }
            if (curr_rot_y > 45 && curr_rot_y < 135)
            {
                endRotation = rot_east;
                //curr_dir = Direction.EAST;
            }
            if (curr_rot_y > 135 && curr_rot_y < 225)
            {
                endRotation = rot_south;
                //curr_dir = Direction.SOUTH;
            }
            if (curr_rot_y > 255 && curr_rot_y < 315)
            {
                endRotation = rot_weast;
                //curr_dir = Direction.WEAST;
            }
        }

        if (GetComponent<Transform>().local_position == endPosition && transform.local_rotation == endRotation)
        {

            if (Input.GetKeyDown(KeyCode.Q)) //Left
            {
                endRotation = new Vector3(transform.local_rotation.x, transform.local_rotation.y - 90, 0);
            }
            if (Input.GetKeyDown(KeyCode.E)) //Left
            {
                endRotation = new Vector3(transform.local_rotation.x, transform.local_rotation.y + 90, 0);
            }

            if (Input.GetKeyDown(KeyCode.A)) //Left
            {
                MoveLeft(out tile_mov_x, out tile_mov_y);
            }
            else if (Input.GetKeyDown(KeyCode.D)) //Right
            {
                MoveRight(out tile_mov_x, out tile_mov_y);
            }
            else if (Input.GetKeyDown(KeyCode.W)) //Up
            {
                MoveForward(out tile_mov_x, out tile_mov_y);
            }
            else if (Input.GetKeyDown(KeyCode.S)) //Down
            {
                MoveBackward(out tile_mov_x, out tile_mov_y);
            }

            //Calculate endPosition
            if ((tile_mov_x != 0 || tile_mov_y != 0) && array2Da[curr_x + tile_mov_x, curr_y + tile_mov_y] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * tile_mov_x, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * tile_mov_y);
                curr_x += tile_mov_x;
                curr_y += tile_mov_y;
                Debug.Log("Should be moving");
            }
            else
            {
                //Debug.Log("Failed to move");
                //Debug.Log(tile_mov_x.ToString());
                //Debug.Log(tile_mov_y.ToString());
            }
        }
        else
        {
            GetComponent<Transform>().local_position = Vector3.MoveTowards(GetComponent<Transform>().local_position, endPosition, movSpeed * Time.DeltaTime());
        }
    }

    public void BlockCamera()
    {
        blocked_camera = true;
    }

    public void UnBlockCamera()
    {
        blocked_camera = false;
    }

    public void MoveForward(out int tile_mov_x, out int tile_mov_y)
    {
        tile_mov_x = 0;
        tile_mov_y = 0;
        if (curr_dir == Direction.NORTH)
        {
            tile_mov_y = 1;
        }
        if (curr_dir == Direction.EAST)
        {
            tile_mov_x = 1;
        }
        if (curr_dir == Direction.SOUTH)
        {
            tile_mov_y = -1;
        }
        if (curr_dir == Direction.WEAST)
        {
            tile_mov_x = -1;
        }
    }

    public void MoveRight(out int tile_mov_x, out int tile_mov_y)
    {
        tile_mov_x = 0;
        tile_mov_y = 0;
        if (curr_dir == Direction.NORTH)
        {
            tile_mov_x = 1;
        }
        if (curr_dir == Direction.EAST)
        {
            tile_mov_y = 1;
        }
        if (curr_dir == Direction.SOUTH)
        {
            tile_mov_x = -1;
        }
        if (curr_dir == Direction.WEAST)
        {
            tile_mov_y = -1;
        }
    }

    public void MoveBackward(out int tile_mov_x, out int tile_mov_y)
    {
        tile_mov_x = 0;
        tile_mov_y = 0;
        if (curr_dir == Direction.NORTH)
        {
            tile_mov_y = -1;
        }
        if (curr_dir == Direction.EAST)
        {
            tile_mov_x = -1;
        }
        if (curr_dir == Direction.SOUTH)
        {
            tile_mov_y = 1;
        }
        if (curr_dir == Direction.WEAST)
        {
            tile_mov_x = 1;
        }
    }

    public void MoveLeft(out int tile_mov_x, out int tile_mov_y)
    {
        tile_mov_x = 0;
        tile_mov_y = 0;
        if (curr_dir == Direction.NORTH)
        {
            tile_mov_x = -1;
        }
        if (curr_dir == Direction.EAST)
        {
            tile_mov_y = -1;
        }
        if (curr_dir == Direction.SOUTH)
        {
            tile_mov_x = 1;
        }
        if (curr_dir == Direction.WEAST)
        {
            tile_mov_y = 1;
        }
    }

}