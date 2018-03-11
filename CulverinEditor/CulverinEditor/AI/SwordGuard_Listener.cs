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
        my_self = GetLinkedObject("my_self");
        events_in_memory = new List<PerceptionEvent>();
    }

    void Update()
    {
        UpdateMemory();
    }

    public override void OnEventRecieved(PerceptionEvent event_recieved)
    {
        if (IsPriotitaryEvent(event_recieved))
        {
            ClearEvents();
        }
        else return;

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
                        my_self.GetComponent<EnemySword_BT>().heard_something = true;
                        my_self.GetComponent<Investigate_Action>().forgot_event = false;
                        my_self.GetComponent<Investigate_Action>().SetEvent(event_to_memory);
                        my_self.GetComponent<EnemySword_BT>().InterruptAction();


                        events_in_memory.Add(event_to_memory);

                        Debug.Log("I Heard The Player");

                        event_to_memory.start_counting = false;
                    }
                    else
                    {
                        my_self.GetComponent<EnemySword_BT>().heard_something = true;
                        my_self.GetComponent<Investigate_Action>().forgot_event = false;
                        my_self.GetComponent<Investigate_Action>().SetEvent(event_to_memory);
                        my_self.GetComponent<EnemySword_BT>().InterruptAction();

                        events_in_memory.Add(event_to_memory);

                        Debug.Log("I Heard Somethin");

                        event_to_memory.start_counting = false;
                    }
                }
                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:

                PerceptionPlayerSeenEvent seen_event_tmp = (PerceptionPlayerSeenEvent)event_recieved;

                if (my_self == seen_event_tmp.enemy_who_saw)
                {
                    Debug.Log("Player Seen");
                    PerceptionPlayerSeenEvent new_event_to_memory = new PerceptionPlayerSeenEvent(seen_event_tmp);
                    events_in_memory.Add(new_event_to_memory);

                    my_self.GetComponent<EnemySword_BT>().InterruptAction();
                    my_self.GetComponent<EnemySword_BT>().player_detected = true;
                    my_self.GetComponent<ChasePlayer_Action>().SetEvent(new_event_to_memory);
                    player_seen = true;

                    Debug.Log("Player in sight");
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
                my_self.GetComponent<EnemySword_BT>().heard_something = false;
                my_self.GetComponent<Investigate_Action>().forgot_event = true;
                Debug.Log("I lost Somethin");
                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:
                my_self.GetComponent<EnemySword_BT>().player_detected = false;
                my_self.GetComponent<ChasePlayer_Action>().forgot_event = true;
                Debug.Log("Player out of sight");
                break;
        }
    }

    bool OnHearRange(PerceptionEvent event_heard)
    {

        PerceptionHearEvent tmp = (PerceptionHearEvent)event_heard;

        int my_tile_x = my_self.GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = my_self.GetComponent<Movement_Action>().GetCurrentTileY();

        if (hear_range < tmp.radius_in_tiles)
        {
            if (RadiusOverlap(my_tile_x, my_tile_y, hear_range, tmp.objective_tile_x, tmp.objective_tile_y, tmp.radius_in_tiles))
                return true;

            return false;
        }
        else
        {
            if (RadiusOverlap(tmp.objective_tile_x, tmp.objective_tile_y, tmp.radius_in_tiles, my_tile_x, my_tile_y, hear_range))
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

