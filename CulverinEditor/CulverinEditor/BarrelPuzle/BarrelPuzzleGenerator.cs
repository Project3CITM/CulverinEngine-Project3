using System;
using System.Linq;
using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;
using CulverinEditor.Map;

public class BarrelPuzzleGenerator : CulverinBehaviour
{
    public string path_name = "PuzzlePath"; // "PuzzlePath#.prefab.json";
    public int possible_paths = 6;
    // Its a really bad way to do this but scritps system can handel arrays so....
    
    // The world position of the map tile 0,0 to properly calc the world to tile pos.
    public float map_first_tile_pos_x = 0.0f;
    public float map_first_tile_pos_z = 0.0f;

    public GameObject movement_controller_go = null;
    private MovementController movement_controller = null;

    private PuzzleLogicMap used_logic_map = null;

    private Random rnd = null;
    private GameObject generated_path = null;
    private int generated_path_index = 0;

    // List to handle all fallen barrels and clear them when puzzle needs to be restart. TODO: Might be a better place to handle this...
    private List<GameObject> fallen_barrels;

    // Tile system puzzle map start
    private int puzzle_start_tile_x = 0;
    private int puzzle_start_tile_z = 0;
    private float puzzle_start_pos_x = 0;
    private float puzzle_start_pos_z = 0;

    //--Map data to calc some stuff.
    private int map_width = 0;
    private int map_height = 0;

    const float tile_size = 25.4f;

    // ---------------------------------------

    void Start()
    {
        movement_controller_go = GetLinkedObject("movement_controller_go");
        
        movement_controller = movement_controller_go.GetComponent<MovementController>();

        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();
        GetPuzzleStartingPos();
        
        fallen_barrels = new List<GameObject>();
        rnd = new Random();
        GeneratePath();
    }

    void Update()
    {
        if(used_logic_map == null) UpdateLogicMap();


        //Just testing purposes.
        if (Input.GetKeyDown(KeyCode.M))
        {
            //ResetPath();
            DebugNewLogicMap();
        }
        if (Input.GetKeyDown(KeyCode.M))
        {
            //Debug.Log("x: " + map_first_tile_pos_x + " y: " + map_first_tile_pos_z);
            //Vector3 tile_pos = new Vector3(((120.0f - map_first_tile_pos_x) / tile_size),
            //    ((300.0f - map_first_tile_pos_z) / tile_size), 0.0f);
            //Debug.Log("GO tile pos: " + tile_pos.x + ", " + tile_pos.y);
            //CheckLogicMap((int)tile_pos.x, (int)tile_pos.y);
            //
            //ResetPath();
        }
    }

    // ---------------------------------------

    void GeneratePath()
    {
        generated_path_index = rnd.Next(1, possible_paths + 1);
        string selected_path_name = path_name + generated_path_index.ToString();
        Debug.Log(path_name);
        generated_path = Instantiate(selected_path_name);
        if (generated_path == null)
        {
            Debug.Log("Generated path is null, cant get the reference of the go.");
        }

        Debug.Log("New path generated at x: " + puzzle_start_pos_x + " y: " + puzzle_start_pos_z);

        generated_path.transform.SetPosition(new Vector3(puzzle_start_pos_x, 0.0f, puzzle_start_pos_z));

        UpdateLogicMap();
    }

    public void RemovePath()
    {
        if (generated_path != null)
        {
            generated_path.Destroy(generated_path);
            generated_path = null;
        }
        used_logic_map = null;
        RemoveBarrels();
    }

    public void ResetPath()
    {
        RemovePath();
        GeneratePath();
    }

    // Call this whenever a barrel is hit to fall.
    public void OnBarrelFall(GameObject barrel)
    {
       
        Vector3 tile_pos = GetTilePosFromBarrel(barrel);

       
        // Add the barrel to fallen tracked list
        fallen_barrels.Add(barrel);
        // Calc the tile pos
        Debug.Log(((int)Mathf.Round(tile_pos.x)).ToString());
        Debug.Log(((int)Mathf.Round(tile_pos.y)).ToString());
        // Update logic map if needed
        //movement_controller.SetTileWalkability(5, 14, 0);
       // movement_controller.SetTileWalkability((int)Mathf.Round(tile_pos.x), (int)Mathf.Round(tile_pos.y), 0);
        CheckLogicMap((int)Mathf.Round(tile_pos.x), (int)Mathf.Round(tile_pos.y));
      
    }

