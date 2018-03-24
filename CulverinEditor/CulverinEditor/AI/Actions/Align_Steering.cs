using CulverinEditor;
using CulverinEditor.Debug;


public class Align_Steering : CulverinBehaviour
{
    Movement_Action move;
    float rot_margin = 0.05f;
    float stopping_time = 0.2f;
    float break_acceleration = 0.0f;
    bool in_rot_margin = false;
    float delta = 0.0f;

    void Start()
    {
        move = GetComponent<Movement_Action>();
        Debug.Log("Align Start");
        SetEnabled(false);
    }

    void Update()
    {
        float acceleration = 0;

        if (delta > 0)
            acceleration = move.GetMaxRotAcceleration();
        else
            acceleration = -move.GetMaxRotAcceleration();

        if (!in_rot_margin && Mathf.Abs(delta) <= rot_margin)
        {
            in_rot_margin = true;
            float current_rot_velocity = move.GetCurrentRotVelocity();
            break_acceleration = -current_rot_velocity / stopping_time;
        }

        if (in_rot_margin)
            acceleration = break_acceleration;

        move.Rotate(acceleration);
    }

    public void SetRotation(float delta)
    {
        this.delta = delta;
    }

    public void Reset()
    {
        in_rot_margin = false;
    }
}

