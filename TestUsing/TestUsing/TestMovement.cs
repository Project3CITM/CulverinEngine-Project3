using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

//Attach this script to the tank parent object if you want to see it rotate
public class TestMovement : CulverinBehaviour
{
    private float movSpeed = 0.0f;
    Vector3 endPosition;
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
        for (int i = 0; i < map_width; i++)
        {
            for (int j = 0; j < map_height; j++)
            {
                array2Da[i, j] = 0;
            }
        }
        //array2Da[0,0] = 1;
        //Debug.Log(array2Da[0,0].ToString());
        endPosition = GetComponent<Transform>().Position;
        string map = Map.GetMapString();
        Debug.Log(map);
        int t = 0;
        for (int i = 0; i < map_width; i++)
        {
            for (int j = 0; j < map_height; j++)
            {
                array2Da[i, j] = int.Parse(map[t].ToString());
                t += 1;
            }
        }

        //Search player position
        for (int i = 0; i < map_width; i++)
        {
            for (int j = 0; j < map_height; j++)
            {
                if (array2Da[i, j] == 3)
                {
                    curr_x = i;
                    curr_y = j;
                    array2Da[i, j] = 1;
                }
            }
        }
    }

    void Update()
    {
        GetComponent<Transform>().Position = Vector3.MoveTowards(GetComponent<Transform>().Position, endPosition, movSpeed * Time.DeltaTime());
        if (GetComponent<Transform>().Position == endPosition)
        {
            //2D coordinates, y=z in 3D coordinates
            int tile_mov_x = 0;
            int tile_mov_y = 0;


            if (Input.KeyDown(KeyCode.A)) //Left
            {
                tile_mov_x =  - 1;
            }
            else if (Input.KeyDown(KeyCode.D)) //Right
            {
                tile_mov_x = 1;
            }
            else if (Input.KeyDown(KeyCode.W)) //Up
            {
                tile_mov_y = 1;
            }
            else if (Input.KeyDown(KeyCode.S)) //Down
            {
                tile_mov_y = - 1;
            }

            //Calculate endPosition
            if ((tile_mov_x != 0 || tile_mov_y != 0) && array2Da[curr_x + tile_mov_x, curr_y + tile_mov_y] == 1)
            {
                endPosition = new Vector3(GetComponent<Transform>().Position.x + distanceToMove * tile_mov_x, GetComponent<Transform>().Position.y, GetComponent<Transform>().Position.z + distanceToMove * tile_mov_y);
                curr_x += tile_mov_x;
                curr_y += tile_mov_y;
            }
        }
    }
}