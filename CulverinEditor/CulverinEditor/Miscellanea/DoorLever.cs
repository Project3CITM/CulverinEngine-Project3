using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class DoorLever : CulverinBehaviour
{
    
    public bool active_lever = false;

    public GameObject door_to_open1 = null;
    public GameObject door_to_open2 = null;

    public GameObject lever_interact = null;
    public GameObject lever_go = null;
    private CompAnimation anim_controller = null;
    private CompAudio audio = null;
    private bool on_lever_range = false;
    public bool on_lever_animation = false;
    private string lever_animation_name = "Lever_Down";

    public GameObject close_door_collider = null;
    public bool do_collider_close_door = false;


    void Start()
    {
        
        lever_go = GetLinkedObject("lever_go");
        if(door_to_open1!=null)
        {
        door_to_open1 = GetLinkedObject("door_to_open1");
        }

        if (door_to_open2 != null)
        {
            door_to_open2 = GetLinkedObject("door_to_open2");
        }

        close_door_collider = GetLinkedObject("close_door_collider");

        audio = GetComponent<CompAudio>();
        if (audio == null)
        {
            Debug.Log("There is no audio in puzzle!");
        }
               

        anim_controller = lever_go.GetComponent<CompAnimation>();
        if (anim_controller == null)
        {
            Debug.Log("Animation is null!");
        }
             

        lever_interact = GetLinkedObject("lever_interact");
        if(lever_interact != null)
        {
            lever_interact.SetActive(false);
            Debug.Log("[green] Deactivated Interact");
        }
       
        // -------------------------------------------------------
    }


    void Update()
    {
        //-- TMP: Debug -----
        if(on_lever_animation && anim_controller.IsAnimationStopped(lever_animation_name))
        {
            OnLeverAnimFinish();
        }

        //-- Lever Triggered -----
        if(on_lever_range && !active_lever && !on_lever_animation)
        {            
            if (Input.GetInput_KeyDown("Interact", "Player"))
            {               
                OnLeverActivated();
                GetComponent<CompAudio>().PlayEvent("Lever");
                lever_interact.SetActive(false);
            }
        }

        if (on_lever_animation)
         {
             anim_controller = lever_go.GetComponent<CompAnimation>();
            if (anim_controller != null)
             {
                 if (anim_controller.IsAnimationStopped(lever_animation_name))
                 {
                     Debug.Log("Lever animation ended.");
                     // The lever animation has stopped so puzzle must start.
                     OnLeverAnimFinish();
                 }
             }
         }

    }

    // OnTrigger Lever ------------------------
    void OnTriggerEnter()
    {

        if (active_lever || on_lever_animation)
        {
            return;
        }

        CompCollider col = GetComponent<CompCollider>();
        GameObject obj_col = col.GetCollidedObject();
        Debug.Log(obj_col.GetTag().ToString());

        if (obj_col != null && obj_col.CompareTag("player"))
        {

            lever_interact.SetActive(true);
            on_lever_range = true;

        }

    }

    void OnTriggerLost()
    {

        if (active_lever)
        {
            on_lever_range = false;
            return;
        }

        CompCollider col = GetComponent<CompCollider>();
        GameObject obj_col = col.GetCollidedObject();
        Debug.Log(obj_col.GetTag().ToString());

        if (obj_col != null && obj_col.CompareTag("player"))
        {

            lever_interact.SetActive(false);
            on_lever_range = false;

        }

    }

    // -------------------------------------------------------------------------------------

     

    void OnLeverActivated()
    {
        // Called when lever is activated. Set flag to true and play the animation.
        on_lever_animation = true;
        Debug.Log("Animation called");
        anim_controller = lever_go.GetComponent<CompAnimation>();
        if (anim_controller != null)
        {
            anim_controller.PlayAnimation(lever_animation_name);
            Debug.Log("Animation activated");
        }
        else Debug.Log("Animation IS NULL!!");

    }

    void OnLeverAnimFinish()
    {
        on_lever_animation = false;
      
        active_lever = true;

        if(door_to_open1!=null)
        { 
         DoorLevel2 to_open = door_to_open1.GetComponent<DoorLevel2>();
          to_open.OpenDoor();
        }

        if (door_to_open2 != null)
        {
            DoorLevel2 to_open = door_to_open2.GetComponent<DoorLevel2>();
            to_open.OpenDoor();
        }

        if (do_collider_close_door)
        {
            DoorLevel2 to_open = close_door_collider.GetComponent<DoorLevel2>();
            to_open.CloseDoorByCollider();
        }
    }


}