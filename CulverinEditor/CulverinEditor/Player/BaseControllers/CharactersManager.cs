﻿using CulverinEditor;
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

    State state = State.IDLE;   // To manage player state

    void Start()
    {
        // LINK GAMEOBJECTS OF THE SCENE WITH VARIABLES
        current_character = GetLinkedObject("current_character");
        left_character = GetLinkedObject("left_character");
        right_character = GetLinkedObject("right_character");
    }

    void Update()
    {
        switch(state)
        {
            case State.IDLE:
                {
                    if (Input.GetKeyDown(KeyCode.T))
                    {
                        state = State.CHANGING_LEFT;
                        Debug.Log("Pressed T");
                        Debug.Log("Changing Left");
                        current_character.GetComponent<CharacterController>().SetAnimationTransition("ToOut", true);
                    }
                    else if (Input.GetKeyDown(KeyCode.Y))
                    {
                        state = State.CHANGING_RIGHT;
                        Debug.Log("Pressed Y");
                        Debug.Log("Changing Right");
                        current_character.GetComponent<CharacterController>().SetAnimationTransition("ToOut", true);
                    }

                    //MANAGE SECONDARY ABILITIES -------------------------------------
                    else if(Input.GetKeyDown(KeyCode.K))
                    {
                        Debug.Log("Left Secondary Ability");
                        left_character.GetComponent<CharacterController>().SecondaryAbility();
                    }
                    else if (Input.GetKeyDown(KeyCode.L))
                    {
                        Debug.Log("Right Secondary Ability");
                        right_character.GetComponent<CharacterController>().SecondaryAbility();
                    }
                    // ---------------------------------------------------------------
                    break;
                }

            case State.CHANGING_LEFT:
                {
                    if (current_character.GetComponent<CompAnimation>().IsAnimationStopped("Out"))
                    {
                        left_character.GetComponent<CharacterController>().SetAnimationTransition("ToIn", true);
                        current_character.GetComponent<CharacterController>().SetPosition(CharacterController.Position.BEHIND);
                        left_character.GetComponent<CharacterController>().SetPosition(CharacterController.Position.CURRENT);

                        temporal_change = current_character;
                        current_character = left_character;
                        left_character = temporal_change;

                        //Deactivate previous current character HUD
                        left_character.GetComponent<CharacterController>().UpdateHUD(false);
                        //Activate current character HUD
                        current_character.GetComponent<CharacterController>().UpdateHUD(true);

                        Debug.Log("current character = " + current_character.GetName());
                        state = State.IDLE;
                    }
                    break;
                }

            case State.CHANGING_RIGHT:
                {
                    if (current_character.GetComponent<CompAnimation>().IsAnimationStopped("Out"))
                    {
                        right_character.GetComponent<CharacterController>().SetAnimationTransition("ToIn", true);
                        current_character.GetComponent<CharacterController>().SetPosition(CharacterController.Position.BEHIND);
                        right_character.GetComponent<CharacterController>().SetPosition(CharacterController.Position.CURRENT);

                        temporal_change = current_character;
                        current_character = right_character;
                        right_character = temporal_change;

                        //Deactivate previous current character HUD
                        right_character.GetComponent<CharacterController>().UpdateHUD(false);
                        //Activate current character HUD
                        current_character.GetComponent<CharacterController>().UpdateHUD(true);

                        Debug.Log("current character = " + current_character.GetName());
                        state = State.IDLE;
                    }
                    break;
                }
        }
    }
}