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

    CompAudio audio = null;

    void Start()
    {

        audio = GetComponent<CompAudio>();
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
    
 

    void OnTriggerEnter()
    {
        audio.PlayEvent("SwitchOn");
        SwitchTraps();
    }

    void OnTriggerLost()
    {
        audio.PlayEvent("SwitchOff");
    }

    public void SwitchTraps()
    {
        for (int i = 0; i < childs_to_switch.Count; i++)
        {
            childs_to_switch[i].GetComponent<TrapTile>().SwitchTileState();
        }
    }
}