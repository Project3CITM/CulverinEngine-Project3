using CulverinEditor;
using CulverinEditor.Debug;

public class ComboController : CulverinBehaviour
{
    public GameObject this_combo_2_text_obj;
    public GameObject this_combo_3_text_obj;
    public GameObject this_combo_1_text_obj;
    public GameObject this_combo_bar_obj;
    CompImage combo_bar;

    public float max_time = 2.0f;
    float curr_time = 1.0f;
    float calc_time = 0.0f;

    public int hit_streak = 0;

    public bool hit_in_time = false;

    void Start()
    {
        hit_streak = 0;

        this_combo_bar_obj = GetLinkedObject("this_combo_bar_obj");
        this_combo_2_text_obj = GetLinkedObject("this_combo_2_text_obj");
        this_combo_3_text_obj = GetLinkedObject("this_combo_3_text_obj");
        this_combo_1_text_obj = GetLinkedObject("this_combo_1_text_obj");

        curr_time = max_time;

        //Disable images
        this_combo_2_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_2_text_obj);
        this_combo_3_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_3_text_obj);
        this_combo_bar_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_bar_obj);
        this_combo_1_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_1_text_obj);
    }

    void Update()
    {
        if (curr_time > 0) 
        {
            curr_time -= Time.deltaTime;

            if(curr_time <= 0)
            {
                //When time is over, hit streak returns to 0
                ResetHitStreak();
            }

            //Update Combo Bar image
            FillComboBar(curr_time);
        }
    }

    public void StartComboTime()
    {
        curr_time = max_time;

        this_combo_2_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_2_text_obj);
        this_combo_3_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_3_text_obj);
        this_combo_bar_obj.GetComponent<CompImage>().SetEnabled(true, this_combo_bar_obj);
        this_combo_1_text_obj.GetComponent<CompImage>().SetEnabled(true, this_combo_1_text_obj);
    }

    public void ResetHitStreak()
    {
        hit_streak = 0;
        curr_time = 0;

        //Disable images
        this_combo_2_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_2_text_obj);
        this_combo_3_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_3_text_obj);
        this_combo_bar_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_bar_obj);
        this_combo_1_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_1_text_obj);

        Debug.Log("[green] RESET HIT STREAK");
    }

    public int GetHitStreak()
    {
        Debug.Log("[green] --------------------- " + hit_streak + " ---------------------");
        if(hit_streak >= 2)
        {
            return 2;
        }
        return hit_streak;
    }

    public void IncreaseHitStreak()
    {
        if (hit_streak < 3)
        {
            Debug.Log("[orange] ----------- INCREASE HIT STREAK ------------");

            //Increase hit count
            hit_streak++;

            if (hit_streak == 1)
            {
                this_combo_2_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_2_text_obj);
                this_combo_3_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_3_text_obj);
                this_combo_1_text_obj.GetComponent<CompImage>().SetEnabled(true, this_combo_1_text_obj);
            }
            else if (hit_streak == 2)
            {
                this_combo_2_text_obj.GetComponent<CompImage>().SetEnabled(true, this_combo_2_text_obj);
                this_combo_3_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_3_text_obj);
                this_combo_1_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_1_text_obj);
            }
            else if (hit_streak == 3)
            {
                this_combo_2_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_2_text_obj);
                this_combo_3_text_obj.GetComponent<CompImage>().SetEnabled(true, this_combo_3_text_obj);
                this_combo_1_text_obj.GetComponent<CompImage>().SetEnabled(false, this_combo_1_text_obj);
            }

            curr_time = max_time;

            //Refill time bar
            FillComboBar(curr_time);

            Debug.Log("[orange] ----------- " + hit_streak + " -----------");

        }
        else
        {
            //ResetHitCount
            ResetHitStreak();
        }
    }

    void FillComboBar(float time)
    {
        calc_time = time / max_time;
        combo_bar = this_combo_bar_obj.GetComponent<CompImage>();
        combo_bar.FillAmount(calc_time);
    }
}