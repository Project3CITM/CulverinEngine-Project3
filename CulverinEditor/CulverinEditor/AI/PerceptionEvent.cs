using CulverinEditor;
using CulverinEditor.Debug;

public enum PERCEPTION_EVENT_TYPE
{
    //Hear events
    HEAR_EXPLORER_EVENT = 0,
    HEAR_WALKING_PLAYER,

    //Visual events
    PLAYER_SEEN,

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
        start_counting = true;
    }

    public void SetOrigin(int x, int y) { origin_tile_x = x; origin_tile_y = y; }

    public void SetDestiny(int x, int y) { objective_tile_x = x; objective_tile_y = y; }

}

public class PerceptionHearEvent : PerceptionEvent
{
   public int radius_in_tiles;

   public PerceptionHearEvent(PERCEPTION_EVENT_TYPE _type, float _time_in_memory, int _radius) : base(_type, _time_in_memory)
    {
        radius_in_tiles = _radius;
    }
}
