using CulverinEditor;
using CulverinEditor.Debug;

public class CoolDown : CulverinBehaviour
{
    public CompButton button_cd;

    public float cd_time = 2.0f;
    public float act_time = 0.0f;
    public bool in_cd = false;

    void Start()
    {
    }

    void Update()
    {
        if (in_cd)
        {
            act_time += Time.DeltaTime();
            if (act_time >= cd_time)
            {
                in_cd = false;
                button_cd = GetComponent<CompButton>();
                button_cd.Activate();
            }
        }
    }

    public virtual void OnClick()
    {
        Debug.Log("On Click CD");
    }

    public virtual void ActivateAbility()
    {
        Debug.Log("Activate Ability CD");
    }
}