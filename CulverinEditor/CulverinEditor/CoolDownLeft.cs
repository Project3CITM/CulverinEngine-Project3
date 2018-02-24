using CulverinEditor;
using CulverinEditor.Debug;

public class CoolDownLeft : CulverinBehaviour
{
    CompButton button_cd;
    GameObject l_cd_button;
    float cd_time = 4.0f;
    float act_time = 0.0f;
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

    void OnClick()
    {
        if (in_cd == false)
        {
            ActivateAbility();
        }
    }

    public void ActivateAbility()
    {
        l_cd_button = GetLinkedObject("l_cd_button");
        //this_obj.GetComponent
        button_cd = l_cd_button.GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}