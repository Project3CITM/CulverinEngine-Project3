using CulverinEditor.Debug;
using CulverinEditor;

public class PushBack_Action : Action
{
    Vector3 push_direction;

    int target_x;
    int target_y;

    public float push_speed = 10.0f;

    void Start()
    {
        target_x = 0;
        target_y = 0;
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
        //TODO: Animations GetComponent<CompAnimation>().SetTransition("ToHit");
        GetComponent<Align_Steering>().SetEnabled(false);

        target_x = GetComponent<Movement_Action>().GetCurrentTileX();
        target_y = GetComponent<Movement_Action>().GetCurrentTileY();

        target_x += (int)push_direction.x;
        target_y += (int)push_direction.z;

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
            return ACTION_RESULT.AR_FAIL;

        if (GetComponent<Movement_Action>().ReachedTile(target_x, target_y))
        {
            Debug.Log("Pushed");
           //TODO: Animations  GetComponent<CompAnimation>().SetTransition("ToIdle");
            return ACTION_RESULT.AR_SUCCESS;
        }

        Vector3 my_pos = transform.position;

        Vector3 movement = new Vector3(Vector3.Zero);
        movement.x = (target_x * GetComponent<Movement_Action>().tile_size) - my_pos.x;
        movement.z = (target_y * GetComponent<Movement_Action>().tile_size) - my_pos.z;

        movement = movement.Normalized * 10;

        transform.SetPosition(my_pos + (movement * Time.deltaTime * push_speed));
        
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

