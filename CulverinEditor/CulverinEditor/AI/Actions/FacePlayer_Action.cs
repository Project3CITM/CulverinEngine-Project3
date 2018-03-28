using CulverinEditor;
using CulverinEditor.Debug;

public class FacePlayer_Action : Action
{
    float delta = 0.0f;
    float cur_delta = 0.0f;
    Vector3 obj_vec;
    Vector3 forward;
    Movement_Action move;

    public FacePlayer_Action()
    {
        action_type = ACTION_TYPE.FACE_PLAYER_ACTION;
    }

    public override bool ActionStart()
    {

        GetComponent<Align_Steering>().SetEnabled(true);
        move = GetComponent<Movement_Action>();

        forward = new Vector3(GetComponent<Transform>().GetForwardVector());
        Vector3 pos = new Vector3(GetComponent<Transform>().position);
        Vector3 target_pos = new Vector3(GetLinkedObject("player_obj").GetComponent<Transform>().position);
        obj_vec = new Vector3(target_pos - pos);

        if (pos == target_pos)
        {
            Debug.Log("[error] Face player positions are equal!");
            return true;
        }

        delta = Vector3.AngleBetweenXZ(forward, obj_vec);

        if (delta > Mathf.PI)
        {
            delta = delta - 2 * Mathf.PI;
        }
        if (delta < (-Mathf.PI))
        {
            delta = delta + 2 * Mathf.PI;
        }



        Debug.Log(delta);
        GetComponent<Align_Steering>().SetRotation(delta);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        //Update the movement rotation
        move.UpdateRotation();

        Debug.Log("[error]FACE_update");
        Debug.Log(cur_delta);
        forward = GetComponent<Transform>().GetForwardVector();
        cur_delta = Vector3.AngleBetweenXZ(forward, obj_vec);

        if (Mathf.Abs(cur_delta) < 0.1f || move.FinishedRotation())
        {
            GetComponent<Transform>().RotateAroundAxis(Vector3.Up, cur_delta);
            GetComponent<Align_Steering>().SetEnabled(false);
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
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
