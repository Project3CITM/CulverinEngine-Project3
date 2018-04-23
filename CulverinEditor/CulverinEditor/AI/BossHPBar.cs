using CulverinEditor;
using CulverinEditor.Debug;

public class BossHPBar : CulverinBehaviour
{
    public GameObject hp_bar_boss_background = null;
    public GameObject hp_bar_boss_image = null;
    public GameObject hp_bar_boss_text = null;

    public void Start()
    {
        hp_bar_boss_background = GetLinkedObject("hp_bar_boss_background");
        hp_bar_boss_image = GetLinkedObject("hp_bar_boss_image");
        hp_bar_boss_text = GetLinkedObject("hp_bar_boss_text");

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
