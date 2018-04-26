using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class DoorLevel2 : CulverinBehaviour
{
    public bool door_closed = false;

    public int needed_open_calls = 1;
    int current_open_calls = 0;

    public float max_height = 25.4f;
    public float min_height = 0.0f;
    public float speed_up = 10.0f;
    public float speed_down = 30.0f;

     int door_pos_x = 0;
     int door_pos_y = 0;

    private bool go_close = false;
    private bool go_open = false;

    CompAudio audio;

    void Start()
    {
        audio = GetComponent<CompAudio>();

        Vector3 global_pos = transform.GetGlobalPosition();
        door_pos_x = (int)((global_pos.x + (12.7f)) / 25.4);
        door_pos_y = (int)((global_pos.z + (12.7f)) / 25.4);

        if (door_closed)
        {
            Vector3 pos = GetComponent<Transform>().GetPosition();
            if (pos.y != min_height)
            {
                pos.y = min_height;
                GetComponent<Transform>().SetPosition(pos);
            }
            GetLinkedObject("map_obj").GetComponent<LevelMap>().UpdateMap(door_pos_x, door_pos_y, 1);
        }
        else
        {
            Vector3 pos = GetComponent<Transform>().GetPosition();
            if (pos.y != max_height)
            {
                pos.y = max_height;
                GetComponent<Transform>().SetPosition(pos);
            }
            GetLinkedObject("map_obj").GetComponent<LevelMap>().UpdateMap(door_pos_x, door_pos_y, 0);
        }
    }

    void Update()
    {
        if (go_close)
        {
            Vector3 pos = GetComponent<Transform>().GetPosition();
            if (pos.y > min_height)
            {
                pos.y -= speed_down * Time.deltaTime;
                GetComponent<Transform>().SetPosition(pos);
            }
            else
            {
                Debug.Log("DOOR CLOSED");
                go_close = false;
                door_closed = true;
                //  audio.StopEvent("DoorLoop");
                GetComponent<CompAudio>().PlayEvent("DoorEnd");
                Input.RumblePlay(1.0f, 500);
            }
        }
        else if (go_open)
        {
            Vector3 pos = GetComponent<Transform>().GetPosition();
            if (pos.y < max_height)
            {
                pos.y += speed_up * Time.deltaTime;
                GetComponent<Transform>().SetPosition(pos);
            }
            else
            {
                Debug.Log("DOOR OPENED", Department.STAGE);
                GetLinkedObject("map_obj").GetComponent<LevelMap>().UpdateMap(door_pos_x, door_pos_y, 0);
                go_open = false;
                door_closed = false;
                // audio.StopEvent("DoorLoop");
                GetComponent<CompAudio>().PlayEvent("DoorEnd");
                Input.RumblePlay(1.0f, 500);
            }
        }
    }

    public void OpenDoor()
    {
        current_open_calls++;
        if(current_open_calls == needed_open_calls)
        {
            current_open_calls = 0;
            go_open = true;
            audio.PlayEvent("DoorLoop");
            Input.RumblePlay(0.3f, 5000);
        }
    }

    public void CloseDoor()
    {
        go_close = true;
        GetLinkedObject("map_obj").GetComponent<LevelMap>().UpdateMap(door_pos_x, door_pos_y, 1);
        audio.PlayEvent("DoorLoop");
        Input.RumblePlay(0.3f, 5000);
    }

    public void ChangeStateDoor()
    {
        if (door_closed)
        {
            go_open = true;
            audio.PlayEvent("DoorLoop");
            Input.RumblePlay(0.3f, 5000);
        }
        else
        {
            go_close = true;
            audio.PlayEvent("DoorLoop");
            Input.RumblePlay(0.3f, 5000);
            GetLinkedObject("map_obj").GetComponent<LevelMap>().UpdateMap(door_pos_x, door_pos_y, 1);
        }
    }
}