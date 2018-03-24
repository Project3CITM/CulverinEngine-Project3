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
    CompAnimation anim;
    Movement_Action move;
    ACTION_RESULT move_return;

    public bool forgot_event = false;

    void Start()
    {
        anim = GetComponent<CompAnimation>();
    }

    public Investigate_Action()
    {
        action_type = ACTION_TYPE.INVESTIGATE_ACTION;
    }

    public Investigate_Action(float speed): base(speed)
    {
        action_type = ACTION_TYPE.INVESTIGATE_ACTION;
    }

    public override bool ActionStart()
    {
        Debug.Log("Going to investigate");

        int current_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int current_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        move = GetComponent<Movement_Action>();
        move.GoTo(event_to_react.objective_tile_x, event_to_react.objective_tile_y);
        bool ret = move.ActionStart();

        init_tile_x = current_tile_x;
        init_tile_y = current_tile_y;
        my_state = INVESTIGATESTATE.GOING_TO_INVESTIGATE;

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

                    if (move.ActionStart() == false)
                        return ACTION_RESULT.AR_FAIL;

                    my_state = INVESTIGATESTATE.RETURNING_TO_START;
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
                    return move_return;
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

