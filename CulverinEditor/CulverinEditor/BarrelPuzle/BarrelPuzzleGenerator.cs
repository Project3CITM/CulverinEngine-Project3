using System;
using System.Linq;
using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;
using CulverinEditor.Map;

public class BarrelPuzzleGenerator : CulverinBehaviour
{
    public int puzzle_width = 6;
    public int puzzle_height = 6;

    public string path_name = "PuzzlePath"; // "PuzzlePath#.prefab.json";
    public int possible_paths = 4;
    // Its a really bad way to do this but scritps system can handel arrays so....
    
    // The world position of the map tile 0,0 to properly calc the world to tile pos.
    public float map_first_tile_pos_x = 0.0f;
    public float map_first_tile_pos_z = 0.0f;

    // Reference to movement controller to acces walkability map.
    //public GameObject movement_controller_go = null;
    //private MovementController movement_controller = null;
    
    // A random generator object
    private Random rnd = null;
    // Reference to the path generated.
    private Path current_path = null;
    
    // Tile system puzzle map start
    //private int puzzle_start_tile_x = 0;
    //private int puzzle_start_tile_z = 0;
    //private float puzzle_start_pos_x = 0.0f;
    //private float puzzle_start_pos_z = 0.0f;

    //--Map data to calc some stuff.
    //private int map_width = 0;
    //private int map_height = 0;

    float tile_size = 25.4f;

    // TMP: Testing purposes
    private List<GameObject> tmp_test_path;
    public string path_tile_prefab_name = "tmp_stone";

    // ---------------------------------------

    void Start()
    {
        //movement_controller_go = GetLinkedObject("movement_controller_go");   
        //movement_controller = movement_controller_go.GetComponent<MovementController>();
        //
        //map_width = Map.GetWidthMap();
        //map_height = Map.GetHeightMap();
        //GetPuzzleStartingPos();
        tile_size = 25.4f;
        // TMP:
        tmp_test_path = new List<GameObject>();

        rnd = new Random();
        GeneratePath();
    }

    void Update()
    {
        //Just testing purposes.
        if (Input.GetKeyDown(KeyCode.M))
        {
            ResetPath();
            //DebugNewLogicMap();
        }
    }

    // ---------------------------------------

    void GeneratePath()
    {
        int index = rnd.Next(0, possible_paths);
        Debug.Log("Path index: " + index);
        current_path = new Path(puzzle_width, puzzle_height,
                                index);
        DebugNewLogicMap();
    }

    public void RemovePath()
    {
        // TMP
        foreach (GameObject it in tmp_test_path)
        {
            it.transform.SetPosition(new Vector3(-1000.0f, -1000.0f, -1000.0f));
        }
    }

    public void ResetPath()
    {
        RemovePath();
        GeneratePath();
    }

    // Call this whenever a barrel is hit to fall.
    public void OnBarrelFall(GameObject barrel)
    {
        //Debug.Log("On Barrel Fall");
        //// Calc the tile pos
        //Vector3 tile_pos = GetTilePosFromBarrel(barrel);
        //// Update logic map if needed
        //CheckLogicMap((int)Mathf.Round(tile_pos.x), (int)Mathf.Round(tile_pos.y));
    }
    //
    //void RemoveBarrels()
    //{
    //
    //}
    //
    //Vector3 GetTilePosFromBarrel(GameObject barrel)
    //{
    //    Vector3 ret = new Vector3(0.0f, 0.0f, 0.0f);
    //
    //    // Calc tile pos: Each tile 25.4 * 25.4  must check barrel position and parent position
    //    // Tile size hardcoded for now: 25.4 x 25.4
    //
    //    // Maybe need to add some kind of offset or take into account the half of the tile size
    //    Vector3 temp = (barrel.transform.local_position);
    //
    //    // TODO: Change the conversion formula
    //    ret.x = Mathf.Round((temp.x * 13.0f - 270.0f) / tile_size);
    //    ret.y = Mathf.Round((temp.z * 13.0f + 227.0f) / tile_size);
    //
    //    return ret;
    //}
    //
    //void CheckLogicMap(int x_pos, int y_pos)
    //{
    //    
    //}
    //
    //void GetPuzzleStartingPos()
    //{
    //    if(movement_controller != null)
    //    {
    //        for (int x = 0; x < map_width; ++x)
    //        {
    //            for (int y = 0; y < map_height; ++y)
    //            {
    //                // If tile is puzzle start just get the coord and exit
    //                if (movement_controller.GetTileWalkability(x, y) == 3)
    //                {
    //                    Debug.Log("Puzzle start at tile: " + x + ", " + y);
    //                    puzzle_start_tile_x = x;
    //                    puzzle_start_tile_z = y;
    //
    //                    //Convert tile pos to world pos
    //                    puzzle_start_pos_x = x * tile_size; // Must apply an offset for sure
    //                    puzzle_start_pos_z = y * tile_size; // Must apply an offset for sure
    //                    Debug.Log("Puzzle start at pos: " + puzzle_start_pos_x + ", " + puzzle_start_pos_z);
    //
    //
    //                    return;
    //                }
    //            }
    //        }
    //        Debug.Log("Could not found puzzle start point");
    //    }
    //    else
    //    {
    //        Debug.Log("Movement controller reference not set.");
    //    }
    //}

    void DebugNewLogicMap()
    {
        Debug.Log("Logging logic map.");
        
        for (int y = 0; y < current_path.height; ++y)
        {
            string t = "";
            for (int x = 0; x < current_path.width; ++x)
            {
                t += (current_path.walkability[x, y] + " - ");
            }
            Debug.Log(t);
        }

        // TMP: Debugging purposes.
        for (int y = 0; y < current_path.height; ++y)
        {
            for (int x = 0; x < current_path.width; ++x)
            {
                if (current_path.walkability[x, y] == 0)
                {
                    GameObject tmp = Instantiate(path_tile_prefab_name);
                    tmp.transform.SetPosition(new Vector3(x * 25.4f, 0.0f, y * 25.4f));
                    tmp_test_path.Add(tmp);
                }
            }
        }
    }
}