using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class PerceptionListener : CulverinBehaviour
{

    public GameObject event_manager;
    public GameObject my_self;
    public List<PerceptionEvent> events_in_memory;

   void Start()
    {
       
    }

    void Update()
    {
        
    }

    public virtual void OnEventRecieved(PerceptionEvent event_recieved) { }

    public virtual void OnEventGone(PerceptionEvent event_recieved) { }

    public void UpdateMemory()
    {
        foreach (PerceptionEvent memory_event in events_in_memory)
        {
            if (memory_event.start_counting)
                UpdateEvent(memory_event);
        }
    }

    public void ClearEvents()
    {
        foreach (PerceptionEvent memory_event in events_in_memory)
        {
            OnEventGone(memory_event);
        }

        events_in_memory.Clear();
    }

    public PerceptionEvent GetEvent(PERCEPTION_EVENT_TYPE type)
    {
        foreach (PerceptionEvent memory_event in events_in_memory)
        {
            if (memory_event.type == type)
                return memory_event;
        }

        return null;
    }

    public PerceptionEvent GetEvent()
    {
        Debug.Log("Getting event");

        if (events_in_memory.Count > 0)
            return events_in_memory[0];
        else return null;
    }

    void UpdateEvent(PerceptionEvent event_to_update)
    {
        if (event_to_update.counter_in_memory >= event_to_update.time_in_memory)
        {
            OnEventGone(event_to_update);
        }
        else
        {
            event_to_update.counter_in_memory = event_to_update.counter_in_memory + Time.DeltaTime();
        }
    }

    public bool IsPriotitaryEvent(PerceptionEvent new_event)
    {
        if (events_in_memory.Count > 0)
        {
            if (new_event.type >= events_in_memory[0].type)
            {
                return true;
            }

            return false;
        }

        return true;
    }

}

