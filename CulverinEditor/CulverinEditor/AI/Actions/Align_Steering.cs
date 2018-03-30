using CulverinEditor;
using CulverinEditor.Debug;


public class Align_Steering : CulverinBehaviour
{
    Movement_Action move;
    float rot_margin = 0.05f;
    float slow_angle = 0.6f;
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

        if (Mathf.Abs(delta) <= rot_margin)
        {
            float ideal_velocity = delta / slow_angle;
            acceleration = ideal_velocity - move.GetCurrentRotVelocity();
        }

        move.Rotate(Mathf.Rad2deg(acceleration));

        Debug.Log("Delta: " + delta);
    }

    public void SetRotation(float delta)
    {
        this.delta = delta;
    }

    public float GetDeltaAngle()
    {
        return delta;
    }

    public bool RotationFinished()
    {
        return Mathf.Abs(delta) <= rot_margin;
    }

    public void UpdateRotation(float rotation)
    {
        GetComponent<Transform>().RotateAroundAxis(Vector3.Up, rotation);
        delta -= Mathf.Deg2rad(rotation);
    }

    public float GetRotMargin()
    {
        return rot_margin;
    }
}

