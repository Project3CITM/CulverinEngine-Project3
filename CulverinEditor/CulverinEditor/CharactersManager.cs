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

                Debug.Log(current_character.GetName());

                Debug.Log(current_character.GetComponent<BasicFunctionality>().time);

                if (Input.GetKeyDown(KeyCode.T))
                {
                    state = State.CHANGING_LEFT;
                
                    //current character state = BEHIND
                    time = 0;
                }
                if (Input.GetKeyDown(KeyCode.Y))
                {
                    state = State.CHANGING_RIGHT;
                    time = 0;
                }

                break;

            case State.CHANGING_LEFT:

                time += Time.DeltaTime();

                Debug.Log("Changing Left");
                //if( Animation arms down finished¿? )
                if (time >= 2)
                {
                    Debug.Log("Changed");
   
                    temporal_change = current_character;
                    current_character = left_character;
                    //current character state CURRENT
                    left_character = temporal_change;
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

                Debug.Log("Changing Right");

                if (time >= 2)
                {
                    Debug.Log("Changed");

                    temporal_change = current_character;
                    current_character = right_character;
                    right_character = temporal_change;
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