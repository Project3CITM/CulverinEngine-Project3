using CulverinEditor;
using CulverinEditor.Debug;

public class ChasePlayer_Action : Action
{
    int current_tile_x;
    int current_tile_y;

    PerceptionEvent event_to_react;
    ACTION_RESULT move_return;

    public bool forgot_event = false;
    public float check_player_timer = 1.0f;
    float timer = 0.0f;

    public ChasePlayer_Action()
    {
        action_type = ACTION_TYPE.CHASE_ACTION;
    }

    public ChasePlayer_Action(float speed):base(speed)
    {
        action_type = ACTION_TYPE.CHASE_ACTION;
    }

    public override bool ActionStart()
    {
        Debug.Log("Chasing Player");
        current_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
        current_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

        event_to_react.start_counting = false;

        GetComponent<Movement_Action>().GoToPrevious(current_tile_x, current_tile_y, event_to_react.objective_tile_x, event_to_react.objective_tile_y);

        return GetComponent<Movement_Action>().ActionStart();
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt || forgot_event)
        {
            GetComponent<Movement_Action>().Interupt();
        }

        timer += Time.DeltaTime();

        if (timer >= check_player_timer)
        {
            timer = 0.0f;

            current_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
            current_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

            int player_x, player_y;
            GetComponent<PerceptionSightEnemy>().GetPlayerTilePos(out player_x, out player_y);
            GetComponent<Movement_Action>().GoToPrevious(current_tile_x, current_tile_y, player_x, player_y);
        }

        if (GetComponent<PerceptionSightEnemy>().player_seen == false)
            event_to_react.start_counting = true;
        else
            event_to_react.start_counting = false;

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

    public void SetEvent(PerceptionEvent e)
    {
        event_to_react = new PerceptionEvent(e);
    }
}