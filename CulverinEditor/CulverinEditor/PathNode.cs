using CulverinEditor;
using CulverinEditor.Debug;

namespace CulverinEditor.Pathfinding
{

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

        public void SetParent(PathNode new_parent)
        {
            if (new_parent != null)
            {
                parent = new_parent;
                CalculateCost();
            }
           
            Debug.Debug.Log("New Parent was null");

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

        public bool IsEqual(PathNode node_to_compare)
        {
            if (node_to_compare == null)
                return false;

            if (node_to_compare.x == this.x && node_to_compare.y == this.y)
                return true;

            return false;
        }
    }
}