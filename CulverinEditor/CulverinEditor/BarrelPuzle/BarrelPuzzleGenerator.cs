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

    public float spawn_pos_x = 0.0f;
    public float spawn_pos_y = 0.0f;
    public float spawn_pos_z = 0.0f;

    //public GameObject spawn_pos = null;
    //
    //public bool use_go_as_spawn_pos = false;

    private PuzzleLogicMap used_logic_map = null;

    private Random rnd = null;
    private GameObject generated_path = null;

    // List to handle all fallen barrels and clear them when puzzle needs to be restart. TODO: Might be a better place to handle this...
    private List<GameObject> fallen_barrels;

    private int puzzle_start_pos_x = 0;
    private int puzzle_start_pos_y = 0;

    //--Map data to calc some stuff.
    private int map_width = 0;
    private int map_height = 0;

    // ---------------------------------------

    void Start()
    {
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();
        GetPuzzleStartingPos();

        fallen_barrels = new List<GameObject>();
        rnd = new Random();
        GeneratePath();
    }

    void Update()
    {
        //Just testing purposes.
        if (Input.GetKeyDown(KeyCode.A))
        {
            ResetPath();
        }
    }

    // ---------------------------------------

    void GeneratePath()
    {
        int i = rnd.Next(1, possible_paths + 1);
        string selected_path_name = path_name + i.ToString();
        generated_path = Instantiate(selected_path_name);
        //Vector3 spawn = use_go_as_spawn_pos
        //    ? spawn_pos.transform.position
        //    : new Vector3(spawn_pos_x, spawn_pos_y, spawn_pos_z);
        //generated_path.transform.SetPosition(spawn);
        generated_path.transform.SetPosition(new Vector3(spawn_pos_x, spawn_pos_y, spawn_pos_z));
        switch (i)
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

    //TODO: Call this whenever a barrel is hit to fall.
    public void OnBarrelFall(GameObject barrel)
    {
        // Add the barrel to fallen tracked list
        fallen_barrels.Add(barrel);
        // Calc the tile pos
        Vector3 tile_pos = GetTilePosFromBarrel(barrel);
        // Update logic map if needed
        CheckLogicMap((int)tile_pos.x, (int)tile_pos.y);
    }

    void RemoveBarrels()
    {
        // TODO: Add some kind of animation that moves all barrels to the edge or sink them.
        // For now just destroy them.
        foreach (GameObject b in fallen_barrels)
        {
            b.Destroy(b);
        }
    }

    static Vector3 GetTilePosFromBarrel(GameObject barrel)
    {
        Vector3 ret = new Vector3(0.0f, 0.0f, 0.0f);

        // TODO: Calc tile pos: Each tile 25.4 * 25.4  must check barrel position and parent position



        return ret;
    }

    void CheckLogicMap(int x_pos, int y_pos)
    {
        if (used_logic_map != null)
        {
            // Given tile pos is in map system, must convert to local puzzle coords.
            int x_local_pos = puzzle_start_pos_x - x_pos;
            int y_local_pos = puzzle_start_pos_y - y_pos;

            // Just make sure local coords are mapped properly
            if ((x_local_pos >= 0 && x_local_pos < used_logic_map.width)
                && (y_local_pos >= 0 && y_local_pos < used_logic_map.height))
            {
                if (used_logic_map.walkability[x_local_pos, y_local_pos] == 1)
                {
                    // TODO: Acces to walkability map on movement controller and modify the value with the world map tile coord.

                }
            }
        }
    }

    void GetPuzzleStartingPos()
    {
        // TODO: Get the walkable map from movement

        // TODO: Should move this to movement script
        string tmp_map = Map.GetMapString();
        int t = 0;
        for (int x = 0; x < map_width; ++x)
        {
            for (int y = 0; y < map_height; ++y)
            {
                // If tile is puzzle start just get the coord and exit
                if (int.Parse(tmp_map[t].ToString()) == 3)
                {
                    puzzle_start_pos_x = x;
                    puzzle_start_pos_y = y;
                    return;
                }
                ++t;
            }
        }
    }
}