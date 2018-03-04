using CulverinEditor;
using CulverinEditor.Debug;


class Align_Steering : CulverinBehaviour
{
    Movement_Action move;
    public GameObject myself;
    float rot_margin = 0.05f;
    float stopping_time = 0.2f;
    float break_acceleration = 0.0f;
    bool in_rot_margin = false;

    void Start()
    {
        move = GetComponent<Movement_Action>();
        myself = GetLinkedObject("myself");
        Debug.Log("Align Start");
        SetScriptEnabled(false);
    }

    void Update()
    {
        float delta = move.GetDeltaAngle();

        float acceleration = 0;

        Debug.Log("Angle: " + delta);

        if (delta > 0)
            acceleration = move.GetMaxRotAcceleration();
        else
            acceleration = -move.GetMaxRotAcceleration();

        Debug.Log("Max Rot Accel: " + move.GetMaxRotAcceleration());
        Debug.Log("Rot Acceleration: " + acceleration.ToString());
        Debug.Log("Margin: " + rot_margin.ToString());

        if (!in_rot_margin && Mathf.Abs(delta) <= rot_margin)
        {
            Debug.Log("Under Margin");
            in_rot_margin = true;
            float current_rot_velocity = move.GetCurrentRotVelocity();
            break_acceleration = -current_rot_velocity / stopping_time;
        }

        if (in_rot_margin)
            acceleration = break_acceleration;

        move.Rotate(acceleration);

        if(move.FinishedRotation())
            in_rot_margin = false;
    }
}

