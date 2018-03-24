using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;
using System.Collections;

public class PerceptionManager : CulverinBehaviour
{
    //Listeners
    List<PerceptionListener>    listeners_list;
    List<PerceptionEvent>       perception_events_queue;

    PerceptionManager()
    { }

    // Use this for initialization
    void Start()
    {
        if(listeners_list == null)
            listeners_list = new List<PerceptionListener>();

        perception_events_queue = new List<PerceptionEvent>();
    }

    void Update()
    {
        if (!AnyEvent())
            return;

        foreach(PerceptionEvent perception_event in perception_events_queue)
        {
            SendEventtoListeners(perception_event);
        }

        //Clean frame events
        perception_events_queue.Clear();        
    }

    public void AddListener(PerceptionListener new_listener)
    {
        if(listeners_list == null)
        {
            listeners_list = new List<PerceptionListener>();
        }

        listeners_list.Add(new_listener);

    }

    public void GenEvent(PerceptionEvent new_event)
    {
        perception_events_queue.Add(new_event);
    }

    bool AnyEvent()
    {
        return (perception_events_queue.Count > 0);
    }

    void SendEventtoListeners(PerceptionEvent event_send)
    {
        foreach(PerceptionListener listener in listeners_list)
        {
            listener.OnEventRecieved(event_send);
        }
    }
}