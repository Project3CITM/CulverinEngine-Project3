using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;
using CulverinEditor.Map;
using CulverinEditor.Pathfinding;
using System.Text;

public class PerceptionSightEnemy : CulverinBehaviour
{
    GameObject player_obj;

    public int frustum_lenght = 3;
    public int frustum_size = 1;
    public float time_to_forget = 10.0f;
    float tile_size;

    public float check_player_timer = 0.5f;
    float timer = 0.0f;

    public bool player_seen = false;

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
        player_obj = GetLinkedObject("player_obj");

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

        tile_size = GetComponent<Movement_Action>().tile_size;
        player_seen = false;
    }

    void Update()
    {
        Debug.Log("Noth");

        List<PathNode> north = GetSeenTiles(DIRECTION.E_DIR_NORTH);
        foreach(PathNode n in north)
            Debug.Log("Node: " + n.GetTileX().ToString() + "," + n.GetTileY().ToString());

        Debug.Log("East");

        List<PathNode> east = GetSeenTiles(DIRECTION.E_DIR_EAST);
        foreach (PathNode n in east)
            Debug.Log("Node: " + n.GetTileX().ToString() + "," + n.GetTileY().ToString());

        Debug.Log("West");

        List<PathNode> west = GetSeenTiles(DIRECTION.E_DIR_WEST);
        foreach (PathNode n in west)
            Debug.Log("Node: " + n.GetTileX().ToString() + "," + n.GetTileY().ToString());

        Debug.Log("South");

        List<PathNode> south = GetSeenTiles(DIRECTION.E_DIR_SOUTH);
        foreach (PathNode n in south)
            Debug.Log("Node: " + n.GetTileX().ToString() + "," + n.GetTileY().ToString());

        timer += Time.DeltaTime();

        if (timer >= check_player_timer)
        {
            timer = 0.0f;

            int player_x, player_y;
            GetPlayerTilePos(out player_x, out player_y);

            //Optimization --> Check if player is out of range
            int distance_x = Mathf.Abs(GetCurrentTileX() - player_x);
            int distance_y = Mathf.Abs(GetCurrentTileY() - player_y);

            if (distance_x <= frustum_lenght && distance_y <= frustum_lenght)
            {
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
                                                player_seen = true;
                                                GetComponent<PerceptionEmitter>().TriggerPlayerSeenEvent(time_to_forget, tile_x, tile_y, gameObject, GetCurrentTileX(), GetCurrentTileY());
                                                Debug.Log("Player Seen");
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
                            int tile_y = (GetCurrentTileY() + j);
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
                                            if (tile_y > blocked_tiles_y[k])
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
                                                player_seen = true;
                                                Debug.Log("Player Seen");
                                                GetComponent<PerceptionEmitter>().TriggerPlayerSeenEvent(time_to_forget, tile_x, tile_y, gameObject, GetCurrentTileX(), GetCurrentTileY());
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
                    case DIRECTION.E_DIR_EAST:
                        for (int j = 0; j < frustum_lenght; j++)
                        {
                            int tile_x = (GetCurrentTileX() + j);
                            if (tile_x >= 0 && tile_x < map_width)
                            {
                                for (int i = -j; i <= j + (frustum_size - 1); i++)
                                {
                                    int tile_y = (GetCurrentTileY() + i - ((frustum_size - 1) / 2));
                                    if (tile_y >= 0 && tile_y < map_height)
                                    {
                                        bool cant_see = false;
                                        for (int k = 0; k < blocked_tiles_x.Count; k++)
                                        {
                                            if (tile_x > blocked_tiles_x[k])
                                            {
                                                if (i > 0)
                                                {
                                                    if (tile_y >= blocked_tiles_y[k])
                                                    {
                                                        cant_see = true;
                                                        break;
                                                    }
                                                }
                                                else if (i == 0)
                                                {
                                                    if (tile_y == blocked_tiles_y[k])
                                                    {
                                                        cant_see = true;
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    if (tile_y <= blocked_tiles_y[k])
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
                                                Debug.Log("Player Seen");
                                                player_seen = true;
                                                GetComponent<PerceptionEmitter>().TriggerPlayerSeenEvent(time_to_forget, tile_x, tile_y, gameObject, GetCurrentTileX(), GetCurrentTileY());
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

                    case DIRECTION.E_DIR_WEST:
                        for (int j = 0; j < frustum_lenght; j++)
                        {
                            int tile_x = (GetCurrentTileX() - j);
                            if (tile_x >= 0 && tile_x < map_width)
                            {
                                for (int i = -j; i <= j + (frustum_size - 1); i++)
                                {
                                    int tile_y = (GetCurrentTileY() + i - ((frustum_size - 1) / 2));
                                    if (tile_y >= 0 && tile_y < map_height)
                                    {
                                        bool cant_see = false;
                                        for (int k = 0; k < blocked_tiles_x.Count; k++)
                                        {
                                            if (tile_x < blocked_tiles_x[k])
                                            {
                                                if (i > 0)
                                                {
                                                    if (tile_y >= blocked_tiles_y[k])
                                                    {
                                                        cant_see = true;
                                                        break;
                                                    }
                                                }
                                                else if (i == 0)
                                                {
                                                    if (tile_y == blocked_tiles_y[k])
                                                    {
                                                        cant_see = true;
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    if (tile_y <= blocked_tiles_y[k])
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
                                                Debug.Log("Player Seen");
                                                player_seen = true;
                                                GetComponent<PerceptionEmitter>().TriggerPlayerSeenEvent(time_to_forget, tile_x, tile_y, gameObject, GetCurrentTileX(), GetCurrentTileY());
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
                    case DIRECTION.E_DIR_MAX:
                    default:
                        Debug.Log("Direction Error: AI Can't see anything!");
                        break;
                }
            }
            else
                player_seen = false;
        }
    }

    public int GetCurrentTileX()
    {
        return (int)(GetComponent<Transform>().local_position.x / tile_size);
    }

    public int GetCurrentTileY()
    {
        return (int)(GetComponent<Transform>().local_position.z / tile_size);
    }

    public DIRECTION GetDirection()
    {
        DIRECTION ret = DIRECTION.E_DIR_NONE;

        Vector3 forward = new Vector3(GetComponent<Transform>().GetForwardVector());
        float own_angle = Mathf.Atan2(forward.x, forward.z);
        own_angle *= Mathf.RadToDeg;

        if (own_angle <= 45 && own_angle >= -45)
            ret = DIRECTION.E_DIR_SOUTH;
        else if (own_angle >= 45 && own_angle <= 135)
            ret = DIRECTION.E_DIR_WEST;
        else if (own_angle >= 135 || own_angle <= -135)
            ret = DIRECTION.E_DIR_NORTH;
        else if (own_angle <= -45 && own_angle >= -135)
            ret = DIRECTION.E_DIR_EAST;

        return ret;
    }

    public void GetPlayerTilePos(out int player_x, out int player_y)
    {
        player_obj.GetComponent<MovementController>().GetPlayerPos(out player_x, out player_y);
    }

    private List<PathNode> GetSeenTiles(DIRECTION dir)
    {
        List<PathNode> tiles = new List<PathNode>();
        int current_x = GetCurrentTileX();
        int current_y = GetCurrentTileY();

        switch (dir)
        {
            case DIRECTION.E_DIR_NORTH:
                for (int i = 0; i < frustum_lenght; i++)
                {
                    int pos_y = current_y - i;
                    for (int j = -i; j <= i; j++)
                    {
                        int pos_x = current_x + j;
                        tiles.Add(new PathNode(pos_x, pos_y));
                    }
                }
                break;
            case DIRECTION.E_DIR_SOUTH:
                for (int i = 0; i < frustum_lenght; i++)
                {
                    int pos_y = current_y + i;
                    for (int j = -i; j <= i; j++)
                    {
                        int pos_x = current_x + j;
                        tiles.Add(new PathNode(pos_x, pos_y));
                    }
                }
                break;
            case DIRECTION.E_DIR_EAST:
                for (int i = 0; i < frustum_lenght; i++)
                {
                    int pos_x = current_x + i;
                    for (int j = -i; j <= i; j++)
                    {
                        int pos_y = current_y + j;
                        tiles.Add(new PathNode(pos_x, pos_y));
                    }
                }
                break;
            case DIRECTION.E_DIR_WEST:
                for (int i = 0; i < frustum_lenght; i++)
                {
                    int pos_x = current_x - i;
                    for (int j = -i; j <= i; j++)
                    {
                        int pos_y = current_y + j;
                        tiles.Add(new PathNode(pos_x, pos_y));
                    }
                }
                break;
        }

        return tiles;
    }
}