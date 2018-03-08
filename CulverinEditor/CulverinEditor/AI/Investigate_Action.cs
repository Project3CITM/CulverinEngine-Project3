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
    ACTION_RESULT move_return;

    public bool forgot_event = false;

    public Investigate_Action() { action_type = ACTION_TYPE.INVESTIGATE_ACTION; }

    public override bool ActionStart()
    {
        int current_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int current_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        event_to_react = GetComponent<NormalGuardListener>().GetEvent();

        GetComponent<Movement_Action>().GoTo(current_tile_x, current_tile_y, event_to_react.objective_tile_x, event_to_react.objective_tile_y);

        init_tile_x = current_tile_x;
        init_tile_y = current_tile_y;
        my_state = INVESTIGATESTATE.GOING_TO_INVESTIGATE;

        return GetComponent<Movement_Action>().ActionStart();
    }

    public override ACTION_RESULT ActionUpdate()
    {
        switch(my_state)
        {
            case INVESTIGATESTATE.GOING_TO_INVESTIGATE:
                
                if(interupt)
                {
                    GetComponent<Movement_Action>().Interupt();
                }

               

                ///Make Move update
                move_return = GetComponent<Movement_Action>().ActionUpdate();

                if (move_return == ACTION_RESULT.AR_SUCCESS)
                {
                    my_state = INVESTIGATESTATE.INVESTIGATE;
                    Debug.Log("Investigate");
                }

                if (move_return == ACTION_RESULT.AR_FAIL)
                {
                    GetComponent<Movement_Action>().ActionEnd();
                    return ACTION_RESULT.AR_FAIL;
                }

            break;

            case INVESTIGATESTATE.INVESTIGATE:

                //Trigger investigate animation

                event_to_react.start_counting = true;

                if(forgot_event == true)
                {
                    int current_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
                    int current_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

                    GetComponent<Movement_Action>().GoTo(current_tile_x, current_tile_y, init_tile_x, init_tile_y);

                    if (!GetComponent<Movement_Action>().ActionStart())
                        return ACTION_RESULT.AR_FAIL;

                    my_state = INVESTIGATESTATE.RETURNING_TO_START;
                }

                if (interupt)
                {
                    return ACTION_RESULT.AR_FAIL;
                }

                break;

            case INVESTIGATESTATE.RETURNING_TO_START:

                if (interupt)
                {
                    GetComponent<Movement_Action>().Interupt();
                }

                move_return = GetComponent<Movement_Action>().ActionUpdate();

                if (move_return == ACTION_RESULT.AR_SUCCESS)
                {
                    Debug.Log("Arrived home");
                    return ACTION_RESULT.AR_SUCCESS;
                }

                if (move_return == ACTION_RESULT.AR_FAIL)
                {
                    GetComponent<Movement_Action>().ActionEnd();
                    return ACTION_RESULT.AR_FAIL;
                }

                break;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

}

