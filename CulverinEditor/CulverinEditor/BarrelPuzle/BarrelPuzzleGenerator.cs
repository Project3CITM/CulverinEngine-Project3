using System;
using System.Linq;
using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

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

    private Random rnd = null;
    private GameObject generated_path = null;

    // List to handle all fallen barrels and clear them when puzzle needs to be restart. TODO: Might be a better place to handle this...
    private List<GameObject> fallen_barrels;

    // ---------------------------------------

    void Start()
    {
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
        string selected_path_name = path_name + rnd.Next(1, possible_paths + 1).ToString();
        generated_path = Instantiate(selected_path_name);
        //Vector3 spawn = use_go_as_spawn_pos
        //    ? spawn_pos.transform.position
        //    : new Vector3(spawn_pos_x, spawn_pos_y, spawn_pos_z);
        //generated_path.transform.SetPosition(spawn);
        generated_path.transform.SetPosition(new Vector3(spawn_pos_x, spawn_pos_y, spawn_pos_z));
    }

    public void RemovePath()
    {
        if (generated_path != null)
        {
            generated_path.Destroy(generated_path);
            generated_path = null;
        }

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
        fallen_barrels.Add(barrel);
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
}