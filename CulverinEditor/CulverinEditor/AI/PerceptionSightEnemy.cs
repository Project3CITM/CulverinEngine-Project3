using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;
using CulverinEditor.Map;

public class PerceptionSightEnemy : CulverinBehaviour
{
    public int frustum_lenght = 3;
    public int frustum_size = 1;
    public int tile_size = 2;
    //public GameObject player;

    //Map settings
    int[,] tile_map;
    int map_width = 0;
    int map_height = 0;

    void Start()
    {
        Debug.Log("PerceptionSightEnemy Start");
        //player = GetLinkedObject("player");

        //Map Settings
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();

        tile_map = new int[map_width, map_height];

        string map = Map.GetMapString();
        int string_pos = 0;

        //Get the map and put it in array 2d
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                //Parse needs using System.Text
                tile_map[x, y] = int.Parse(map[string_pos].ToString());
                string_pos += 1;
            }
        }
    }

    void Update()
    {
        //TODO: Optimization --> Check if player is out of range
        if (true)
        {
            // Debug.Log("Player X: " + player.GetComponent<MovementController>().curr_x);
            // Debug.Log("Player Y: " + player.GetComponent<MovementController>().curr_y);
            Debug.Log("Current Tile: (" + GetCurrentTileX() + ", " + GetCurrentTileY() + ")");
            Debug.Log("Frustum size: " + frustum_size);
            Debug.Log("Frustum lenght: " + frustum_lenght);
            bool search_finished = false;
            List<int> blocked_tiles_x = new List<int>();
            List<int> blocked_tiles_y = new List<int>();
            //TODO: Directions --> switch()
            //NORTH:
            for (int j = 0; j < frustum_lenght; j++)
            {
                Debug.Log("j: " + j);
                int tile_y = (GetCurrentTileY() - j);
                for (int i = -j; i <= j + (frustum_size - 1); i++)
                {
                    int tile_x = (GetCurrentTileX() + i - ((frustum_size - 1) / 2));
                    Debug.Log("i: " + i);
                    Debug.Log("Checking Tile: (" + tile_x + ", " + tile_y + ")");
                    bool cant_see = false;
                    Debug.Log("Num blocked tiles: " + blocked_tiles_x.Count);
                    for (int k = 0; k < blocked_tiles_x.Count; k++)
                    {
                        Debug.Log("Blocked Tile: (" + blocked_tiles_x[k] + ", " + blocked_tiles_y[k] + ")");
                        if (tile_y < blocked_tiles_y[k])
                        {
                            if (i > 0)
                            {
                                if (tile_x >= blocked_tiles_x[k])
                                {
                                    cant_see = true;
                                    Debug.Log("Can't See Tile");
                                    break;
                                }
                            }
                            else if (i == 0)
                            {
                                if (tile_x == blocked_tiles_x[k])
                                {
                                    cant_see = true;
                                    Debug.Log("Can't See Tile");
                                    break;
                                }
                            }
                            else
                            {
                                if (tile_x <= blocked_tiles_x[k])
                                {
                                    cant_see = true;
                                    Debug.Log("Can't See Tile");
                                    break;
                                }
                            }
                        }
                    }

                    if (tile_map[tile_x, tile_y] == 1)
                    {
                        Debug.Log("Non Walkable Tile: " + tile_x + ", " + tile_y);
                        blocked_tiles_x.Add(tile_x);
                        blocked_tiles_y.Add(tile_y);
                    }

                    //TODO: Testing with hardcoded player position (2,2)
                    if (2 == tile_x && 2 == tile_y) // player.GetComponent<MovementController>().curr_x // player.GetComponent<MovementController>().curr_y
                    {
                        search_finished = true;
                        if (cant_see == true)
                        {
                            Debug.Log("Player Is OUTSIDE Vision");
                            break;
                        }

                        Debug.Log("Player Is INSIDE Vision");
                        break;
                    }
                }
                if (search_finished == true)
                    break;
            }
        }
    }

    public int GetCurrentTileX()
    {
        return (int)((float)GetComponent<Transform>().local_position.x / tile_size);
    }

    public int GetCurrentTileY()
    {
        return (int)((float)GetComponent<Transform>().local_position.z / tile_size);
    }
}