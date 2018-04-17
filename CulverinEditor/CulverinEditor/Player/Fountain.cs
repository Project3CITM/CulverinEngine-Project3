using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class Fountain : CulverinBehaviour
{
    public GameObject fountain_interact;
    public GameObject player;
    private bool on_fountain_range = false;

    void Start()
    {
        player = GetLinkedObject("player");
        fountain_interact = GetLinkedObject("fountain_interact");
        on_fountain_range = false;
    }

    void Update()
    {
        if (on_fountain_range)
        {
            if (Input.GetInput_KeyDown("Interact", "Player"))
            {
                GetLinkedObject("player").GetComponent<CharactersManager>().HealCharacters();
            }
        }
    }

    // OnTrigger Lever ------------------------
    void OnTriggerEnter()
    {
        CompCollider col = GetComponent<CompCollider>();
        GameObject obj_col = col.GetCollidedObject();
        Debug.Log(obj_col.GetTag().ToString());

        if (obj_col != null && obj_col.CompareTag("player"))
        {
            fountain_interact = GetLinkedObject("fountain_interact");
            fountain_interact.SetActive(true);
            on_fountain_range = true;

        }
    }

    void OnTriggerLost()
    {
        on_fountain_range = false;
    }
}