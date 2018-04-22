using CulverinEditor;
using CulverinEditor.Debug;

public class FacePlayer_Action : Action
{
    float delta = 0.0f;
    Vector3 obj_vec;
    Vector3 forward;
    Movement_Action move;

    public FacePlayer_Action()
    {
        action_type = ACTION_TYPE.FACE_PLAYER_ACTION;
    }

    public override bool ActionStart()
    {
        move = GetComponent<Movement_Action>();
        move.LookAtPlayer();
        move.ActionStart();
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (interupt == true)
            move.Interupt();

        ACTION_RESULT move_update = move.ActionUpdate();

        if (move_update != ACTION_RESULT.AR_IN_PROGRESS)
            move.ActionEnd();

        return move.ActionUpdate();
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }

}
