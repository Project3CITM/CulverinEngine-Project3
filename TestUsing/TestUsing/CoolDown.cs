using CulverinEditor;
using CulverinEditor.Debug;

public class CoolDown : CulverinBehaviour
{
    CompButton button_cd;
    float cd_time = 2.0f;
    float act_time = 0.0f;
    bool in_cd = false;

    void Start()
    {
        button_cd = GetComponent<CompButton>();
        act_time = Time.DeltaTime();
    }

    void Update()
    {
        if (in_cd)
        {
            act_time += Time.DeltaTime();
            if (act_time >= cd_time)
            { 
            in_cd = false;
            button_cd.Activate();
            }
        }

    }

    void OnClick()
    {
        if (in_cd == false)
        {
            button_cd.Deactivate();
            Debug.Log("clicked");
            act_time = 0.0f;
            in_cd = true;
        }
    }

}