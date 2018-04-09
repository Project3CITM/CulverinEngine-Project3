using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;
using CulverinEditor.Map;
using CulverinEditor.Pathfinding;
using System.Text;

public class PerceptionSightEnemy : CulverinBehaviour
{
    GameObject player_obj;
    public GameObject map;
    Pathfinder pathfinder;

    public int frustum_lenght = 3;
    public float time_to_forget = 10.0f;

    public float check_player_timer = 0.5f;
    float timer = 0.0f;

    public bool player_seen = false;
    public bool can_see_around = true;

    public enum DIRECTION
    {
        E_DIR_NONE = -1,
        E_DIR_NORTH,
        E_DIR_SOUTH,
        E_DIR_EAST,
        E_DIR_WEST,
        E_DIR_MAX
    }

    void Start()
    {
        player_obj = GetLinkedObject("player_obj");
        pathfinder = null;
        map = GetLinkedObject("map");
        pathfinder = map.GetComponent<Pathfinder>();
        player_seen = false;
    }

    void Update()
    {
        timer += Time.deltaTime;

        if (timer >= check_player_timer)
        {
            timer = 0.0f;

            List<PathNode> seen_tiles = GetSeenTiles(GetDirection());

            player_obj.GetComponent<MovementController>().GetPlayerPos(out int player_x, out int player_y);

            PathNode player_tile = new PathNode(player_x, player_y);

            bool seen_this_frame = false;

            foreach (PathNode n in seen_tiles)
            {
                if (n.IsEqual(player_tile))
                {
                    seen_this_frame = true;
                    break;
                }
            }

            if (seen_this_frame == false)
            {
                player_seen = false;
            }
            else
            {
                player_seen = true;
                int current_x = GetComponent<Movement_Action>().GetCurrentTileX();
                int current_y = GetComponent<Movement_Action>().GetCurrentTileY();
                GetComponent<PerceptionEmitter>().TriggerPlayerSeenEvent(time_to_forget, player_x, player_y, gameObject, current_x, current_y);
            }
        }
    }

    public DIRECTION GetDirection()
    {
        DIRECTION ret = DIRECTION.E_DIR_NONE;

        Vector3 forward = new Vector3(GetComponent<Transform>().GetForwardVector());
        float delta = Mathf.Atan2(forward.x, forward.z);

        if (delta > Mathf.PI)
            delta = delta - 2 * Mathf.PI;
        if (delta < (-Mathf.PI))
            delta = delta + 2 * Mathf.PI;

        if (delta <= (Mathf.PI / 4) && delta >= -(Mathf.PI / 4))
            ret = DIRECTION.E_DIR_SOUTH;
        else if (delta >= (Mathf.PI / 4) && delta <= 3 * (Mathf.PI / 4))
            ret = DIRECTION.E_DIR_EAST;
        else if (delta >= (3 * (Mathf.PI / 4)) || delta <= -(3 * (Mathf.PI / 4)))
            ret = DIRECTION.E_DIR_NORTH;
        else if (delta <= -(Mathf.PI / 4) && delta >= -(3 * (Mathf.PI / 4)))
            ret = DIRECTION.E_DIR_WEST;

        return ret;
    }

    public void GetPlayerTilePos(out int player_x, out int player_y)
    {
        player_x = 0;
        player_y = 0;

        if (player_obj != null)
        {
            MovementController temp = player_obj.GetComponent<MovementController>();
            if (temp != null)
            {
                temp.GetPlayerPos(out int x, out int y);
                player_x = x;
                player_y = y;
            }
        }
    }

