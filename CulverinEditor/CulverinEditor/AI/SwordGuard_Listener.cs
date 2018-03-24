using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class SwordGuard_Listener : PerceptionListener
{
    public int hear_range = 2;
    bool player_seen = false;

    void Start()
    {
        event_manager = GetLinkedObject("event_manager");
        event_manager.GetComponent<PerceptionManager>().AddListener(this);
        events_in_memory = new List<PerceptionEvent>();
    }

    void Update()
    {
        UpdateMemory();
    }

    public override void OnEventRecieved(PerceptionEvent event_recieved)
    {
        if (IsPriotitaryEvent(event_recieved) == true)
        {
            ClearEvents();
        }
        else
        {
            return;
        }

        switch (event_recieved.type)
        {
            case PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT:
            case PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER:

                PerceptionHearEvent tmp = (PerceptionHearEvent)event_recieved;

                if (OnHearRange(tmp))
                {
                    PerceptionHearEvent event_to_memory = new PerceptionHearEvent(tmp);

                    if (event_recieved.type == PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER && player_seen)
                    {
                        GetComponent<EnemySword_BT>().heard_something = true;
                        GetComponent<Investigate_Action>().forgot_event = false;
                        GetComponent<Investigate_Action>().SetEvent(event_to_memory);
                        GetComponent<EnemySword_BT>().InterruptAction();
                        GetComponent<EnemySword_BT>().SetAction(Action.ACTION_TYPE.INVESTIGATE_ACTION);
                        events_in_memory.Add(event_to_memory);
                        event_to_memory.start_counting = false;
                    }
                    else
                    {
                        GetComponent<EnemySword_BT>().heard_something = true;
                        GetComponent<Investigate_Action>().forgot_event = false;
                        GetComponent<Investigate_Action>().SetEvent(event_to_memory);
                        GetComponent<EnemySword_BT>().InterruptAction();
                        GetComponent<EnemySword_BT>().SetAction(Action.ACTION_TYPE.INVESTIGATE_ACTION);
                        events_in_memory.Add(event_to_memory);
                        event_to_memory.start_counting = false;
                    }
                }
                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:

                PerceptionPlayerSeenEvent seen_event_tmp = (PerceptionPlayerSeenEvent)event_recieved;

                if (gameObject == seen_event_tmp.enemy_who_saw)
                {
                    PerceptionPlayerSeenEvent new_event_to_memory = new PerceptionPlayerSeenEvent(seen_event_tmp);
                    events_in_memory.Add(new_event_to_memory);
                    GetComponent<EnemySword_BT>().InterruptAction();
                    GetComponent<EnemySword_BT>().player_detected = true;
                    GetComponent<EnemySword_BT>().SetAction(Action.ACTION_TYPE.ENGAGE_ACTION);
                    GetComponent<ChasePlayer_Action>().SetEvent(new_event_to_memory);
                    player_seen = true;
                    new_event_to_memory.start_counting = false;
                }
                break;
        }
    }

    public override void OnEventGone(PerceptionEvent event_recieved)
    {
        switch (event_recieved.type)
        {
            case PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT:
            case PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER:
                GetComponent<EnemySword_BT>().heard_something = false;
                GetComponent<Investigate_Action>().forgot_event = true;
                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:
                GetComponent<EnemySword_BT>().player_detected = false;
                GetComponent<EnemySword_BT>().SetAction(Action.ACTION_TYPE.DISENGAGE_ACTION);
                GetComponent<ChasePlayer_Action>().forgot_event = true;
                break;
        }
    }

    bool OnHearRange(PerceptionHearEvent event_heard)
    {
        int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        if (hear_range < event_heard.radius_in_tiles)
        {
            if (RadiusOverlap(my_tile_x, my_tile_y, hear_range, event_heard.objective_tile_x, event_heard.objective_tile_y, event_heard.radius_in_tiles))
                return true;
            
            return false;
        }
        else
        {
            if (RadiusOverlap(event_heard.objective_tile_x, event_heard.objective_tile_y, event_heard.radius_in_tiles, my_tile_x, my_tile_y, hear_range))
                return true;
           
            return false;
        }
    }

    bool RadiusOverlap(int little_tile_x, int little_tile_y, int little_radius, int big_tile_x, int big_tile_y, int big_radius)
    {
        //Check if one of the four boundaries is inside the bigger radius
        //x - radius, y - radius
        if (((little_tile_x - little_radius) >= (big_tile_x - big_radius))
            && ((little_tile_y - little_radius) >= (big_tile_y - big_radius))
            && ((little_tile_x - little_radius) <= (big_tile_x + big_radius))
            && ((little_tile_y - little_radius) <= (big_tile_y + big_radius)))
            return true;

        //x - radius, y + radius
        if (((little_tile_x - little_radius) >= (big_tile_x - big_radius))
            && ((little_tile_y + little_radius) >= (big_tile_y - big_radius))
            && ((little_tile_x - little_radius) <= (big_tile_x + big_radius))
            && ((little_tile_y + little_radius) <= (big_tile_y + big_radius)))
            return true;

        //x + radius, y - radius
        if (((little_tile_x + little_radius) >= (big_tile_x - big_radius))
            && ((little_tile_y - little_radius) >= (big_tile_y - big_radius))
            && ((little_tile_x + little_radius) <= (big_tile_x + big_radius))
            && ((little_tile_y - little_radius) <= (big_tile_y + big_radius)))
            return true;

        //x + radius, y + radius
        if (((little_tile_x + little_radius) >= (big_tile_x - big_radius))
            && ((little_tile_y + little_radius) >= (big_tile_y - big_radius))
            && ((little_tile_x + little_radius) <= (big_tile_x + big_radius))
            && ((little_tile_y + little_radius) <= (big_tile_y + big_radius)))
            return true;

        return false;
    }
}

