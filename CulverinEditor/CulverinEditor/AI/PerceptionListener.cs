using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class PerceptionListener : CulverinBehaviour
{
    public GameObject event_manager;
    public List<PerceptionEvent> events_in_memory;
    List<PerceptionEvent> to_remove = new List<PerceptionEvent>();

    void Start()
    {
        events_in_memory = new List<PerceptionEvent>();
    }

    void Update()
    { }

    public virtual void OnEventRecieved(PerceptionEvent event_recieved)
    { }

    public virtual void OnEventGone(PerceptionEvent event_recieved)
    { }

    public void UpdateMemory()
    {
        if (events_in_memory.Count == 0)
            return;

            
        foreach (PerceptionEvent e in events_in_memory)
        {
            if (e.start_counting)
                UpdateEvent(e);

            if (e.is_finished)
                to_remove.Add(e);
        }

        foreach (PerceptionEvent e in to_remove)
        {
            Debug.Log("Removed Element");
            events_in_memory.Remove(e);
        }
        to_remove.Clear();


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
        foreach (PerceptionEvent pe in events_in_memory)
            Debug.Log("Event" + pe.objective_tile_x + "," + pe.objective_tile_y);
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
            event_to_update.is_finished = true;
        }
        else
        {
            event_to_update.counter_in_memory = event_to_update.counter_in_memory + Time.deltaTime;
        }
    }

    public bool IsPriotitaryEvent(PerceptionEvent new_event)
    {
        if (events_in_memory.Count > 0)
        {
            return events_in_memory[0].IsPrioritary(new_event);           
        }

        return true;
    }

}

