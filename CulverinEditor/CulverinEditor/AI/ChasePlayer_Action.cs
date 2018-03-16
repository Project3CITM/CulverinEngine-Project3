using CulverinEditor;
using CulverinEditor.Debug;

public class ChasePlayer_Action : Action
{
    int current_tile_x;
    int current_tile_y;

    PerceptionEvent event_to_react;
    Movement_Action move;
    CompAnimation anim;
    ACTION_RESULT move_return;

    public bool forgot_event = false;
    public float check_player_timer = 1.0f;
    float timer = 0.0f;

    void Start()
    {
        move = GetComponent<Movement_Action>();
        anim = GetComponent<CompAnimation>();
    }

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
        bool ret = move.ActionStart();
        Debug.Log("Chasing Player");

        anim = GetComponent<CompAnimation>();
        anim.SetTransition("ToChase");

        event_to_react.start_counting = false;

        move.GoToPrevious(event_to_react.objective_tile_x, event_to_react.objective_tile_y, true);

        return ret;
    }

    public override bool ActionEnd()
    {
        move.NotChase();
        base.ActionEnd();
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt || forgot_event)
        {
            move.Interupt();
        }

        timer += Time.deltaTime;

        if (timer >= check_player_timer)
        {
            timer = 0.0f;
            GetComponent<PerceptionSightEnemy>().GetPlayerTilePos(out int player_x, out int player_y);
            move.GoToPrevious(player_x, player_y);
        }

        if (GetComponent<PerceptionSightEnemy>().player_seen == false)
            event_to_react.start_counting = true;
        else
            event_to_react.start_counting = false;

        ///Make Move update
        move.chase = true;
        move_return = move.ActionUpdate();

        if (move_return == ACTION_RESULT.AR_SUCCESS)
        {
            GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
            move.ActionEnd();
        }

        if (move_return == ACTION_RESULT.AR_FAIL)
        {
            move.ActionEnd();
            GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
            return ACTION_RESULT.AR_FAIL;
        }        

        return move_return;
    }

    public void SetEvent(PerceptionEvent e)
    {
        event_to_react = e;
    }
}