using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;
using CulverinEditor.Map;
using System.Text;

public class PerceptionSightEnemy : CulverinBehaviour
{
    //public GameObject player_obj;

    public int frustum_lenght = 3;
    public int frustum_size = 1;
    public int tile_size = 2;

    //Player debuging options until I can get the player position from the other script, which brokes atm
    public int player_x = 1;
    public int player_y = 2;

    bool player_seen = false;

    public enum DIRECTION
    {
        E_DIR_NONE = -1,
        E_DIR_NORTH,
        E_DIR_SOUTH,
        E_DIR_EAST,
        E_DIR_WEST,
        E_DIR_MAX
    }

    //Map settings
    int[,] tile_map;
    int map_width = 0;
    int map_height = 0;

    void Start()
    {
        // player_obj = GetLinkedObject("player_obj");

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
        //TODO: Remove all logs in the update!
        //TODO: Optimization --> Check if player is out of range
        if (true)
        {
            //  Debug.Log("Player X: " + player_obj.GetComponent<MovementController>().curr_x);
            //  Debug.Log("Player Y: " + player_obj.GetComponent<MovementController>().curr_y);
            //  Debug.Log("Player X: " + player_x);
            //  Debug.Log("Player Y: " + player_y);
            //  Debug.Log("Current Tile: (" + GetCurrentTileX() + ", " + GetCurrentTileY() + ")");
            //  Debug.Log("Frustum size: " + frustum_size);
            //  Debug.Log("Frustum lenght: " + frustum_lenght);
            bool search_finished = false;
            List<int> blocked_tiles_x = new List<int>();
            List<int> blocked_tiles_y = new List<int>();

            switch (GetDirection())
            {
                case DIRECTION.E_DIR_NONE:
                       Debug.Log("Direction Error: AI Can't see anything!");
                    break;
                case DIRECTION.E_DIR_NORTH:
                    for (int j = 0; j < frustum_lenght; j++)
                    {
                        int tile_y = (GetCurrentTileY() - j);
                        if (tile_y >= 0 && tile_y < map_height)
                        {
                            for (int i = -j; i <= j + (frustum_size - 1); i++)
                            {
                                int tile_x = (GetCurrentTileX() + i - ((frustum_size - 1) / 2));
                                if (tile_x >= 0 && tile_x < map_width)
                                {
                                    bool cant_see = false;
                                    for (int k = 0; k < blocked_tiles_x.Count; k++)
                                    {
                                        if (tile_y < blocked_tiles_y[k])
                                        {
                                            if (i > 0)
                                            {
                                                if (tile_x >= blocked_tiles_x[k])
                                                {
                                                    cant_see = true;
                                                    break;
                                                }
                                            }
                                            else if (i == 0)
                                            {
                                                if (tile_x == blocked_tiles_x[k])
                                                {
                                                    cant_see = true;
                                                    break;
                                                }
                                            }
                                            else
                                            {
                                                if (tile_x <= blocked_tiles_x[k])
                                                {
                                                    cant_see = true;
                                                    break;
                                                }
                                            }
                                        }
                                    }

                                    if (tile_map[tile_x, tile_y] == 1)
                                    {
                                        blocked_tiles_x.Add(tile_x);
                                        blocked_tiles_y.Add(tile_y);
                                    }

                                    //if (player_obj.GetComponent<MovementController>().curr_x == tile_x && player_obj.GetComponent<MovementController>().curr_y == tile_y)
                                    if (player_x == tile_x && player_y == tile_y)
                                    {
                                        search_finished = true;
                                        if (cant_see == true)
                                        {
                                            player_seen = false;
                                            break;
                                        }

                                        if (player_seen == false)
                                        {
                                            //TODO: GetComponent<Movement_Action>().GoTo()
                                            player_seen = true;
                                        }
                                        break;
                                    }
                                }
                            }
                        }

                        if (search_finished == true)
                            break;
                        else if (j == (frustum_lenght - 1))    // If player is out of range
                            player_seen = false;
                    }
                    break;
                case DIRECTION.E_DIR_SOUTH:
                    for (int j = 0; j < frustum_lenght; j++)
                    {
                           Debug.Log("j: " + j);
                        int tile_y = (GetCurrentTileY() + j);
                        if (tile_y >= 0 && tile_y < map_height)
                        {
                            for (int i = -j; i <= j + (frustum_size - 1); i++)
                            {
                                int tile_x = (GetCurrentTileX() + i - ((frustum_size - 1) / 2));
                                if (tile_x >= 0 && tile_x < map_width)
                                {
                                     Debug.Log("i: " + i);
                                     Debug.Log("Checking Tile: (" + tile_x + ", " + tile_y + ")");
                                    bool cant_see = false;
                                     Debug.Log("Num blocked tiles: " + blocked_tiles_x.Count);
                                    for (int k = 0; k < blocked_tiles_x.Count; k++)
                                    {
                                           Debug.Log("Blocked Tile: (" + blocked_tiles_x[k] + ", " + blocked_tiles_y[k] + ")");
                                        if (tile_y > blocked_tiles_y[k])
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

                                    //if (player_obj.GetComponent<MovementController>().curr_x == tile_x && player_obj.GetComponent<MovementController>().curr_y == tile_y)
                                    if (player_x == tile_x && player_y == tile_y)
                                    {
                                        search_finished = true;
                                        if (cant_see == true)
                                        {
                                            player_seen = false;
                                            Debug.Log("Player Is OUTSIDE Vision");
                                            break;
                                        }

                                        if (player_seen == false)
                                        {
                                            //TODO: GetComponent<Movement_Action>().GoTo()
                                            player_seen = true;
                                        }
                                        Debug.Log("Player Is INSIDE Vision");
                                        break;
                                    }
                                }
                            }
                        }

                        if (search_finished == true)
                            break;
                        else if (j == (frustum_lenght - 1))    // If player is out of range
                            player_seen = false;                        
                    }
                    Debug.Log("Player Is being seen? " + player_seen);
                    break;
                case DIRECTION.E_DIR_EAST:
                    break;
                case DIRECTION.E_DIR_WEST:
                    break;
                case DIRECTION.E_DIR_MAX:
                default:
                    Debug.Log("Direction Error: AI Can't see anything!");
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

    public DIRECTION GetDirection()
    {
        DIRECTION ret = DIRECTION.E_DIR_NONE;

        Vector3 forward = new Vector3(GetComponent<Transform>().GetForwardVector());
        float own_angle = Mathf.Atan2(forward.x, forward.z);
        own_angle *= Mathf.RadToDeg;

        if (own_angle <= 45 && own_angle >= -45)
            ret = DIRECTION.E_DIR_NORTH;
        else if (own_angle >= 45 && own_angle <= 135)
            ret = DIRECTION.E_DIR_EAST;
        else if (own_angle >= 135 || own_angle <= -135)
            ret = DIRECTION.E_DIR_SOUTH;
        else if (own_angle <= -45 && own_angle >= -135)
            ret = DIRECTION.E_DIR_WEST;

        return ret;
    }
}