using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;


public class TrapTile : CulverinBehaviour
{
    enum CHANGE_STATE
    {
        TRAP_IDLE,
        MOVING_UP,
        MOVING_DOWN
    }

    public float max_height = 25.4f;
    public float min_height = 0.0f;
    public float speed_up = 10.0f;
    public float speed_down = 30.0f;

    public bool trap_walkable = false;
    CHANGE_STATE curr_state = 0;

    private int tile_x = 0;
    private int tile_z = 0;
    private int value = 0;

    private GameObject map = null;

    void Start()
    {

        Vector3 global_pos = transform.GetGlobalPosition();
        tile_x = (int)((global_pos.x + (12.7f)) / 25.4);
        tile_z = (int)((global_pos.z + (12.7f)) / 25.4);

        if (trap_walkable)
        {
            global_pos.y = min_height;
            value = 0;
        } 
        else
        {
            global_pos.y = max_height;
            value = 1;
        }
        GetComponent<Transform>().SetPosition(global_pos);

        map = GetLinkedObject("map_obj");
        LevelMap map_level = map.GetComponent<LevelMap>();
        if (map_level != null)
        {
            map_level.UpdateMap(tile_x, tile_z, value);
        }

        curr_state = CHANGE_STATE.TRAP_IDLE;
    }

    void Update()
    {
        if (curr_state != 0)
        {
            if (curr_state == CHANGE_STATE.MOVING_DOWN)
            {
                Vector3 pos = GetComponent<Transform>().GetPosition();
                if (pos.y > min_height)
                {
                    pos.y -= speed_down * Time.deltaTime;
                    GetComponent<Transform>().SetPosition(pos);
                }
                else
                {
                    pos.y = min_height;
                    GetComponent<Transform>().SetPosition(pos);
                    curr_state = CHANGE_STATE.TRAP_IDLE;
                    LevelMap map_level = map.GetComponent<LevelMap>();
                    if(map_level != null)
                    {
                        map_level.UpdateMap(tile_x, tile_z, value);
                    }
                }
            }
            else
            {
                Vector3 pos = GetComponent<Transform>().GetPosition();
                if (pos.y < max_height)
                {
                    pos.y += speed_up * Time.deltaTime;
                    GetComponent<Transform>().SetPosition(pos);
                }
                else
                {
                    pos.y = max_height;
                    GetComponent<Transform>().SetPosition(pos);
                    curr_state = CHANGE_STATE.TRAP_IDLE;
                }
            }
        }
    }

    public void SwitchTileState()
    {
        trap_walkable = !trap_walkable;
        if (trap_walkable)
        {
            curr_state = CHANGE_STATE.MOVING_DOWN;
            value = 0;
        }
        else
        {
            curr_state = CHANGE_STATE.MOVING_UP;
            value = 1;
            LevelMap map_level = map.GetComponent<LevelMap>();
            if (map_level != null)
            {
                map_level.UpdateMap(tile_x, tile_z, value);
            }
        }
    }

}