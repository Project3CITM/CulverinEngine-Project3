using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;
using System.Collections.Generic;
using System.Collections;
using System;
using System.Text;


namespace CulverinEditor.Pathfinding
{
    public class Pathfinder
    {
        //Map settings
        int[,] tile_map;
        int map_width = 0;
        int map_height = 0;

        //Pathfind
        List<PathNode> open_list;
        List<PathNode> close_list;
        List<PathNode> path_list;

        // Use this for initialization
        public void Init()
        {
            //Pathfinding
            open_list = new List<PathNode>();
            close_list = new List<PathNode>();
            path_list = new List<PathNode>();

            //Map Settings
            map_width = Map.Map.GetWidthMap();
            map_height = Map.Map.GetHeightMap();

            tile_map = new int[map_width, map_height];

            string map = Map.Map.GetMapString();
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

        public List<PathNode> CalculatePath(PathNode origin, PathNode destiny)
        {
            //Clean both lists
            open_list.Clear();
            close_list.Clear();

            //Add my current position to the open list of them
            open_list.Add(origin);

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

            return path_list;

        }

        public PathNode GetLowestScoreNode()
        {

            if (open_list.Count == 0)
            {
                Debug.Debug.Log("Open List is Empty");
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
            foreach(PathNode node_in_list in close_list)
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
            //Clean the return list
            if (path_list.Count > 0)
                path_list.Clear();

            PathNode new_node = close_list[close_list.Count - 1];

            path_list.Add(new_node);

            while (new_node.GetParent() != null)
            {
                    new_node = new_node.GetParent();
                    path_list.Add(new_node);
            }

            path_list.Reverse();
        }

        public int GetMapWidth()
        {
            return map_width;
        }

        public int GetMapHeight()
        {
            return map_height;
        }

        public List<PathNode> GetWalkableAdjacents(PathNode node)
        {
            List<PathNode> ret = new List<PathNode>();

            if (node.GetTileX() < map_width - 1)
            {
                if (tile_map[node.GetTileX() + 1, node.GetTileY()] == 0)
                    ret.Add(new PathNode(node.GetTileX() + 1, node.GetTileY()));
            }

            if (node.GetTileX() > 0)
            {
                if (tile_map[node.GetTileX() - 1, node.GetTileY()] == 0)
                    ret.Add(new PathNode(node.GetTileX() - 1, node.GetTileY()));
            }

            if (node.GetTileY() < map_height - 1)
            {
                if (tile_map[node.GetTileX(), node.GetTileY() + 1] == 0)
                    ret.Add(new PathNode(node.GetTileX(), node.GetTileY() + 1));
            }

            if (node.GetTileY() > 0)
            {
                if (tile_map[node.GetTileX(), node.GetTileY() - 1] == 0)
                    ret.Add(new PathNode(node.GetTileX(), node.GetTileY() - 1));
            }

            return ret;

        }

    }
}
