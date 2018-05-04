using CulverinEditor;
using CulverinEditor.Debug;

public class BossHPBar : CulverinBehaviour
{
    public GameObject hp_bar_boss_background = null;
    public GameObject hp_bar_boss_image = null;
    public GameObject hp_bar_boss_text = null;

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
        hp_bar_img = hp_bar_boss_background.GetComponent<CompImage>();
        hp_bar_img_background = GetComponent<CompImage>();

        hp_bar_img.SetAlpha(0.0f);
        hp_bar_img_background.SetAlpha(0.0f);
        hp_bar_appearing_timer = 0.0f;
        appeared = false;
    }

    public void Update()
    {
        if (!appeared)
        {
            hp_bar_appearing_timer += Time.deltaTime;
            hp_bar_img.SetAlpha(hp_bar_appearing_timer / hp_bar_appearing_time);
            hp_bar_img_background.SetAlpha(hp_bar_appearing_timer / hp_bar_appearing_time);
            if (hp_bar_appearing_timer >= hp_bar_appearing_time)
            {
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