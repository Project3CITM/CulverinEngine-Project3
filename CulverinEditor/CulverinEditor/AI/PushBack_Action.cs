using CulverinEditor.Debug;
using CulverinEditor;

public class PushBack_Action : Action
{
    public GameObject map;
    Movement_Action move;
    PerceptionSightEnemy perceptionsight;
    Pathfinder pathfinder;
    Vector3 push_direction;

    void Start()
    {
        move = GetComponent<Movement_Action>();
        perceptionsight = GetComponent<PerceptionSightEnemy>();
        map = GetLinkedObject("map");
        pathfinder = map.GetComponent<Pathfinder>();
    }

    public PushBack_Action()
    {
        action_type = ACTION_TYPE.PUSHBACK_ACTION;
    }

    public PushBack_Action(Vector3 push_direction_, float speed) : base(speed)
    {
        action_type = ACTION_TYPE.PUSHBACK_ACTION;
        push_direction = push_direction_;
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

