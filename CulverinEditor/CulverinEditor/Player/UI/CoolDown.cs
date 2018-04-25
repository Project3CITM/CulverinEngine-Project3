using CulverinEditor;
using CulverinEditor.Debug;

public class CoolDown : CulverinBehaviour
{
    public CompButton button_cd;

    public float cd_time = 2.0f;
    public float act_time = 0.0f;
    protected float calc_time = 0.0f;
    public bool in_cd = false;
    public int seconds = 0;
    protected int prev_seconds = 0;
    protected bool reset_timer = false;

    public virtual void Update()
    {
        if (in_cd)
        {
            //Manage the Radial Fill Cooldown
            float final_time = cd_time - act_time;
            if (final_time <= 0.0f)
            {
                final_time = 0.0f;
            }

            calc_time = final_time / cd_time;
            GetComponent<CompImage>().FillAmount(calc_time);

            //Manage Logical CD time
            act_time += Time.deltaTime;

            //Detect when activate the button again
            if (act_time >= cd_time)
            {
                in_cd = false;
                button_cd = GetComponent<CompButton>();
                button_cd.Activate();
                Debug.Log("ACTIVATE ", Department.PLAYER);
                GetComponent<CompImage>().FillAmount(1.0f);
                reset_timer = true;
            }
        }
    }

    public virtual void OnClick()
    {
    }

    public virtual void ActivateAbility()
    {
    }

    protected void ManageTextTimer(GameObject timer_obj)
    {
        seconds = (int)(cd_time - act_time) + 1;
        if (seconds < prev_seconds)
        {
            prev_seconds = seconds;

            //Set time text
            timer_obj.GetComponent<CompText>().SetText(prev_seconds.ToString());
        }
    }

    protected void ResetTextTimer(GameObject timer_obj)
    {
        //Set time text
        timer_obj.GetComponent<CompText>().SetText("");
    }
}