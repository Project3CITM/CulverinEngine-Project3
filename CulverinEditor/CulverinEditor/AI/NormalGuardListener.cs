using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class NormalGuardListener : PerceptionListener
{
    public int hear_range = 2;

    void Start()
    {
        GetLinkedObject("event_manager").GetComponent<PerceptionManager>().AddListener(this);
        events_in_memory = new List<PerceptionEvent>();
    }

    void Update()
    {
        UpdateMemory();
    }

    public override void OnEventRecieved(PerceptionEvent event_recieved)
    {
        Debug.Log("number of events: " + events_in_memory.Count.ToString());
        Debug.Log("event memory time = " + event_recieved.time_in_memory.ToString());

        if (IsPriotitaryEvent(event_recieved))
        {
            ClearEvents();
            Debug.Log("Cleared other events");
            events_in_memory.Add(event_recieved);
        }
        else return;

        switch (event_recieved.type)
        {
            case PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT:
            case PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER:

                if(event_recieved.type == PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER)
                {
                    //Check if i have seen the player before
                    //If i do, i will detect this 
                }

                if (OnHearRange(event_recieved))
                {
                    GetLinkedObject("my_self").GetComponent<BT>().heard_something = true;
                    GetLinkedObject("my_self").GetComponent<Investigate_Action>().forgot_event = false;
                    GetLinkedObject("my_self").GetComponent<BT>().InterruptAction();

                    Debug.Log("I Heard Somethin");

                    event_recieved.start_counting = false;
                }

                break;    

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:
                GetLinkedObject("my_self").GetComponent<BT>().InterruptAction();
                GetLinkedObject("my_self").GetComponent<BT>().player_detected = true;
                break;

           
        }
    }

    public override void OnEventGone(PerceptionEvent event_recieved)
    {
        switch (event_recieved.type)
        {
            case PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT:
            case PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER:

                GetLinkedObject("my_self").GetComponent<BT>().heard_something = false;
                GetLinkedObject("my_self").GetComponent<Investigate_Action>().forgot_event = true;


                Debug.Log("I lost Somethin");
                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:
                GetLinkedObject("my_self").GetComponent<BT>().player_detected = false;
                GetLinkedObject("my_self").GetComponent<ChasePlayer_Action>().forgot_event = true;

                break;

            
        }

        events_in_memory.Remove(event_recieved);
    }

    bool OnHearRange(PerceptionEvent event_heard)
    {
        PerceptionHearEvent tmp = (PerceptionHearEvent)event_heard;

        int my_tile_x = GetLinkedObject("my_self").GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetLinkedObject("my_self").GetComponent<Movement_Action>().GetCurrentTileY();

        Debug.Log(my_tile_x.ToString());
        Debug.Log(my_tile_y.ToString());

        if (hear_range < tmp.radius_in_tiles)
        {
            Debug.Log("Hear < radius");

            if (RadiusOverlap(my_tile_x, my_tile_y, hear_range, tmp.objective_tile_x, tmp.objective_tile_y, tmp.radius_in_tiles))
                return true;

            return false;
        }
        else
        {
            Debug.Log("Hear > radius");

            if (RadiusOverlap(tmp.objective_tile_x, tmp.objective_tile_y, tmp.radius_in_tiles, my_tile_x, my_tile_y, hear_range))
                return true;

            return false;
        }
    }

    bool RadiusOverlap(int little_tile_x, int little_tile_y, int little_radius, int big_tile_x, int big_tile_y, int big_radius)
    {
        //Check if one of the four boundaries is inside the bigger radius
        //x - radius, y - radius
        if (((little_tile_x - little_radius) >= (big_tile_x - big_radius)) && ((little_tile_y - little_radius) >= (big_tile_y - big_radius)) && ((little_tile_x - little_radius) <= (big_tile_x + big_radius)) && ((little_tile_y - little_radius) <= (big_tile_y + big_radius)))
            return true;

        //x - radius, y + radius
        if (((little_tile_x - little_radius) >= (big_tile_x - big_radius)) && ((little_tile_y + little_radius) >= (big_tile_y - big_radius)) && ((little_tile_x - little_radius) <= (big_tile_x + big_radius)) && ((little_tile_y + little_radius) <= (big_tile_y + big_radius)))
            return true;

        //x + radius, y - radius
        if (((little_tile_x + little_radius) >= (big_tile_x - big_radius)) && ((little_tile_y - little_radius) >= (big_tile_y - big_radius)) && ((little_tile_x + little_radius) <= (big_tile_x + big_radius)) && ((little_tile_y - little_radius) <= (big_tile_y + big_radius)))
            return true;

        //x + radius, y + radius
        if (((little_tile_x + little_radius) >= (big_tile_x - big_radius)) && ((little_tile_y + little_radius) >= (big_tile_y - big_radius)) && ((little_tile_x + little_radius) <= (big_tile_x + big_radius)) && ((little_tile_y + little_radius) <= (big_tile_y + big_radius)))
            return true;

        return false;
    }

}

