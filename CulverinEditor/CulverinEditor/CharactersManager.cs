using CulverinEditor;
using CulverinEditor.Debug;

public class CharactersManager : CulverinBehaviour
{
    public enum State
    {
        IDLE = 0,
        CHANGING_LEFT,
        CHANGING_RIGHT
    }

    public GameObject current_character;
    public GameObject left_character;
    public GameObject right_character;
    private GameObject temporal_change = null;

    State state = State.IDLE;                   // To manage player state
    float time = 0;
    bool changed = false;
    void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        current_character = GetLinkedObject("current_character");
        left_character = GetLinkedObject("left_character");
        right_character = GetLinkedObject("right_character");
        // Start Idle animation
        // GameObject lweapon_obj_anim = GetLinkedObject("lweapon_obj_anim");
        //anim_controller_left = lweapon_obj_anim.GetComponent<CompAnimation>();
        //anim_controller_left.PlayAnimation("Idle");
    }

    void Update()
    {
        switch(state)
        {
            case State.IDLE:

               

                if (Input.GetKeyDown(KeyCode.T))
                {
                    state = State.CHANGING_LEFT;
                    Debug.Log("Pressed T");
                    Debug.Log("Changing Left");
                    //current character state = BEHIND
                    time = 0;
                }
                if (Input.GetKeyDown(KeyCode.Y))
                {
                    Debug.Log("Pressed Y");
                    Debug.Log("Changing Right");
                    state = State.CHANGING_RIGHT;
                    time = 0;
                }
                break;

            case State.CHANGING_LEFT:

                time += Time.DeltaTime();

               
                //if( Animation arms down finished¿? )
                if (time >= 2)
                {
                  
   
                    temporal_change = current_character;
                    current_character = left_character;
                    //current character state CURRENT
                    left_character = temporal_change;
                    
                    Debug.Log("current character = " + current_character.GetName());
                    changed = true;
                }
                //if( Animation arms up finished¿? )
                if(changed == true)
                {
                    
                    changed = false;
                    state = State.IDLE;
                }
                break;

            case State.CHANGING_RIGHT:

                time += Time.DeltaTime();

               

                if (time >= 2)
                {

                    temporal_change = current_character;
                    current_character = right_character;
                    right_character = temporal_change;
                    Debug.Log("current character = " + current_character.GetName());
                    changed = true;
                }
                //if(current_character state == CURRENT )
                if (changed == true)
                {
                    changed = false;
                    state = State.IDLE;
                }    
                break;
        }
    }
}