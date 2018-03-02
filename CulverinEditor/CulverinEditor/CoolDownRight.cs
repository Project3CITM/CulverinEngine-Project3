using CulverinEditor;
using CulverinEditor.Debug;

public class CoolDownRight : CulverinBehaviour
{
    GameObject this_button_right;
    CompButton button_cd;
    float cd_time = 2.0f;
    float act_time = 0.0f;
    public bool in_cd = false;

    void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        this_button_right = GetLinkedObject("this_button_right");
    }

    void Update()
    {
        if (in_cd)
        {
            act_time += Time.DeltaTime();
            if (act_time >= cd_time)
            {
                in_cd = false;
                button_cd = this_button_right.GetComponent<CompButton>();
                button_cd.Activate();
            }
        }

    }

    void OnClick()
    {
        if (GetLinkedObject("player_obj").GetComponent<CharacterController>().GetState() == 0)
        {
            if (in_cd == false)
            {
                ActivateAbility();
                // Set Attacking State
                GetLinkedObject("player_obj").GetComponent<CharacterController>().SetState(CharacterController.State.COVER);
            }
        }
    }

    public void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = this_button_right.GetComponent<CompButton>();
        button_cd.Deactivate();

        Debug.Log("Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}