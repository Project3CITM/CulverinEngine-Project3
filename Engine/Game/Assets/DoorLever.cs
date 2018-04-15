using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class DoorLever : CulverinBehaviour
{
    
    public bool active_lever = false;
  

    public GameObject lever_interact = null;
    public GameObject lever_go = null;
    private CompAnimation anim_controller = null;
    private CompAudio audio = null;
    private bool on_lever_range = false;
    private bool on_lever_animation = false;
    private string lever_animation_name = "Lever_Down"; 
   

    

    void Start()
    {
        
        lever_go = GetLinkedObject("lever_go");

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
    }

    void OnLeverAnimFinish()
    {
        on_lever_animation = false;
      
        active_lever = true;
        Debug.Log("DOOR OPENED");
    }


}