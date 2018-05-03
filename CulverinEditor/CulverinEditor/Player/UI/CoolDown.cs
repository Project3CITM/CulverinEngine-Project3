using CulverinEditor;
using CulverinEditor.Debug;

public class CoolDown : CulverinBehaviour
{
    protected CompButton button_cd;
    protected CompImage fill_image;
    protected CompText timer_text;
    protected CharactersManager characters_manager;

    public float cd_time = 2.0f;
    public float act_time = 0.0f;
    protected float calc_time = 0.0f;
    public bool in_cd = false;
    public int seconds = 0;
    protected int prev_seconds = 0;
    protected bool reset_timer = false;

    protected void LinkTextTimer(GameObject text_obj)
    {
        timer_text = text_obj.GetComponent<CompText>();
    }

    public virtual void Start()
    {
        button_cd = GetComponent<CompButton>();
        fill_image = GetComponent<CompImage>();
        characters_manager = GetLinkedObject("player_obj").GetComponent<CharactersManager>();
    }

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
            fill_image.FillAmount(calc_time);

            //Manage Logical CD time
            act_time += Time.deltaTime;

            //Detect when activate the button again
            if (act_time >= cd_time)
            {
                in_cd = false;
                button_cd.Activate();
                fill_image.FillAmount(1.0f);
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

    protected void ManageTextTimer()
    {
        seconds = (int)(cd_time - act_time) + 1;
        if (seconds < prev_seconds)
        {
            prev_seconds = seconds;

            //Set time text
            timer_text.SetText(prev_seconds.ToString());
        }
    }

    protected void ResetTextTimer()
    {
        //Set time text
        timer_text.SetText("");
    }

    public void SetCDTime(float time)
    {
        cd_time = time;
    }
}