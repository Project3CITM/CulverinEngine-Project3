using System;
using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class Lever : CulverinBehaviour
{
    // Lines-----------------------------
    public GameObject Puzzle_line_1;
    public GameObject Puzzle_line_2;
    public GameObject Puzzle_line_3;
    public GameObject Puzzle_line_4;
    public GameObject Puzzle_line_5;
    public GameObject Puzzle_line_6;

    //Lists ---------------------------------------
    private List<GameObject> line1;
    private List<GameObject> line2;
    private List<GameObject> line3;
    private List<GameObject> line4;
    private List<GameObject> line5;
    private List<GameObject> line6;

    // 
    public float speed_barrel = 5.0f;
    public float wheight_barrel = 20.0f;
    public float barrel_fall_speed = 0.0f;
    public float floor_height = 0.0f;

    //time to wait, move second mode
    public float delay_second_mode = 6.0f;
    private float time = 0.0f;

    public int barrel_per_line = 6; // Defines the puzzle width
    public int number_lines = 6;    // Defines the puzzle height
    public bool active_lever = false;

    private bool phase1 = false; // Set info all barrels
    private bool phase2 = false; // Move barrels mode.PUZZLE
    private bool phase_wait = false; // wait to move the other mode
    private bool phase3 = false; // Move barrels mode.FILLING
    private bool editmap = false; // editmap

    public int possible_paths = 4;
    // Puzzle orientation: 0-North. 1-East. 2-South. 3-West.
    public int puzzle_orientation = 0;

    private Vector3 orientation_x;
    private Vector3 orientation_z;

    // Tile system puzzle map start
    //      Tile coords
    public int puzzle_start_tile_x = 0;
    public int puzzle_start_tile_z = 0;
    //      World coords
    private float puzzle_start_pos_x = 0.0f;
    private float puzzle_start_pos_z = 0.0f;

    // A random generator object
    private Random rnd = null;

    private float tile_size;

    // Path support class to handle the path walkability.
    private Path current_path = null;

    //--------------

    public GameObject lever_go = null;
    private CompAnimation anim_controller = null;
    private bool on_lever_animation = false;
    private string lever_animation_name = "Lever_Down"; // TODO: Set the animation name
    private BarrelPuzzleManager barrel_puzzel_manager = null; // Put both scripts in same GO.



    public GameObject level_map;
    public GameObject player;


    PuzzleCountdown countdown;

    //Needed to know when the fill barrels are placed in puzzle
    BarrelFall fill_barrel;
   
    
    //--------------

    void Start()
    {
        tile_size = 25.4f;

        rnd = new Random();

        lever_go = GetLinkedObject("lever_go");


        countdown = GetComponent<PuzzleCountdown>();
        if(countdown == null)
        {
            Debug.Log("There is no countdown in puzzle!");
        }

        anim_controller = lever_go.GetComponent<CompAnimation>();
        if (anim_controller == null)
        {
            Debug.Log("Animation is null!");
        }
        barrel_puzzel_manager = GetComponent<BarrelPuzzleManager>();
        if (barrel_puzzel_manager == null)
        {
            Debug.Log("Animation is null!");
        }

        Puzzle_line_1 = GetLinkedObject("Puzzle_line_1");
        Puzzle_line_2 = GetLinkedObject("Puzzle_line_2");
        Puzzle_line_3 = GetLinkedObject("Puzzle_line_3");
        Puzzle_line_4 = GetLinkedObject("Puzzle_line_4");
        Puzzle_line_5 = GetLinkedObject("Puzzle_line_5");
        Puzzle_line_6 = GetLinkedObject("Puzzle_line_6");

        line1 = new List<GameObject>();
        line2 = new List<GameObject>();
        line3 = new List<GameObject>();
        line4 = new List<GameObject>();
        line5 = new List<GameObject>();
        line6 = new List<GameObject>();

        // Get All barrels from Puzzle.
        SetBarrels();

        // Desactivate all barrels, while the player dont stay in puzzle. 
        DesactivateBarrels(line1);
        DesactivateBarrels(line2);
        DesactivateBarrels(line3);
        DesactivateBarrels(line4);
        DesactivateBarrels(line5);
        DesactivateBarrels(line6);

        // Map
        current_path = new Path(barrel_per_line, number_lines, puzzle_orientation);
        GeneratePath();

        
        //// Testing --------------------------------------------
        //for (int y = 0; y < number_lines; y++)
        //{
        //    for (int x = 0; x < barrel_per_line; x++)
        //    {
        //        current_path.walkability[x, y] = 0;
        //    }
        //}

        //current_path.walkability[4, 0] = 1; 
        //current_path.walkability[4, 1] = 1; 
        //current_path.walkability[4, 2] = 1;
        //current_path.walkability[4, 3] = 1;
        //current_path.walkability[5, 3] = 1;
        //current_path.walkability[5, 4] = 1;
        //current_path.walkability[5, 5] = 1;



        // -------------------------------------------------------
    }
    void Update()
    {
        //-- TMP: Debug -----
        if (Input.GetKeyDown(KeyCode.B))
        {
         
            if (on_lever_animation == true)
            {
                Debug.Log("Hardcoded lever anim finish.");
                OnLeverAnimFinish();
            }
            else
            {
                Debug.Log("Puzzle activated");
                OnLeverActivated();
            }
        }


        if (Input.GetKeyDown(KeyCode.N))
        {
            ResetPuzzle();
        }


        //---------------------

        if (on_lever_animation)
         {
             anim_controller = lever_go.GetComponent<CompAnimation>();
            if (anim_controller != null)
             {
                 if (anim_controller.IsAnimationStopped(lever_animation_name))
                 {
                     Debug.Log("Lever animation ended.");
                     // The lever animation has stopped so puzzle must start.
                     OnLeverAnimFinish();
                 }
             }
         }

      ;
        if (active_lever)
        {
            if (!phase1) // Set info all barrels
            {
                SetInfo(line1, 0);
                SetInfo(line2, 1);
                SetInfo(line3, 2);
                SetInfo(line4, 3);
                SetInfo(line5, 4);
                SetInfo(line6, 5);
                phase1 = true;
                SetPuzzleWalkability(1);
            }
            if (!phase2) // Move barrels mode.PUZZLE
            {

                MoveBarrels(line1);
                MoveBarrels(line2);
                MoveBarrels(line3);
                MoveBarrels(line4);
                MoveBarrels(line5);
                MoveBarrels(line6);
                phase3 = true;
                phase2 = true;
                phase_wait = true;
                time = Time.realtimeSinceStartup + delay_second_mode;

            }
            if (phase_wait) // wait to move the other mode
            {
               // Debug.Log("Waiting");
                // Wait delay to move other barrels
                float time_transcured = Time.realtimeSinceStartup;
                if (time_transcured >= time)
                {
                    phase3 = false;
                    phase_wait = false;
                }
            }
            if (!phase3) // Move barrels mode.FILLING
            {
                MoveBarrels(line1, true);
                MoveBarrels(line2, true);
                MoveBarrels(line3, true);
                MoveBarrels(line4, true);
                MoveBarrels(line5, true);
                MoveBarrels(line6, true);
                phase3 = true;
                editmap = true;
            }

            if (editmap && fill_barrel.IsPlaced())
            {
                SetPathWalkable();
                editmap = false;
            }


            if(countdown.IsCountdownOver())
            {
                ResetPuzzle();
            }
        }
    }

    // -------------------------------------------------------------------------------------

    void MoveBarrels(List<GameObject> list, bool isfilling = false)
    {
        for (int i = 0; i < list.Count; i++)
        {
            BarrelFall b_fall = list[i].GetComponent<BarrelFall>();

            if (b_fall.IsPuzzleMode())
            {
                Debug.Log("Activating puzzle barrels");          
                list[i].SetActive(true);

            }
            else if (isfilling)
            {
                Debug.Log("Activating filling barrels");                
                list[i].SetActive(true);
                fill_barrel = b_fall;
            }
        }
    }

    void DesactivateBarrels(List<GameObject> list)
    {
        for (int i = 0; i < list.Count; i++)
        {
            list[i].SetActive(false);
        }
    }

    void SetInfo(List<GameObject> list, int number_of_lines)
    {
        int count_barrel = barrel_per_line - 1;
        int y = number_of_lines;

        int curr_x = 0;
        int curr_y = 0;
        for (int x = barrel_per_line - 1; x >= 0; x--)
        {
            curr_x = puzzle_start_tile_x + x * (int)(orientation_x.x + orientation_z.x);
            curr_y = puzzle_start_tile_z + y * (int)(orientation_x.z + orientation_z.z);	

            if (current_path.walkability[x, y] == 0)
            {
               
                list[count_barrel--].GetComponent<BarrelFall>().SetData(speed_barrel, wheight_barrel, curr_x, curr_y, barrel_fall_speed, BarrelFall.ModeBarrel.PUZZLE, floor_height);
            }
            else if (current_path.walkability[x, y] == 1)
            {
                list[count_barrel--].GetComponent<BarrelFall>().SetData(speed_barrel, wheight_barrel, curr_x, curr_y, barrel_fall_speed, BarrelFall.ModeBarrel.FILLING, floor_height);
            }
        }
    }

    void SetBarrels()
    {
        bool stop = false;
        int count = 0;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_1.GetChildByTagIndex("barrel", count++);
            if (temp == null)
                stop = true;
            else
                line1.Add(temp);
        }
        stop = false;
        count = 0;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_2.GetChildByTagIndex("barrel", count++);
            if (temp == null)
                stop = true;
            else
                line2.Add(temp);
        }
        stop = false;
        count = 0;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_3.GetChildByTagIndex("barrel", count++);
            if (temp == null)
                stop = true;
            else
                line3.Add(temp);
        }
        stop = false;
        count = 0;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_4.GetChildByTagIndex("barrel", count++);
            if (temp == null)
                stop = true;
            else
                line4.Add(temp);
        }
        stop = false;
        count = 0;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_5.GetChildByTagIndex("barrel", count++);
            if (temp == null)
                stop = true;
            else
                line5.Add(temp);
        }
        stop = false;
        count = 0;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_6.GetChildByTagIndex("barrel", count++);
            if (temp == null)
                stop = true;
            else
                line6.Add(temp);
        }
    }

    void OnLeverActivated()
    {
        // Called when lever is activated. Set flag to true and play the animation.
        on_lever_animation = true;
        Debug.Log("Animation called");
        anim_controller = lever_go.GetComponent<CompAnimation>();
        if (anim_controller != null)
        {
            anim_controller.PlayAnimation(lever_animation_name);
            Debug.Log("Animation activated");
        }
    }

    void OnLeverAnimFinish()
    {
        on_lever_animation = false;
        // Activate the water
        barrel_puzzel_manager.OnPuzzleActivated();
        // Activate the puzzle
        active_lever = true; // TODO: Verify this is correct and uncomment this line

        countdown.StartCountdown();
    }

    void OnTriggerEnter()
    {
        // TODO: Activate UI button "Interact"
        Debug.Log("OnTriggerEnter Lever");
        if (!active_lever)
        {
            Debug.Log("Lever Ready");
            OnLeverActivated();
            Debug.Log("Lever Activated");
        }
    }

    void OnTriggerLost()
    {
        // TODO: Deactivate UI button "Interact"
    }

    // Convert from tile coords to world coords
    void TileToWorld(int tile_x, int tile_z, out float world_x, out float world_z)
    {
        world_x = tile_x * tile_size - (tile_size / 2.0f);
        world_z = tile_z * tile_size - (tile_size / 2.0f);
    }

    // Convert from world coords to tile coords
    void WorldToTile(float world_x, float world_z, out int tile_x, out int tile_z)
    {
        tile_x = (int)((world_x + (tile_size / 2.0f)) / tile_size);
        tile_z = (int)((world_z + (tile_size / 2.0f)) / tile_size);
    }

    // Actually generate the path walkability
    void GeneratePath()
    {
        int index = rnd.Next(0, possible_paths);
        current_path.CreateWalkability(index);

        TileToWorld(puzzle_start_tile_x, puzzle_start_tile_z, out puzzle_start_pos_x, out puzzle_start_pos_z);
        SetOrientationVectors();
        Debug.Log("Path index: " + index);
        Debug.Log("Puzzle start at tile: " + puzzle_start_tile_x + ", " + puzzle_start_tile_z);
        Debug.Log("Puzzle start at pos: " + puzzle_start_pos_x + ", " + puzzle_start_pos_z);

        LogLogicMap();
    }

    void LogLogicMap()
    {
        Debug.Log("Logging logic map ------------------");

        Debug.Log("Puzzle start at tile: " + puzzle_start_tile_x + ", " + puzzle_start_tile_z);
        Debug.Log("Puzzle start at pos: " + puzzle_start_pos_x + ", " + puzzle_start_pos_z);
        Debug.Log("Puzzle orienttion: " + current_path.puzzle_direction);

        for (int y = 0; y < current_path.height; ++y)
        {
            string t = "";
            for (int x = 0; x < current_path.width; ++x)
            {
                t += (current_path.walkability[x, y] + " - ");
            }
            Debug.Log(t);
        }

        //TMP: Debugging purposes.
      /*  for (int y = 0; y < current_path.height; ++y)
        {
            for (int x = 0; x < current_path.width; ++x)
            {
                if (current_path.walkability[x, y] == 0)
                {
                    GameObject tmp = Instantiate("tmp_stone");
                    tmp.transform.SetPosition(new Vector3(x * 25.4f + puzzle_start_pos_x, 0.0f, y * 25.4f + puzzle_start_pos_z));
                    //tmp_test_path.Add(tmp);
                }
            }
        }*/
    }

    void SetOrientationVectors()
    {
        // Puzzle orientation: 0-North. 1-East. 2-South. 3-West.
        switch (puzzle_orientation)
        {
            case 0:
                orientation_x = new Vector3(1, 0, 0);
                orientation_z = new Vector3(0, 0, 1);
                break;
            case 1:
                orientation_x = new Vector3(0, 0, -1);
                orientation_z = new Vector3(1, 0, 0);
                break;
            case 2:
                orientation_x = new Vector3(-1, 0, 0);
                orientation_z = new Vector3(0, 0, -1);
                break;
            case 3:
                orientation_x = new Vector3(0, 0, 1);
                orientation_z = new Vector3(1, 0, 0);
                break;
        }
    }


    void SetPathWalkable()
    {
        level_map = GetLinkedObject("level_map");
        LevelMap level_map_script;
        if (level_map == null)
            Debug.Log("MAP IS NULL");
        level_map_script = level_map.GetComponent<LevelMap>();

        player = GetLinkedObject("player");
        MovementController player_mov = player.GetComponent<MovementController>();


        for (int y = 0; y < current_path.height; ++y)
        {
            for (int x = 0; x < current_path.width; ++x)
            {   
              
                if (current_path.walkability[x, y] == 0)
                {
                    level_map_script.UpdateMap(x + puzzle_start_tile_x, y + puzzle_start_tile_z, 0);
                    player_mov.SetTileWalkability(x + puzzle_start_tile_x, y + puzzle_start_tile_z, 0);
                }
                else
                {
                    level_map_script.UpdateMap(x + puzzle_start_tile_x, y + puzzle_start_tile_z, 3);
                    player_mov.SetTileWalkability(x + puzzle_start_tile_x, y + puzzle_start_tile_z, 3);
                }
            }
            
        }
    }

    public void ResetPuzzle()
    {
        ResetLine(line1);
        ResetLine(line2);
        ResetLine(line3);
        ResetLine(line4);
        ResetLine(line5);
        ResetLine(line6);

        phase1 = false;
        phase2 = false;
        phase_wait = false;
        phase3 = false;
        editmap = false;
        active_lever = false;
        time = 0.0f;

        SetPuzzleWalkability(3);

    }

    void ResetLine(List<GameObject> list)
    {
        int i = barrel_per_line - 1;     

        for (; i >= 0; i--)
        {
            BarrelFall b_fall = list[i].GetComponent<BarrelFall>();            
            b_fall.ResetBarrel();
        }
    }


    void SetPuzzleWalkability(int value)
    {
        level_map = GetLinkedObject("level_map");
        LevelMap level_map_script;
        if (level_map == null)
            Debug.Log("MAP IS NULL");
        level_map_script = level_map.GetComponent<LevelMap>();

        player = GetLinkedObject("player");
        MovementController player_mov = player.GetComponent<MovementController>();

        for (int y = 0; y < current_path.height; ++y)
        {
            for (int x = 0; x < current_path.width; ++x)
            {              
                    level_map_script.UpdateMap(x + puzzle_start_tile_x, y + puzzle_start_tile_z, value);
                    player_mov.SetTileWalkability(x + puzzle_start_tile_x, y + puzzle_start_tile_z, value);
            }

        }
    }
}