    void RemoveBarrels()
    {
        // TODO: Add some kind of animation that moves all barrels to the edge or sink them.
        // TODO: Updte again the logic map to no walkble
        // TODO: Probably can directly destroy the barrels, reset them instead
        // For now just destroy them.
        foreach (GameObject b in fallen_barrels)
        {
            b.Destroy(b);
        }
    }

    Vector3 GetTilePosFromBarrel(GameObject barrel)
    {
        Vector3 ret = new Vector3(0.0f, 0.0f, 0.0f);

        // Calc tile pos: Each tile 25.4 * 25.4  must check barrel position and parent position
        // Tile size hardcoded for now: 25.4 x 25.4

        // Maybe need to add some kind of offset or take into account the half of the tile size
        Vector3 temp = (barrel.transform.local_position);

        ret.x = Mathf.Round((temp.x*13.0f + 10.0f) / 25.4f);
        ret.y = Mathf.Round((temp.z*13.0f + 227.0f) / 25.4f);
        
       
        return ret;
    }

    void CheckLogicMap(int x_pos, int y_pos)
    {
        if (used_logic_map == null) UpdateLogicMap();

        if (used_logic_map != null)
        {
            Debug.Log("Checking tile walkability. Map tile: " + x_pos + ", " + y_pos);
            // Given tile pos is in map system, must convert to local puzzle coords.
            int x_local_pos =  x_pos - 1;
            int y_local_pos = y_pos - 9;

            Debug.Log("Tile local: " + x_local_pos + ", " + y_local_pos);
            // Just make sure local coords are mapped properly
            if ((x_local_pos >= 0 && x_local_pos < used_logic_map.width)
               && (y_local_pos >= 0 && y_local_pos < used_logic_map.height))
            {
                //Debug.Log("Tile is inside path boundaries. Local walkability is: " + used_logic_map.walkability[x_pos, y_pos]);
                if (used_logic_map.walkability[x_local_pos, y_local_pos] == 0)
                {
                    // Acces to walkability map on movement controller and modify the value with the world map tile coord.
                    if (movement_controller == null)
                    {
                        movement_controller = movement_controller_go.GetComponent<MovementController>();
                    }

                    movement_controller.SetTileWalkability(x_pos, y_pos, 0);
                    Debug.Log("TEST");
                }
            }
            Debug.Log("TEST2");
        }
    }

    void GetPuzzleStartingPos()
    {
        for (int x = 0; x < map_width; ++x)
        {
            for (int y = 0; y < map_height; ++y)
            {
                // If tile is puzzle start just get the coord and exit
                if (movement_controller.GetTileWalkability(x, y) == 3)
                {
                    Debug.Log("Puzzle start at tile: " + x + ", " + y);
                    puzzle_start_tile_x = x;
                    puzzle_start_tile_z = y;

                    //Convert tile pos to world pos
                    puzzle_start_pos_x = x * tile_size; // Must apply an offset for sure
                    puzzle_start_pos_z = y * tile_size; // Must apply an offset for sure
                    Debug.Log("Puzzle start at pos: " + puzzle_start_pos_x + ", " + puzzle_start_pos_z);


                    return;
                }
            }
        }
        Debug.Log("Could not found puzzle start point");
    }

    void UpdateLogicMap()
    {
        switch (generated_path_index)
        {
            case 1:
                used_logic_map = generated_path.GetComponent<PuzzlePath1>().logic_map;
                break;
            case 2:
                used_logic_map = generated_path.GetComponent<PuzzlePath2>().logic_map;
                break;
            case 3:
                used_logic_map = generated_path.GetComponent<PuzzlePath3>().logic_map;
                break;
            case 4:
                used_logic_map = generated_path.GetComponent<PuzzlePath4>().logic_map;
                break;
            case 5:
                used_logic_map = generated_path.GetComponent<PuzzlePath5>().logic_map;
                break;
            case 6:
                used_logic_map = generated_path.GetComponent<PuzzlePath6>().logic_map;
                break;
        }

        //DebugNewLogicMap();
    }

    void DebugNewLogicMap()
    {
        if (used_logic_map != null)
        {
            for (int y = 0; y < used_logic_map.height; ++y)
            {
                string t = "";
                for (int x = 0; x < used_logic_map.width; ++x)
                {
                    t += (used_logic_map.walkability[x, y] + " - ");
                }
                Debug.Log(t);
            }
        }
        else
        {
            Debug.Log("Used logic map not assigned.");
        }
    }
}