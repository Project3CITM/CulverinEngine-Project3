using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;
using System.Collections.Generic;
using System.Collections;

public class Pathfinder : CulverinBehaviour
{
    //Map settings
    static int[,] tile_map;
    static int map_width = 0;
    static int map_height = 0;

    //Pathfind
    public class PathNode
    {
       public PathNode(int _x, int _y)
        {
            x = _x;
            y = _y;
        }
       public PathNode(int _x, int _y, PathNode _parent)
        {
            x = _x;
            y = _y;
            parent = _parent;
        }

        //Tile position
        int x, y;

        uint distance, cost;
        PathNode parent = null;

        public int GetScore()
        {
            return (int)(distance + cost);
        }

        public void CalculateDistance(PathNode destiny)
        {
            uint x_distance = (uint)(destiny.x - x);
            uint y_distance = (uint)(destiny.y - y);

            distance = x_distance + y_distance;
        }

        public void CalculateDistance(int _x, int _y)
        {
            uint x_distance = (uint)(_x - x);
            uint y_distance = (uint)(_y - y);

            distance = x_distance + y_distance;
        }

        public void CalculateCost()
        {
            if (parent == null)
            {
                cost = 0;
            }
            else
            {
                cost = parent.cost + 1;
            }
        }

        public List<PathNode> GetWalkableAdjacents()
        {
            List<PathNode> ret = new List<PathNode>();

            if (x < map_width - 1)
            {
                if (tile_map[x + 1, y] == 1)
                    ret.Add(new PathNode(x + 1, y));
            }

            if (x > 0)
            {
                if (tile_map[x - 1, y] == 1)
                    ret.Add(new PathNode(x - 1, y));
            }

            if (y < map_height - 1)
            {
                if (tile_map[x, y + 1] == 1)
                    ret.Add(new PathNode(x, y + 1));
            }

            if (y > 0)
            {
                if (tile_map[x, y - 1] == 1)
                    ret.Add(new PathNode(x, y - 1));
            }

            return ret;

        }

        public void SetParent(PathNode new_parent)
        {
            if (new_parent != null)
            {
                parent = new_parent;
                CalculateCost();
            }

            Debug.Log("New Parent was null");

        }

        public PathNode GetParent()
        {
            return parent;
        }

        public int GetTileX()
        {
            return x;
        }

        public int GetTileY()
        {
            return y;
        }

    }

    List<PathNode> open_list;
    List<PathNode> close_list;
    List<PathNode> path_list;

    // Use this for initialization
    void Start()
    {
        //Pathfinding
        open_list = new List<PathNode>();
        close_list = new List<PathNode>();
        path_list = new List<PathNode>();

        //Map Settings
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();

        tile_map = new int[map_width, map_height];

        string map = Map.GetMapString();
        int string_pos = 0;

        //Get the map and put it in array 2d
        for (int i = 0; i < map_width; i++)
        {
            for (int j = 0; i < map_height; j++)
            {
                tile_map[i, j] = int.Parse(map[string_pos].ToString());
                string_pos += 1;
            }
        }

    }

    public List<PathNode> CalculatePath(PathNode origin, PathNode destiny)
    {
        //Clean both lists
        open_list.Clear();
        close_list.Clear();

        //Add my current position to the open list of them
        open_list.Add(origin);

        while (open_list.Count > 0)
        {
            if (close_list.Contains(destiny))
                break;

            //Get the nearest node
            PathNode current_tile = GetLowestScoreNode();
            close_list.Add(current_tile);

            //Erase it form the open list
            open_list.Remove(open_list[0]);

            //Get the adjacent walkable nodes of the lowest node
            List<PathNode> adjacent_nodes = current_tile.GetWalkableAdjacents();

            foreach(PathNode node in adjacent_nodes)
            {
                //If it is in the closed list we will ignore it
                if (close_list.Contains(node))
                    continue;

                //If it isn't in the open list we will set it and push it to the list
                if (!open_list.Contains(node))
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
                    PathNode actual_parent = node.GetParent();
                    int old_score = node.GetScore();

                    node.SetParent(current_tile);
                    int new_score = node.GetScore();

                    if (old_score < new_score)
                        node.SetParent(actual_parent);
                }
            }


        }

        FillPathList();

        return path_list;

    }

    public PathNode GetLowestScoreNode()
    {

        if(open_list.Count == 0)
        {
            Debug.Log("Open List is Empty");
            return null;
        }

        PathNode ret = open_list[0];

        foreach(PathNode node in open_list)
        {
            if (node.GetScore() < ret.GetScore())
                ret = node;
        }

        return ret;
    }

    void FillPathList()
    {
        //Clean the return list
        if (path_list.Count > 0)
            path_list.Clear();

        PathNode new_node = close_list[close_list.Count - 1];

        do
        {
            path_list.Add(new_node);
            new_node = new_node.GetParent();

        } while (new_node.GetParent() != null);
    }

    

}
