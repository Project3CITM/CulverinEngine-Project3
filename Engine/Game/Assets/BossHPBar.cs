using CulverinEditor;
using CulverinEditor.Debug;

public class BossHPBar : CulverinBehaviour
{
    public void Start()
    {

    }

    public void Update()
    {

    }

    public void SetHPBar(float hp_percentage)
    {
        CompImage hp_bar = gameObject.GetComponent<CompImage>();
        hp_bar.FillAmount(hp_percentage);
    }
}
