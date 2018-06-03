using CulverinEditor;
using CulverinEditor.Debug;

public class BlackBarsCutscene : CulverinBehaviour
{
    public GameObject black_bar_top = null;
    public GameObject black_bar_bot = null;

    private CompRectTransform black_bar_top_rect_transform = null;
    private CompRectTransform black_bar_bot_rect_transform = null;

    public float top_bar_initial_pos_y = 610.0f;
    public float top_bar_final_pos_y = 500.0f;

    public float black_bar_appearing_time = 1.5f;
    private float black_bar_appearing_timer = 0.0f;

    private float speed = 0.0f;
    private Vector3 new_pos_vec = Vector3.Zero;

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

        black_bar_top_rect_transform = black_bar_top.GetComponent<CompRectTransform>();
        black_bar_bot_rect_transform = black_bar_bot.GetComponent<CompRectTransform>();

        black_bar_top.GetComponent<CompImage>().SetEnabled(false);
        black_bar_bot.GetComponent<CompImage>().SetEnabled(false);

        black_bars_state = BLACK_BARS_STATE.BBS_NONE;

        speed = Mathf.Abs(top_bar_initial_pos_y - top_bar_final_pos_y) / black_bar_appearing_time;
        new_pos_vec = Vector3.Zero;
    }

    public void Update()
    {
        switch (black_bars_state)
        {
            case BLACK_BARS_STATE.BBS_NONE:
                break;

            case BLACK_BARS_STATE.BBS_APPEARING:
                black_bar_appearing_timer += Time.deltaTime;

                // Top bar
                new_pos_vec.y = top_bar_initial_pos_y - speed * black_bar_appearing_timer;
                black_bar_top_rect_transform.SetUIPosition(new_pos_vec);

                // Bot bar
                new_pos_vec.y *= -1.0f;
                black_bar_bot_rect_transform.SetUIPosition(new_pos_vec);

                if (black_bar_appearing_timer >= black_bar_appearing_time)
                {
                    // Top bar
                    new_pos_vec.y = top_bar_final_pos_y;
                    black_bar_top_rect_transform.SetUIPosition(new_pos_vec);

                    // Bot bar
                    new_pos_vec.y *= -1.0f;
                    black_bar_bot_rect_transform.SetUIPosition(new_pos_vec);

                    black_bar_appearing_timer = 0.0f;
                    black_bars_state = BLACK_BARS_STATE.BBS_NONE;
                }
                break;

            case BLACK_BARS_STATE.BBS_DISAPPEARING:
                black_bar_appearing_timer += Time.deltaTime;

                // Top bar
                new_pos_vec.y = top_bar_final_pos_y + speed * black_bar_appearing_timer;
                black_bar_top_rect_transform.SetUIPosition(new_pos_vec);

                // Bot bar
                new_pos_vec.y *= -1.0f;
                black_bar_bot_rect_transform.SetUIPosition(new_pos_vec);

                if (black_bar_appearing_timer >= black_bar_appearing_time)
                {
                    // Top bar
                    new_pos_vec.y = top_bar_initial_pos_y;
                    black_bar_top_rect_transform.SetUIPosition(new_pos_vec);

                    // Bot bar
                    new_pos_vec.y *= -1.0f;
                    black_bar_bot_rect_transform.SetUIPosition(new_pos_vec);

                    black_bar_top.GetComponent<CompImage>().SetEnabled(false);
                    black_bar_bot.GetComponent<CompImage>().SetEnabled(false);

                    black_bars_state = BLACK_BARS_STATE.BBS_NONE;
                }
                break;

            default:
                break;
        }
    }

    public void ActivateBlackBar(bool active)
    {
        if (active)
        {
            black_bar_top.GetComponent<CompImage>().SetEnabled(active);
            black_bar_bot.GetComponent<CompImage>().SetEnabled(active);
            black_bars_state = BLACK_BARS_STATE.BBS_APPEARING;
        }
        else
            black_bars_state = BLACK_BARS_STATE.BBS_DISAPPEARING;
    }
}