using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;
using System.Collections.Generic;
using System.Collections;
using System;
using System.Text;
using CulverinEditor.Pathfinding;

public class Pathfinder : CulverinBehaviour
{
    //Map settings
    int[,] tile_map;
    int map_width = 0;
    int map_height = 0;

    //Pathfind
    List<PathNode> open_list;
    List<PathNode> close_list;
    List<PathNode> path_list;

    Dictionary<string, PathNode> occupied_tiles;

    // Use this for initialization
    public void Start()
    {
        //Pathfinding
        open_list = new List<PathNode>();
        close_list = new List<PathNode>();
        occupied_tiles = new Dictionary<string, PathNode>();

        //Map Settings
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();

        tile_map = new int[map_width, map_height];

        string map = Map.GetMapString();
        int string_pos = 0;

        //Get the map and put it in array 2d
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                //Parse needs using System.Text
                tile_map[x, y] = int.Parse(map[string_pos].ToString());
                string_pos += 1;
            }
        }

    }

    void Update()
    {

    }

    public List<PathNode> CalculatePath(PathNode origin, PathNode destiny, bool recalculate = false)
    {
        //Clean both lists
        open_list.Clear();
        close_list.Clear();

        //Add my current position to the open list of them
        open_list.Add(origin);

        //We will recalculate if next tile is occupied
        if(recalculate)
        {
            //1 is to set occupied tiles as non walkable
            SetOccupiedTilesValue(1);
        }

        while (open_list.Count > 0)
        {
            if (IsInCloseList(destiny) != null)
                break;

            //Get the nearest node
            PathNode current_tile = GetLowestScoreNode();
            close_list.Add(current_tile);

            //Erase it form the open list
            open_list.Remove(current_tile);

            //Get the adjacent walkable nodes of the lowest node
            List<PathNode> adjacent_nodes = GetWalkableAdjacents(current_tile);

            foreach (PathNode node in adjacent_nodes)
            {
                //If it is in the closed list we will ignore it
                if (IsInCloseList(node) != null)
                    continue;

                //If it isn't in the open list we will set it and push it to the list

                PathNode tmp_node = IsInOpenList(node);

                if (tmp_node == null)
                {
                    node.CalculateDistance(destiny);
                    node.CalculateCost();
                    node.SetParent(current_tile);

                    open_list.Add(node);
                }
                else
                {
                    //If it is in the open list we will check if with the new parent
                    //it's score is lower than before, and if it is update;
                    PathNode actual_parent = tmp_node.GetParent();
                    int old_score = tmp_node.GetScore();

                    tmp_node.SetParent(current_tile);
                    int new_score = tmp_node.GetScore();

                    if (old_score < new_score)
                        tmp_node.SetParent(actual_parent);
                }
            }


        }

        FillPathList();

        //We are done reset the occupied tiles to walkable
        if (recalculate)
        {
            //0 is to set occupied tiles as walkable
            SetOccupiedTilesValue(0);
        }

        return path_list;

    }

    public PathNode GetLowestScoreNode()
    {

        if (open_list.Count == 0)
        {
            return null;
        }

        PathNode ret = open_list[0];

        foreach (PathNode node in open_list)
        {
            if (node.GetScore() < ret.GetScore())
                ret = node;
        }

        return ret;
    }

    private PathNode IsInCloseList(PathNode node)
    {
        foreach (PathNode node_in_list in close_list)
        {
            if (node_in_list.IsEqual(node))
                return node_in_list;
        }

        return null;
    }

    private PathNode IsInOpenList(PathNode node)
    {
        foreach (PathNode node_in_list in open_list)
        {
            if (node_in_list.IsEqual(node))
                return node_in_list;
        }

        return null;
    }

    private void FillPathList()
    {
        path_list = new List<PathNode>();

        PathNode new_node = close_list[close_list.Count - 1];

        path_list.Add(new_node);

        while (new_node.GetParent() != null)
        {
            new_node = new_node.GetParent();
            path_list.Add(new_node);
        }

        path_list.Reverse();
        path_list.Remove(path_list[0]);

    }

    public int GetMapWidth()
    {
        return map_width;
    }

    public int GetMapHeight()
    {
        return map_height;
    }

    public List<PathNode> GetWalkableAdjacents(PathNode node, uint range = 1)
    {
        List<PathNode> ret = new List<PathNode>();

        for (int i = (int)-range; i <= range; i++)
        {
            if (i == 0)
                continue;

            if (node.GetTileX() + i > 0 && node.GetTileX() + i < map_width)
            {
                if (tile_map[node.GetTileX() + i, node.GetTileY()] != 1)
                    ret.Add(new PathNode(node.GetTileX() + i, node.GetTileY()));
            }

            if (node.GetTileY() + i > 0 && node.GetTileY() + i < map_height)
            {
                if (tile_map[node.GetTileX(), node.GetTileY() + i] != 1)
                    ret.Add(new PathNode(node.GetTileX(), node.GetTileY() + i));
            }
        }

        return ret;
    }

    public bool IsWalkableTile(uint tile_to_check_x, uint tile_to_check_y)
    {
        //Check if the tiles are inside the map
        if (tile_to_check_x < map_width && tile_to_check_y < map_height)
        {
            if (tile_map[tile_to_check_x, tile_to_check_y] == 0 || tile_map[tile_to_check_x, tile_to_check_y] == 2)
                return true;
        }
        return false;
    }

    public bool IsWalkableTile(PathNode pn)
    {
        //Check if the tiles are inside the map
        if (pn.GetTileX() < map_width && pn.GetTileY() < map_height)
        {
            if (tile_map[pn.GetTileX(), pn.GetTileY()] == 0 || tile_map[pn.GetTileX(), pn.GetTileY()] == 2)
                return true;
        }
        return false;
    }

    private void SetOccupiedTilesValue(int walkability)
    {
        foreach (KeyValuePair<string, PathNode> node in occupied_tiles)
        {
            int occupied_x = node.Value.GetTileX();
            int occupied_y = node.Value.GetTileY();

            if (IsWalkableTile((uint)occupied_x, (uint)occupied_y) == true)
                tile_map[occupied_x, occupied_y] = walkability;
        }
    }

    public void UpdateOccupiedTiles(string name, PathNode tile)
    {
        occupied_tiles[name] = tile;
    }

    public bool IsOccupiedTile(int x, int y)
    {
        foreach (KeyValuePair<string, PathNode> node in occupied_tiles)
        {
            if (node.Value.GetTileX() == x && node.Value.GetTileY() == y)
                return true;
        }

        return false;
    }

    public bool IsOccupiedTile(PathNode tile)
    {
        foreach (KeyValuePair<string, PathNode> node in occupied_tiles)
        {
            if (node.Value.IsEqual(tile))
                return true;
        }

        return false;
    }
}

