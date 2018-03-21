using CulverinEditor;
using CulverinEditor.Debug;

public enum PERCEPTION_EVENT_TYPE
{
    //Hear events
    HEAR_EXPLORER_EVENT = 0,
    HEAR_WALKING_PLAYER = 1,

    //Visual events
    PLAYER_SEEN = 2,

    //Other events
    PLAYER_DETECTED_EVENT

}

public class PerceptionEvent : CulverinBehaviour
{
    //General variables
    public PERCEPTION_EVENT_TYPE    type;
    public float                    time_in_memory;
    public float                    counter_in_memory;
    public bool                     start_counting;
    public bool                     is_finished;

    //Origin of the event
    public int                      origin_tile_x;
    public int                      origin_tile_y;

    //Objective of the event
    public int                      objective_tile_x = 0;
    public int                      objective_tile_y = 0;

    public PerceptionEvent(PERCEPTION_EVENT_TYPE _type, float _time_in_memory)
    {
        type = _type;
        time_in_memory = _time_in_memory;
        counter_in_memory = 0.0f;
        is_finished = false;
        start_counting = true;
    }

    public PerceptionEvent(PerceptionEvent e)
    {
        type = e.type;
        time_in_memory = e.time_in_memory;
        objective_tile_x = e.objective_tile_x;
        objective_tile_y = e.objective_tile_y;
        origin_tile_y = e.origin_tile_y;
        origin_tile_x = e.origin_tile_x;
        counter_in_memory = 0.0f;
        start_counting = true;
        is_finished = false;
    }

    public void SetOrigin(int x, int y) { origin_tile_x = x; origin_tile_y = y; }

    public void SetDestiny(int x, int y) { objective_tile_x = x; objective_tile_y = y; }

    public virtual bool IsPrioritary(PerceptionEvent new_event) { return true; }
}

public class PerceptionHearEvent : PerceptionEvent
{
    public int radius_in_tiles;

    public PerceptionHearEvent(PERCEPTION_EVENT_TYPE _type, float _time_in_memory, int _radius) : base(_type, _time_in_memory)
    {
        radius_in_tiles = _radius;
    }

    public PerceptionHearEvent(PerceptionHearEvent e) : base(e)
    {
        radius_in_tiles = e.radius_in_tiles;
    }

    public override bool IsPrioritary(PerceptionEvent new_event)
    {
        if (new_event.type >= PERCEPTION_EVENT_TYPE.PLAYER_SEEN)
            return true;

        return false;
    }

}

public class PerceptionPlayerSeenEvent : PerceptionEvent
{
    public int player_seen_in_x;
    public int player_seen_in_y;
    public GameObject enemy_who_saw;

    public PerceptionPlayerSeenEvent(float _time_in_memory, int player_x, int player_y, GameObject enemy_who_saw_) : base(PERCEPTION_EVENT_TYPE.PLAYER_SEEN, _time_in_memory)
    {
        Debug.Log(this.type);

        player_seen_in_x = player_x;
        player_seen_in_y = player_y;
        enemy_who_saw = enemy_who_saw_;
    }

    public PerceptionPlayerSeenEvent(PerceptionPlayerSeenEvent e) : base(e)
    {
        player_seen_in_x = e.player_seen_in_x;
        player_seen_in_y = e.player_seen_in_y;
        enemy_who_saw = e.enemy_who_saw;
    }

    public override bool IsPrioritary(PerceptionEvent new_event)
    {
        if (new_event.type >= PERCEPTION_EVENT_TYPE.PLAYER_SEEN)
            return true;

        return false;
    }

}
