using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class AnimationTrigger : CulverinBehaviour
{
    bool triggered = false;
    public GameObject cutscene_owner;

    void Start()
    {
        cutscene_owner = GetLinkedObject("cutscene_owner");
    }


    void Update()
    {
        if(GetComponent<DoorLever>().on_lever_animation == true && triggered == false)
        {
            triggered = true;
            cutscene_owner.GetComponent<CharactersManager>().StartPlayerCutscene();
        }
    }
}