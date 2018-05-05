using CulverinEditor;
using CulverinEditor.Debug;

public class BossHPBar : CulverinBehaviour
{
    public GameObject hp_bar_boss_background = null;
    public GameObject hp_bar_boss_image = null;
    public GameObject hp_bar_boss_text = null;

    public GameObject player_icons = null;
    private CompRectTransform player_icons_transform = null;
    private Vector3 icons_position = Vector3.Zero;
    public GameObject hp_bar_parent = null;
    private CompRectTransform hp_bar_parent_transform = null;
    private Vector3 hp_bar_parent_position = Vector3.Zero;
    public float icons_initial_y = 350.0f;
    public float icons_end_y = 305.0f;
    public float hp_bar_initial_y = 522.0f;
    public float hp_bar_end_y = 477.0f;

    public float hp_bar_appearing_time = 2.0f;
    private float hp_bar_appearing_timer = 0.0f;
    private bool appeared = false;

    private CompImage hp_bar_img;
    private CompImage hp_bar_img_background;

    public void Start()
    {
        hp_bar_boss_background = GetLinkedObject("hp_bar_boss_background");
        hp_bar_boss_image = GetLinkedObject("hp_bar_boss_image");
        hp_bar_boss_text = GetLinkedObject("hp_bar_boss_text");

        player_icons = GetLinkedObject("player_icons");
        player_icons_transform = player_icons.GetComponent<CompRectTransform>();
        icons_position = player_icons_transform.GetUIPosition();

        hp_bar_parent = GetLinkedObject("hp_bar_parent");
        hp_bar_parent_transform = hp_bar_parent.GetComponent<CompRectTransform>();
        hp_bar_parent_position = hp_bar_parent_transform.GetUIPosition();

        hp_bar_img = hp_bar_boss_background.GetComponent<CompImage>();
        hp_bar_img_background = GetComponent<CompImage>();

        hp_bar_img.SetAlpha(0.0f);
        hp_bar_img_background.SetAlpha(0.0f);
        hp_bar_appearing_timer = 0.0f;

        icons_position.y = icons_initial_y;
        player_icons_transform.SetUIPosition(icons_position);
        hp_bar_parent_position.y = hp_bar_initial_y;
        hp_bar_parent_transform.SetUIPosition(hp_bar_parent_position);

        appeared = false;
    }

    public void Update()
    {
        if (!appeared)
        {
            hp_bar_appearing_timer += Time.deltaTime;
            hp_bar_img.SetAlpha(hp_bar_appearing_timer / hp_bar_appearing_time);
            hp_bar_img_background.SetAlpha(hp_bar_appearing_timer / hp_bar_appearing_time);

            float spd = (icons_end_y - icons_initial_y) / hp_bar_appearing_time;
            icons_position.y = icons_initial_y + spd * hp_bar_appearing_timer;
            player_icons_transform.SetUIPosition(icons_position);
            hp_bar_parent_position.y = hp_bar_initial_y + spd * hp_bar_appearing_timer;
            hp_bar_parent_transform.SetUIPosition(hp_bar_parent_position);

            if (hp_bar_appearing_timer >= hp_bar_appearing_time)
            {
                icons_position.y = icons_end_y;
                player_icons_transform.SetUIPosition(icons_position);
                hp_bar_img.SetAlpha(1.0f);
                hp_bar_img_background.SetAlpha(1.0f);
                appeared = true;
            }
        }
    }

    public void ActivateHPBar(bool active)
    {
        hp_bar_boss_background.SetActive(active);
        hp_bar_boss_image.SetActive(active);
        hp_bar_boss_text.SetActive(active);
    }

    public void SetHPBar(float hp_percentage)
    {
        CompImage hp_bar = gameObject.GetComponent<CompImage>();
        hp_bar.FillAmount(hp_percentage);
    }
}