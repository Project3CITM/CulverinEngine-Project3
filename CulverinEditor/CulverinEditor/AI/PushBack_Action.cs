using CulverinEditor.Debug;
using CulverinEditor;

public class PushBack_Action : Action
{

    Movement_Action move;
    PerceptionSightEnemy perceptionsight;
    GameObject pathfinder_obj;
    Pathfinder pathfinder;

    void Start()
    {
        move = GetComponent<Movement_Action>();
        perceptionsight = GetComponent<PerceptionSightEnemy>();
        pathfinder = GetLinkedObject("pathfinder_obj").GetComponent<Pathfinder>();
    }

    public PushBack_Action()
    {
        action_type = ACTION_TYPE.PUSHBACK_ACTION;
    }

    public PushBack_Action(float speed) : base(speed)
    {
        action_type = ACTION_TYPE.PUSHBACK_ACTION;
    }

    public override bool ActionStart()
    {
        Debug.Log("PUSHBACK!");
        GetComponent<CompAnimation>().SetTransition("ToHit");
        GetComponent<Align_Steering>().SetEnabled(false);
        uint target_x = (uint)move.GetCurrentTileX();
        uint target_y = (uint)move.GetCurrentTileY();

        switch(perceptionsight.GetDirection())
        {
            case PerceptionSightEnemy.DIRECTION.E_DIR_SOUTH:
                target_y -= 1;
                break;
            case PerceptionSightEnemy.DIRECTION.E_DIR_NORTH:
                target_y += 1;
                break;
            case PerceptionSightEnemy.DIRECTION.E_DIR_EAST:
                target_x -= 1;
                break;
            case PerceptionSightEnemy.DIRECTION.E_DIR_WEST:
                target_x += 1;
                break;
        }
        if (!pathfinder.IsWalkableTile(target_x, target_y))
        {
            interupt = true;
        }

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
        {
            return ACTION_RESULT.AR_FAIL;
        }
        if (move.ReachedTile())
        {
            return ACTION_RESULT.AR_IN_PROGRESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

