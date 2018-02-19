using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

//Attach this script to the tank parent object if you want to see it rotate
public class TestMovement : CulverinBehaviour
{
    private float movSpeed = 0.0f;
    Vector3 endPosition;
    Vector3 endRotation;
    int distanceToMove = 10;
    int[,] array2Da;
    int curr_x = 0;
    int curr_y = 0;
    int map_width = 0;
    int map_height = 0;

    void Start()
    {
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

      if(Input.GetMouseButtonRepeat(2))
        {
            Debug.Log("Trying to rotate");
            float rot_x = Input.GetMouseYAxis();
            float rot_y = Input.GetMouseXAxis();

            if (rot_x != 0)
            {
                transform.local_rotation = new Vector3(transform.local_rotation.x - rot_x, transform.local_rotation.y, 0);
            }

            if (rot_y != 0)
            {
                transform.local_rotation = new Vector3(transform.local_rotation.x, transform.local_rotation.y + rot_y, 0);
            }
        }
        else
        {

            //Quaternion rot_step = Quaternion.RotateTowards(Quaternion.FromEulerAngles(transform.rotation), Quaternion.FromEulerAngles(endRotation), movSpeed * 20 * Time.DeltaTime());
            //transform.Rotation = Quaternion.ToEulerAngles();
        }
      if(Input.GetMouseButtonUp(2))
        {
            Vector3 forwards = Vector3.Zero;
            Vector3 rightwards = new Vector3(0, 90, 0);
            Vector3 backwards = new Vector3(0, 180, 0);
            Vector3 leftwards = new Vector3(0, 270, 0);

            float curr_rot_y = transform.local_rotation.y;

            if (curr_rot_y < 45 || curr_rot_y > 315)
            {
                endRotation = forwards;
            }
            if (curr_rot_y > 45 && curr_rot_y < 135)
            {
                endRotation = rightwards;
            }
            if (curr_rot_y > 135 && curr_rot_y < 225)
            {
                endRotation = backwards;
            }
            if (curr_rot_y > 255 && curr_rot_y < 315)
            {
                endRotation = leftwards;
            }
        }

        if (GetComponent<Transform>().local_position == endPosition && transform.local_rotation == endRotation)
        {
            //2D coordinates, y=z in 3D coordinates
            int tile_mov_x = 0;
            int tile_mov_y = 0;


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
                tile_mov_x =  - 1;
            }
            else if (Input.GetKeyDown(KeyCode.D)) //Right
            {
                tile_mov_x = 1;
            }
            else if (Input.GetKeyDown(KeyCode.W)) //Up
            {
                tile_mov_y = 1;
            }
            else if (Input.GetKeyDown(KeyCode.S)) //Down
            {
                tile_mov_y = - 1;
            }

            //Calculate endPosition
            if ((tile_mov_x != 0 || tile_mov_y != 0) && array2Da[curr_x + tile_mov_x, curr_y + tile_mov_y] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * tile_mov_x, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * tile_mov_y);
                curr_x += tile_mov_x;
                curr_y += tile_mov_y;
            }
        }
        else
        {
            GetComponent<Transform>().local_position = Vector3.MoveTowards(GetComponent<Transform>().local_position, endPosition, movSpeed * Time.DeltaTime());
        }
    }
}