    private List<PathNode> GetSeenTiles(DIRECTION dir)
    {
        List<PathNode> tiles = new List<PathNode>();
        List<PathNode> unseen_tiles = new List<PathNode>();
        int current_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int current_y = GetComponent<Movement_Action>().GetCurrentTileY();

        // -------- Add the tiles in a circle -------- //
        // ------- [Adds realism to the enemy] ------- //
        if (can_see_around)
        {
            switch (dir)
            {
                case DIRECTION.E_DIR_NORTH:
                    tiles.Add(new PathNode(current_x - 1, current_y));
                    tiles.Add(new PathNode(current_x + 1, current_y));

                    tiles.Add(new PathNode(current_x - 1, current_y + 1));
                    tiles.Add(new PathNode(current_x, current_y + 1));
                    tiles.Add(new PathNode(current_x + 1, current_y + 1));
                    break;

                case DIRECTION.E_DIR_SOUTH:
                    tiles.Add(new PathNode(current_x - 1, current_y - 1));
                    tiles.Add(new PathNode(current_x, current_y - 1));
                    tiles.Add(new PathNode(current_x + 1, current_y - 1));

                    tiles.Add(new PathNode(current_x - 1, current_y));
                    tiles.Add(new PathNode(current_x + 1, current_y));
                    break;

                case DIRECTION.E_DIR_EAST:
                    tiles.Add(new PathNode(current_x - 1, current_y - 1));
                    tiles.Add(new PathNode(current_x, current_y - 1));

                    tiles.Add(new PathNode(current_x - 1, current_y));

                    tiles.Add(new PathNode(current_x - 1, current_y + 1));
                    tiles.Add(new PathNode(current_x, current_y + 1));
                    break;

                case DIRECTION.E_DIR_WEST:
                    tiles.Add(new PathNode(current_x, current_y - 1));
                    tiles.Add(new PathNode(current_x + 1, current_y - 1));

                    tiles.Add(new PathNode(current_x + 1, current_y));

                    tiles.Add(new PathNode(current_x, current_y + 1));
                    tiles.Add(new PathNode(current_x + 1, current_y + 1));
                    break;

                case DIRECTION.E_DIR_NONE:
                case DIRECTION.E_DIR_MAX:
                default:
                    Debug.Log("[error] GetSeenTiles: Wrong Direction: " + dir);
                    break;
            }
        }
        // --------------------------------------------- //

        switch (dir)
        {
            case DIRECTION.E_DIR_NORTH:
                for (int i = 0; i < frustum_lenght; i++)
                {
                    int pos_y = current_y - i;
                    for (int j = -i; j <= i; j++)
                    {
                        int pos_x = current_x + j;

                        if (pathfinder.IsWalkableTile((uint)pos_x, (uint)pos_y) == false)
                        {
                            for (int k = 0; k < frustum_lenght - i; k++)
                            {
                                int unseen_pos_y = current_y - i - k;
                                if (j < 0)
                                {
                                    for (int l = 0; l <= k; l++)
                                    {
                                        int unseen_pos_x = current_x + j - l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                                else if (j > 0)
                                {
                                    for (int l = 0; l <= k; l++)
                                    {
                                        int unseen_pos_x = current_x + j + l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                                else if (j == 0)
                                {
                                    for (int l = -k; l <= k; l++)
                                    {
                                        int unseen_pos_x = current_x + j + l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                            }
                        }

                        PathNode tile = new PathNode(pos_x, pos_y);
                        bool is_unseen = false;

                        foreach (PathNode n in unseen_tiles)
                            if (tile.IsEqual(n))
                                is_unseen = true;
                        if (is_unseen == false)
                            if (pos_x > 0 && pos_y > 0)
                                tiles.Add(new PathNode(pos_x, pos_y));
                    }
                }
                break;
            case DIRECTION.E_DIR_SOUTH:
                for (int i = 0; i < frustum_lenght; i++)
                {
                    int pos_y = current_y + i;
                    for (int j = -i; j <= i; j++)
                    {
                        int pos_x = current_x + j;

                        if (pathfinder.IsWalkableTile((uint)pos_x, (uint)pos_y) == false)
                        {
                            for (int k = 0; k < frustum_lenght - i; k++)
                            {
                                int unseen_pos_y = current_y + i + k;
                                if (j < 0)
                                {
                                    for (int l = 0; l <= k; l++)
                                    {
                                        int unseen_pos_x = current_x + j - l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                                else if (j > 0)
                                {
                                    for (int l = 0; l <= k; l++)
                                    {
                                        int unseen_pos_x = current_x + j + l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                                else if (j == 0)
                                {
                                    for (int l = -k; l <= k; l++)
                                    {
                                        int unseen_pos_x = current_x + j + l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                            }
                        }

                        PathNode tile = new PathNode(pos_x, pos_y);
                        bool is_unseen = false;

                        foreach (PathNode n in unseen_tiles)
                            if (tile.IsEqual(n))
                                is_unseen = true;
                        if (is_unseen == false)
                            tiles.Add(new PathNode(pos_x, pos_y));
                    }
                }
                break;
            case DIRECTION.E_DIR_EAST:
                for (int i = 0; i < frustum_lenght; i++)
                {
                    int pos_x = current_x + i;
                    for (int j = -i; j <= i; j++)
                    {
                        int pos_y = current_y + j;

                        if (pathfinder.IsWalkableTile((uint)pos_x, (uint)pos_y) == false)
                        {
                            for (int k = 0; k < frustum_lenght - i; k++)
                            {
                                int unseen_pos_x = current_x + i + k;
                                if (j < 0)
                                {
                                    for (int l = 0; l <= k; l++)
                                    {
                                        int unseen_pos_y = current_y + j - l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                                else if (j > 0)
                                {
                                    for (int l = 0; l <= k; l++)
                                    {
                                        int unseen_pos_y = current_y + j + l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                                else if (j == 0)
                                {
                                    for (int l = -k; l <= k; l++)
                                    {
                                        int unseen_pos_y = current_y + j + l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                            }
                        }

                        PathNode tile = new PathNode(pos_x, pos_y);
                        bool is_unseen = false;

                        foreach (PathNode n in unseen_tiles)
                            if (tile.IsEqual(n))
                                is_unseen = true;
                        if (is_unseen == false)
                            tiles.Add(new PathNode(pos_x, pos_y));
                    }
                }
                break;
            case DIRECTION.E_DIR_WEST:
                for (int i = 0; i < frustum_lenght; i++)
                {
                    int pos_x = current_x - i;
                    for (int j = -i; j <= i; j++)
                    {
                        int pos_y = current_y + j;

                        if (pathfinder.IsWalkableTile((uint)pos_x, (uint)pos_y) == false)
                        {
                            for (int k = 0; k < frustum_lenght - i; k++)
                            {
                                int unseen_pos_x = current_x - i - k;
                                if (j < 0)
                                {
                                    for (int l = 0; l <= k; l++)
                                    {
                                        int unseen_pos_y = current_y + j - l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                                else if (j > 0)
                                {
                                    for (int l = 0; l <= k; l++)
                                    {
                                        int unseen_pos_y = current_y + j + l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                                else if (j == 0)
                                {
                                    for (int l = -k; l <= k; l++)
                                    {
                                        int unseen_pos_y = current_y + j + l;
                                        unseen_tiles.Add(new PathNode(unseen_pos_x, unseen_pos_y));
                                    }
                                }
                            }
                        }

                        PathNode tile = new PathNode(pos_x, pos_y);
                        bool is_unseen = false;

                        foreach (PathNode n in unseen_tiles)
                            if (tile.IsEqual(n))
                                is_unseen = true;
                        if (is_unseen == false)
                            tiles.Add(new PathNode(pos_x, pos_y));
                    }
                }
                break;
        }

        return tiles;
    }
}