using System;
using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class LevelMap : CulverinBehaviour
{
    public GameObject map_obj;
    private int[,] level_map;
    public int map_width = 0;
    public int map_height = 0;

    // Elliot ----
    public System.Random randomspeed;

    public int[,] map
    {
        get
        {
            return level_map;
        }
    }

    void Start()
    {
        randomspeed = new System.Random(8675309);
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();

        level_map = new int[map_width, map_height];
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                level_map[x, y] = 0;
            }
        }

        string map_string = Map.GetMapString();

        int t = 0;
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                level_map[x, y] = int.Parse(map_string[t].ToString());
                t += 1;
            }
        }
    }


    //void Update()
    //{

    //}

    public void UpdateMap(int x, int y, int value)
    {
        Debug.Log(x);
        Debug.Log(y);
        Debug.Log(value);
        level_map[x, y] = value;
    }

    public int GetValue(int x, int y)
    {
        return level_map[x, y];
    }

    public void GetPositionByeValue(out int x, out int y, int value)
    {
        for (int j = 0; j < map_height; j++)
        {
            for (int i = 0; i < map_width; i++)
            {
                if(level_map[i, j] == value)
                {
                    x = i;
                    y = j;

                    return;
                }
            }
        }

        x = 0;
        y = 0;
    }

}