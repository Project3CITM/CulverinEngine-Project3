using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class TrapManager : CulverinBehaviour
{
    public bool door_tile_is_A = true;

    private GameObject map = null;

    private List<GameObject> childs_to_switch;

    private bool switching_states = false;

    void Start()
    {
        map = GetLinkedObject("map_obj");

        childs_to_switch = new List<GameObject>();

        bool stop = false;
        int num_childs = 0;
        while (stop == false)
        {
            GameObject temp = gameObject.GetChildByTagIndex("trap_floor", num_childs++);
            if (temp == null)
            {
                stop = true;
            }
            else
            { 
                childs_to_switch.Add(temp);
            }
        }
    }
    
    void Update()
    {
        if (switching_states)
        {
            int tile_x = 0;
            int tile_z = 0;
            int value = 0;
            bool switch_done = true;

            map = GetLinkedObject("map_obj");
            LevelMap map_level = map.GetComponent<LevelMap>();
            for (int i = 0; i < childs_to_switch.Count; i++)
            {
                if (childs_to_switch[i].GetComponent<TrapTile>().CheckChangeState(out tile_x, out tile_z, out value))
                {
                    map_level.UpdateMap(tile_x, tile_z, value);
                    switch_done = false;
                }
            }

            if(switch_done)
            {
                switching_states = false;
            }
        }
    }

    void OnTriggerEnter()
    {
        SwitchTraps();
    }

    public void SwitchTraps()
    {
        map = GetLinkedObject("map_obj");
        LevelMap map_level = map.GetComponent<LevelMap>();
        int curr_x = 0;
        int curr_z = 0;
        int curr_val = 0;

        for (int i = 0; i < childs_to_switch.Count; i++)
        {
            if(childs_to_switch[i].GetComponent<TrapTile>().SwitchTileState(out curr_x, out curr_z, out curr_val))
            {
                map_level.UpdateMap(curr_x, curr_z, 1);             
            }
            else
            {
                map_level.UpdateMap(curr_x, curr_z, 0);
            }
        }
        switching_states = true;
    }
}