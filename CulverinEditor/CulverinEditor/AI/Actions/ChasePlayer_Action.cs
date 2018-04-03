using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;

public class ChasePlayer_Action : Action
{
    PerceptionEvent event_to_react;
    Movement_Action move;
    ACTION_RESULT move_return;
    public bool forgot_event = false;
    public float check_player_timer = 1.0f;
    float timer = 0.0f;

    void Start()
    {
        move = GetComponent<Movement_Action>();
    }

    public ChasePlayer_Action()
    {
        action_type = ACTION_TYPE.CHASE_ACTION;
    }

    public override bool ActionStart()
    {
        event_to_react.start_counting = false;
        move.GoToPlayer();
        //Debug.Log("[green] woooow party path");
        interupt = false;
        bool ret = move.ActionStart();
        return ret;
    }

    public override bool ActionEnd()
    {
        Debug.Log("[blue] chase player action end");
        interupt = false;
        move.SetInterupt(false);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (forgot_event == true || GetComponent<Movement_Action>().NextToPlayer() == true || interupt == true )
        {
            Debug.Log("[yellow] Move interruptus " + forgot_event + " " + GetComponent<Movement_Action>().NextToPlayer() + " " + interupt);
            move.Interupt();
        }

        if (forgot_event == false)
        {
            timer += Time.deltaTime;

            if (timer >= check_player_timer && move.CenteredInTile())
            {
                timer = 0.0f;
                move.GoToPlayer();
            }

            if (GetComponent<PerceptionSightEnemy>().player_seen == false)
                event_to_react.start_counting = true;
            else
                event_to_react.start_counting = false;
        }
        ///Make Move update
        move_return = move.ActionUpdate();

        if (move_return != ACTION_RESULT.AR_IN_PROGRESS)
            move.ActionEnd();

        return move_return;
    }

    public void SetEvent(PerceptionEvent e)
    {
        event_to_react = e;
    }

    public void SetInterupt(bool i)
    {
        interupt = i;
    }

}