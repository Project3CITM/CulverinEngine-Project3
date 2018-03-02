using CulverinEditor;
using CulverinEditor.Debug;

public class CoolDown : CulverinBehaviour
{
    public GameObject character;
    public CompButton button_cd;
    public GameObject this_obj;
    public float cd_time = 2.0f;
    public float act_time = 0.0f;
    public bool in_cd = false;

    void Start()
    {
        this_obj = GetLinkedObject("this_obj");
        character = GetLinkedObject("character");
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
        if (character.GetComponent<CharacterController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                ActivateAbility();
            }
        }
    }

    public void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = this_obj.GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}