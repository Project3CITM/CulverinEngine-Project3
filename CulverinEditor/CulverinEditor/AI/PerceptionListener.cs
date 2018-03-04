using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class PerceptionListener : CulverinBehaviour
{

    public GameObject event_manager;
    public List<PerceptionEvent> events_in_memory;

    void Start()
    {
        GetLinkedObject("event_manager").GetComponent<PerceptionManager>().AddListener(this);
    }

    void Update()
    {
        
    }

    public virtual void OnEventRecieved(PerceptionEvent event_recieved) { }

    
}

