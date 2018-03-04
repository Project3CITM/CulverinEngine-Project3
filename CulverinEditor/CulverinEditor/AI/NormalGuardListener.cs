using CulverinEditor;
using CulverinEditor.Debug;

public class NormalGuardListener : PerceptionListener
{
    
    void Start()
    {

    }

    void Update()
    {
        
    }

    public override void OnEventRecieved(PerceptionEvent event_recieved)
    {
       switch(event_recieved.type)
        {
            case PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT:

                if(OnPerceptionRange(event_recieved))
                {
                        //do something
                }

                break;

            case PERCEPTION_EVENT_TYPE.HEAR_ALERT_EVENT:

                break;

            case PERCEPTION_EVENT_TYPE.VISUAL_EVENT:

                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_DETECTED_EVENT:

                break;
        }
    }

    bool OnPerceptionRange(PerceptionEvent event_heard)
    {
        PerceptionHearEvent tmp = (PerceptionHearEvent)event_heard;

        int my_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int my_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        if ((my_tile_x >= (tmp.objective_tile_x - tmp.radius_in_tiles)) && (my_tile_y >= (tmp.objective_tile_y - tmp.radius_in_tiles)) && (my_tile_x <= (tmp.objective_tile_x + tmp.radius_in_tiles)) && (my_tile_y <= (tmp.objective_tile_y + tmp.radius_in_tiles)))
            return true;

        return false;
    }

}

