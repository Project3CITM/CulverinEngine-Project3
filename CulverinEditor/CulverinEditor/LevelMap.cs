using System;
using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class LevelMap : CulverinBehaviour
{
    public int[,] map;
    public int map_width = 0;
    public int map_height = 0;

    void Start()
    {
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();

        map = new int[map_width, map_height];
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                map[x, y] = 0;
            }
        }

        string map_string = Map.GetMapString();

        int t = 0;
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                map[x, y] = int.Parse(map_string[t].ToString());
                t += 1;
            }
        }
    }


    //void Update()
    //{

    //}

    void UpdateMap()
    {

    }

}