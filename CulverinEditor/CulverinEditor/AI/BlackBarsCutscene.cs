using CulverinEditor;
using CulverinEditor.Debug;

public class BlackBarsCutscene : CulverinBehaviour
{
    public GameObject black_bar_top = null;
    public GameObject black_bar_bot = null;

    private CompImage black_bar_top_img = null;
    private CompImage black_bar_bot_img = null;

    public float black_bar_appearing_time = 1.5f;
    private float black_bar_appearing_timer = 0.0f;

    private enum BLACK_BARS_STATE
    {
        BBS_NONE,
        BBS_APPEARING,
        BBS_DISAPPEARING
    };

    private BLACK_BARS_STATE black_bars_state;

    public void Start()
    {
        black_bar_top = GetLinkedObject("black_bar_top");
        black_bar_bot = GetLinkedObject("black_bar_bot");

        black_bar_top_img = black_bar_top.GetComponent<CompImage>();
        black_bar_bot_img = black_bar_bot.GetComponent<CompImage>();

        black_bar_top_img.SetAlpha(0.0f);
        black_bar_bot_img.SetAlpha(0.0f);

        black_bars_state = BLACK_BARS_STATE.BBS_NONE;
    }

    public void Update()
    {
        float alpha = 0.0f;

        switch (black_bars_state)
        {
            case BLACK_BARS_STATE.BBS_NONE:
                break;
       
            case BLACK_BARS_STATE.BBS_APPEARING:
                black_bar_appearing_timer += Time.deltaTime;

                alpha = black_bar_appearing_timer / black_bar_appearing_time;

                black_bar_top_img.SetAlpha(alpha);
                black_bar_bot_img.SetAlpha(alpha);

                if (alpha >= 1.0f)
                {
                    black_bar_appearing_timer = 0.0f;
                    black_bars_state = BLACK_BARS_STATE.BBS_NONE;
                }                
                break;
       
            case BLACK_BARS_STATE.BBS_DISAPPEARING:
                black_bar_appearing_timer += Time.deltaTime;

                alpha = (black_bar_appearing_timer / black_bar_appearing_time) - 1.0f;
                alpha *= -1.0f;

                black_bar_top_img.SetAlpha(alpha);
                black_bar_bot_img.SetAlpha(alpha);

                if (alpha <= 0.0f)
                    black_bars_state = BLACK_BARS_STATE.BBS_NONE;
                
                break;
       
            default:
                break;
        }
    }

    public void ActivateBlackBar(bool active)
    {
        if (active)        
            black_bars_state = BLACK_BARS_STATE.BBS_APPEARING;
        else
            black_bars_state = BLACK_BARS_STATE.BBS_DISAPPEARING;
    }
}