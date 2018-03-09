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

    private Random rnd = null;
    private GameObject generated_path = null;

    // ---------------------------------------

    void Start()
    {
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
    }

    public void RemovePath()
    {
        if (generated_path != null)
        {
            generated_path.Destroy(generated_path);
            generated_path = null;
        }
    }

    public void ResetPath()
    {
        RemovePath();
        GeneratePath();
    }
}