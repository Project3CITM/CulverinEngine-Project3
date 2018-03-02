using CulverinEditor;
using CulverinEditor.Debug;

public class CoolDownLeft : CulverinBehaviour
{
    GameObject this_button_left;
    CompButton button_cd;
    float cd_time = 4.0f;
    float act_time = 0.0f;
    public bool in_cd = false;

    void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        this_button_left = GetLinkedObject("this_button_left");
    }

    void Update()
    {
        if (in_cd)
        {
            act_time += Time.DeltaTime();
            if (act_time >= cd_time)
            {
                in_cd = false;
                button_cd = this_button_left.GetComponent<CompButton>();
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
                GetLinkedObject("player_obj").GetComponent<CharacterController>().SetState(CharacterController.State.ATTACKING);
            }
        }
    }

    public void ActivateAbility()
    {
        //this_obj.GetComponent
        button_cd = this_button_left.GetComponent<CompButton>(); // The script is placed in the same GameObject that contains the CompButton
        button_cd.Deactivate();

        Debug.Log("Clicked");
        act_time = 0.0f;
        in_cd = true;
    }
}