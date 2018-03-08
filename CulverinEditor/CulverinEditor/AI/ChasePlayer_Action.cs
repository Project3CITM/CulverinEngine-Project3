using CulverinEditor;
using CulverinEditor.Debug;

public class ChasePlayer_Action : Action
{
    int init_tile_x;
    int init_tile_y;

    PerceptionEvent event_to_react;
    ACTION_RESULT move_return;

    public bool forgot_event = false;

    ChasePlayer_Action()
    {
        action_type = ACTION_TYPE.CHASE_ACTION;
    }

    public override bool ActionStart()
    {
        Debug.Log("Chasing Player");
        init_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        init_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        event_to_react = GetComponent<NormalGuardListener>().GetEvent();

        GetComponent<Movement_Action>().GoTo(init_tile_x, init_tile_y, event_to_react.objective_tile_x, event_to_react.objective_tile_y);

        return GetComponent<Movement_Action>().ActionStart();
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt)
        {
            GetComponent<Movement_Action>().Interupt();
        }

        ///Make Move update
        move_return = GetComponent<Movement_Action>().ActionUpdate();

        if (move_return == ACTION_RESULT.AR_SUCCESS)
        {
            Debug.Log("Chased");
        }

        if (move_return == ACTION_RESULT.AR_FAIL)
        {
            GetComponent<Movement_Action>().ActionEnd();
            return ACTION_RESULT.AR_FAIL;
        }        

        return move_return;
    }

}