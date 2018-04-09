using CulverinEditor;
using CulverinEditor.Debug;

public class Investigate_Action : Action
{
    public enum INVESTIGATESTATE
    {
        NO_STATE,
        GOING_TO_INVESTIGATE,
        INVESTIGATE,
        RETURNING_TO_START
    }

    int init_tile_x;
    int init_tile_y;

    INVESTIGATESTATE my_state;
    PerceptionEvent event_to_react;
    Movement_Action move;
    ACTION_RESULT move_return;

    public bool forgot_event = false;

    void Start()
    {}

    public Investigate_Action()
    {
        action_type = ACTION_TYPE.INVESTIGATE_ACTION;
    }

    public override bool ActionStart()
    {
        int current_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int current_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        move = GetComponent<Movement_Action>();
        move.GoTo(event_to_react.objective_tile_x, event_to_react.objective_tile_y);
        bool ret = move.ActionStart();

        init_tile_x = current_tile_x;
        init_tile_y = current_tile_y;
        my_state = INVESTIGATESTATE.GOING_TO_INVESTIGATE;

        //TODO_AI:hear something audio
        //GetComponent<CompAudio>().PlayEvent("Dracarys");

        return ret;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        switch (my_state)
        {
            case INVESTIGATESTATE.INVESTIGATE:
                //Trigger investigate animation

                if (interupt)
                {
                    interupt = false;
                    return ACTION_RESULT.AR_FAIL;
                }

                event_to_react.start_counting = true;

                if (forgot_event == true)
                {
                    move.GoTo(init_tile_x, init_tile_y);

                    //Lost something audio
                    //TODO_AI: Ivestigation audio
                    //GetComponent<CompAudio>().PlayEvent("Dracarys");

                    my_state = INVESTIGATESTATE.RETURNING_TO_START;

                    if (move.ActionStart() == false)
                        return ACTION_RESULT.AR_FAIL;
                }

                return ACTION_RESULT.AR_IN_PROGRESS;

            case INVESTIGATESTATE.GOING_TO_INVESTIGATE:
            case INVESTIGATESTATE.RETURNING_TO_START:

                if (interupt)
                {
                    interupt = false;
                    move.Interupt();
                }

                move_return = move.ActionUpdate();

                if (move_return != ACTION_RESULT.AR_IN_PROGRESS)
                {
                    move.ActionEnd();

                    if (move_return == ACTION_RESULT.AR_FAIL)
                    {
                        return move_return;
                    }
                    else
                    {
                        if (my_state == INVESTIGATESTATE.GOING_TO_INVESTIGATE)
                            my_state = INVESTIGATESTATE.INVESTIGATE;
                        else
                            return move_return;
                    }
                }

                break;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }

    public void SetEvent(PerceptionEvent e)
    {
        event_to_react = e;
    }
}

