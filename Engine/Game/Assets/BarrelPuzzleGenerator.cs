using System;
using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelPuzzleGenerator : CulverinBehaviour
{
    public string[] paths_names;

    private Random rnd;

    // ---------------------------------------

    void Start()
    {
        rnd = new Random();
        GeneratePath();
    }

    void Update()
    {
        
    }

    // ---------------------------------------

    void GeneratePath()
    {
        string selected_path_name = paths_names[rnd.Next(0, paths_names.Length)];
        GameObject path_prefab = Instantiate(selected_path_name);
    }
}