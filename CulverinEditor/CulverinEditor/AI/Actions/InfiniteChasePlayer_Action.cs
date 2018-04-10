using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;

public class InfiniteChasePlayer_Action : Action
{

    Movement_Action move;

    public float check_player_timer = 1.0f;
    float timer = 0.0f;

    void Start()
    {
        move = GetComponent<Movement_Action>();
    }

    public InfiniteChasePlayer_Action()
    {
        action_type = ACTION_TYPE.INFINITE_CHASE_ACTION;
    }

    public override bool ActionStart()
    {
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
       
        if (GetComponent<Movement_Action>().NextToPlayer() == true || interupt == true)
        {
            move.Interupt();
        }


        timer += Time.deltaTime;

        if (timer >= check_player_timer && move.CenteredInTile())
        {
            timer = 0.0f;
            move.GoToPlayer();
        }

        ///Make Move update
        ACTION_RESULT move_return = move.ActionUpdate();

        if (move_return != ACTION_RESULT.AR_IN_PROGRESS)
            move.ActionEnd();

        return move_return;
    }
}