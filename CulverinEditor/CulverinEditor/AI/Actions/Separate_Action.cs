using CulverinEditor;
using CulverinEditor.Debug;

public class Separate_Action : Action
{

    Movement_Action move;
    ACTION_RESULT move_return;

    public bool forgot_event = false;
    public float check_player_timer = 1.0f;
    float timer = 0.0f;

    uint destiny_tile_x=0;
    uint destiny_tile_y = 0;

    void Start()
    {
        move = GetComponent<Movement_Action>();
    }

    public Separate_Action()
    {
        action_type = ACTION_TYPE.SEPARATE_ACTION;
    }

    public override bool ActionStart()
    {
        move.chase = true;
        move.GoTo((int)destiny_tile_x, (int)destiny_tile_y);
        bool ret = move.ActionStart();
        return ret;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
        {
            move.Interupt();
        }

        move_return = move.ActionUpdate();
        if (move_return != ACTION_RESULT.AR_IN_PROGRESS)
        {
            move.ActionEnd();
        }

        return move_return;
    }


    public void SetTileDestinySeparate(uint x, uint y)
    {
        destiny_tile_x = x;
        destiny_tile_y = y;
    }

}