using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class Boss_Listener : PerceptionListener
{
    public int hear_range = 2;
    bool player_seen = false;

    void Start()
    {

        event_manager = GetLinkedObject("event_manager");
        if (event_manager == null) Debug.Log("[error]EVENT MANAGER NULL");

        PerceptionManager perception_manager = event_manager.GetComponent<PerceptionManager>();
        if (perception_manager != null)
        {
            perception_manager.AddListener(this);
        }
        else
        {
            Debug.Log("THERE IS NO PERCEPTION MANAGER");
        }
        events_in_memory = new List<PerceptionEvent>();

    }

    void Update()
    {
        UpdateMemory();
    }

    public override void OnEventRecieved(PerceptionEvent event_recieved)
    {

        if (IsPriotitaryEvent(event_recieved) == true)
        {
            ClearEvents();
        }
        else
        {
            return;
        }

        switch (event_recieved.type)
        {
            case PERCEPTION_EVENT_TYPE.HEAR_EXPLORER_EVENT:
            case PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER:
         
                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:

                PerceptionPlayerSeenEvent seen_event_tmp = (PerceptionPlayerSeenEvent)event_recieved;


                if (gameObject.IsEquals(seen_event_tmp.enemy_who_saw))
                {


                    GetComponent<Boss_BT>().InterruptAction();
                    GetComponent<Boss_BT>().player_detected = true;

                    if (GetComponent<Boss_BT>().InCombat() == false)
                    {
                        GetComponent<Boss_BT>().SetAction(Action.ACTION_TYPE.ENGAGE_ACTION);
                    }

                    GetComponent<ChasePlayer_Action>().SetInterupt(false);

                    player_seen = true;

                    PerceptionPlayerSeenEvent new_event_to_memory = new PerceptionPlayerSeenEvent(seen_event_tmp);
                    events_in_memory.Add(new_event_to_memory);
                    GetComponent<ChasePlayer_Action>().SetEvent(new_event_to_memory);
                    GetComponent<ChasePlayer_Action>().forgot_event = false;
                    new_event_to_memory.start_counting = false;
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

                break;

            case PERCEPTION_EVENT_TYPE.PLAYER_SEEN:
                /*GetComponent<Boss_BT>().player_detected = false;
                GetComponent<ChasePlayer_Action>().forgot_event = true;

                if (GetComponent<Boss_BT>().InCombat() == true && GetComponent<PerceptionSightEnemy>().player_seen == false)
                    GetComponent<Boss_BT>().SetAction(Action.ACTION_TYPE.DISENGAGE_ACTION);*/


                break;
        }
    }
}

