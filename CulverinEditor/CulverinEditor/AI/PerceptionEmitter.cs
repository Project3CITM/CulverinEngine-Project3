using CulverinEditor;
using CulverinEditor.Debug;

public class PerceptionEmitter : CulverinBehaviour
{
    public GameObject event_manager;

    void Start()
    {
        event_manager = GetLinkedObject("event_manager");
    }

    public void TriggerHearEvent(PERCEPTION_EVENT_TYPE type, float time_in_memory, int radius, int tile_x, int tile_y)
    {
        PerceptionHearEvent new_event = new PerceptionHearEvent(type, time_in_memory, radius);

        new_event.SetOrigin(tile_x, tile_y);
        new_event.SetDestiny(tile_x, tile_y);

        event_manager.GetComponent<PerceptionManager>().GenEvent(new_event);
    }

    public void TriggerHearEvent(PERCEPTION_EVENT_TYPE type, float time_in_memory, int radius, int origin_tile_x, int origin_tile_y, int destiny_x, int destiny_y)
    {
        PerceptionHearEvent new_event = new PerceptionHearEvent(type, time_in_memory, radius);

        new_event.SetOrigin(origin_tile_x, origin_tile_y);
        new_event.SetDestiny(destiny_x, destiny_y);

        event_manager.GetComponent<PerceptionManager>().GenEvent(new_event);
    }

    public void TriggerPlayerSeenEvent(float time_in_memory, int player_x, int player_y, GameObject enemy_who_saw, int enemy_x, int enemy_y)
    {
        PerceptionPlayerSeenEvent new_event = new PerceptionPlayerSeenEvent(time_in_memory, player_x, player_y, enemy_who_saw);

        new_event.SetOrigin(enemy_x, enemy_y);
        new_event.SetDestiny(player_x, player_y);

        event_manager.GetComponent<PerceptionManager>().GenEvent(new_event);
    }

}

