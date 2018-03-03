using CulverinEditor;
using CulverinEditor.Debug;


class Align_Steering : CulverinBehaviour
{
    Movement_Action move;
    public GameObject myself;
    float stopping_margin = 0.5f;
    float stopping_time = 1.0f;
    float break_acceleration = 0.0f;
    bool in_rot_margin;

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

        if (delta > 0)
            acceleration = move.GetMaxRotAcceleration();
        else
            acceleration = -move.GetMaxRotAcceleration();

        if (!in_rot_margin && delta <= stopping_margin )
        {
            in_rot_margin = true;
            float current_rot_velocity = move.GetCurrentRotVelocity();
            break_acceleration = -current_rot_velocity / stopping_time;
        }

        if (in_rot_margin)
            acceleration = break_acceleration;

        Debug.Log("Rot Acceleration: " + acceleration.ToString());

        move.Rotate(acceleration);
    }
}

