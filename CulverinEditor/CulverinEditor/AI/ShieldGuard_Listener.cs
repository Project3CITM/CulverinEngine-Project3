using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class ShieldGuard_Listener : PerceptionListener
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
        if (IsPriotitaryEvent(event_recieved))
        {
            ClearEvents();
            events_in_memory.Add(event_recieved);
        }
        else return;

        switch (event_recieved.type)
        {
            case PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT:
            case PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER:

                if (OnHearRange(event_recieved))
                {
                    if (event_recieved.type == PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER && player_seen)
                    {
                        GetComponent<EnemyShield_BT>().heard_something = true;
                        GetComponent<Investigate_Action>().forgot_event = false;
                        GetComponent<Investigate_Action>().SetEvent(event_recieved);
                        GetComponent<EnemyShield_BT>().InterruptAction();

                        Debug.Log("I Heard The Player");

                        event_recieved.start_counting = false;
                    }
                    else
                    {
                        GetComponent<EnemyShield_BT>().heard_something = true;
                        GetComponent<Investigate_Action>().forgot_event = false;
                        GetComponent<Investigate_Action>().SetEvent(event_recieved);
                        GetComponent<EnemyShield_BT>().InterruptAction();

                        Debug.Log("I Heard Somethin");

                        event_recieved.start_counting = false;
                    }
                }
                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:
                GetComponent<EnemyShield_BT>().InterruptAction();
                GetComponent<EnemyShield_BT>().player_detected = true;
                GetComponent<EnemyShield_BT>().SetAction(Action.ACTION_TYPE.ENGAGE_ACTION);
                player_seen = true;
                GetComponent<Movement_Action>().look_at_player = false;
                GetComponent<ChasePlayer_Action>().SetEvent(event_recieved);
                Debug.Log("Player in sight");
                break;
        }
    }

    public override void OnEventGone(PerceptionEvent event_recieved)
    {
        switch (event_recieved.type)
        {
            case PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT:
            case PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER:
                GetComponent<EnemyShield_BT>().heard_something = false;
                GetComponent<Investigate_Action>().forgot_event = true;
                Debug.Log("I lost Somethin");
                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:
                GetComponent<EnemyShield_BT>().player_detected = false;
                GetComponent<EnemyShield_BT>().SetAction(Action.ACTION_TYPE.DISENGAGE_ACTION);
                GetComponent<ChasePlayer_Action>().forgot_event = true;
                GetComponent<Movement_Action>().look_at_player = true;
                Debug.Log("Player out of sight");
                break;
        }
    }

    bool OnHearRange(PerceptionEvent event_heard)
    {
        PerceptionHearEvent tmp = (PerceptionHearEvent)event_heard;

        int my_tile_x = my_self.GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = my_self.GetComponent<Movement_Action>().GetCurrentTileY();

        Debug.Log(my_tile_x.ToString());
        Debug.Log(my_tile_y.ToString());

        if (hear_range < tmp.radius_in_tiles)
        {
            if (RadiusOverlap((int)my_tile_x, (int)my_tile_y, hear_range, (int)tmp.objective_tile_x, (int)tmp.objective_tile_y, (int)tmp.radius_in_tiles))
                return true;

            return false;
        }
        else
        {
            if (RadiusOverlap((int)tmp.objective_tile_x, (int)tmp.objective_tile_y, (int)tmp.radius_in_tiles, (int)my_tile_x, (int)my_tile_y, hear_range))
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

