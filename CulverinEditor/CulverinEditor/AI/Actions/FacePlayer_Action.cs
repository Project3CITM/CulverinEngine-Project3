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
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
       /* if (interupt == true)
        {
            move.Interupt();
        }*/

        return move.ActionUpdate();
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }

    public bool IsFaced()
    {
        forward = new Vector3(GetComponent<Transform>().GetForwardVector());
        Vector3 pos = new Vector3(GetComponent<Transform>().position);
        Vector3 target_pos = new Vector3(GetLinkedObject("player_obj").GetComponent<Transform>().position);
        obj_vec = new Vector3(target_pos - pos);

        if (pos == target_pos)
        {
            return true;
        }

        delta = Vector3.AngleBetweenXZ(forward, obj_vec);

        return Mathf.Abs(delta) < 0.1f;
    }
}
