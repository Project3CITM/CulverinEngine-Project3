using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;
using CulverinEditor.Map;
using System.Collections.Generic;
using System.Collections;

public class AIManager : CulverinBehaviour
{
    //bool path_reached = false;
    Pathfinder path_creator;
    List<PathNode> my_path;

    float timer = 0.0f;
    bool path_reached;

    
    public int x, y;
    

    

    // Use this for initialization
    void Start()
    {
        path_creator = new Pathfinder();
        path_creator.Init();
	x = 0;
        y = 0;
        my_path = path_creator.CalculatePath(new PathNode(x, y), new PathNode(0, 5));

        path_reached = false;

    }

    void Update()
    {
        if (!path_reached)
        {
            if (timer >= 0.50f)
            {
                MoveNextTile();
                timer = 0.0f;
            }
            else
            {
                timer += Time.DeltaTime();
            }


        }

      if(Input.GetKeyDown(KeyCode.W))
        ChangePath();

    }

    void MoveNextTile()
    {

        if (my_path.Count == 0)
        {
            path_reached = true;
            return;
        }

        GetComponent<Transform>().position = new Vector3(my_path[0].GetTileX() * 2, 2, my_path[0].GetTileY() * 2);
        x = my_path[0].GetTileX();
        y = my_path[0].GetTileY();
        my_path.Remove(my_path[0]);
    }

    void ChangePath()
    {
        my_path.Clear();
        my_path = path_creator.CalculatePath(new PathNode(x, y), new PathNode(0, 0));

    }

}