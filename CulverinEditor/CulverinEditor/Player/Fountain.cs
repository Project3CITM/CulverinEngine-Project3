using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class Fountain : CulverinBehaviour
{
    public GameObject fountain_interact;
    public GameObject player;
    public GameObject upper_water;
    public GameObject bottom_water;
    public int number_of_uses = 3;
    public float final_y_bottom = 0;
    public float final_x_scale_bottom = 0;
    public float final_z_scale_bottom = 0;
    public float deplenish_speed = 0.01f;
    public float heal_percentage = 0.25f;
    private float starting_y_bottom;
    private float starting_x_scale_bottom;
    private float starting_z_scale_bottom;
    private float destination_y = 0;
    private float destination_x = 0;
    private float destination_z = 0;
    private int current_usage = 0;
    private bool reducing_water = false;
    private bool on_fountain_range = false;

    void Start()
    {
        player = GetLinkedObject("player");
        fountain_interact = GetLinkedObject("fountain_interact");
        upper_water = GetLinkedObject("upper_water");
        bottom_water = GetLinkedObject("bottom_water");
        on_fountain_range = false;
        current_usage = number_of_uses;
        reducing_water = false;
        starting_x_scale_bottom = bottom_water.transform.GetScale().x;
        starting_z_scale_bottom = bottom_water.transform.GetScale().z;
        starting_y_bottom = bottom_water.transform.GetPosition().y;

        fountain_interact.GetComponent<CompImage>().DeactivateRender();
    }

    void Update()
    {
        if (on_fountain_range == true)
        {
            if (Input.GetInput_KeyDown("Interact", "Player") && current_usage > 0)
            {
                player.GetComponent<CharactersManager>().HealCharacters(heal_percentage);
                reducing_water = true;
                current_usage--;
                float by = (float)current_usage / (float)number_of_uses;
                destination_y = Mathf.Lerp(starting_y_bottom, final_y_bottom, by);
                destination_x = Mathf.Lerp(starting_x_scale_bottom, final_x_scale_bottom, by);
                destination_z = Mathf.Lerp(starting_z_scale_bottom, final_z_scale_bottom, by);
                if (current_usage == 0)
                {
                    on_fountain_range = false;
                    fountain_interact.GetComponent<CompImage>().DeactivateRender();
                }
                StatsScore.Heal();
            }
        }

        if (reducing_water == true)
        {
            int checks = 0;
            Vector3 bottom_pos = bottom_water.transform.GetPosition();
            if (destination_y < bottom_pos.y)
            {
                bottom_water.transform.SetPosition(new Vector3(bottom_pos.x, bottom_pos.y - deplenish_speed, bottom_pos.z));
            }
            else
            {
                checks++;
            }

            Vector3 bottom_scale = bottom_water.transform.GetScale();
            if (destination_x < bottom_scale.x)
            {
                bottom_water.transform.SetScale(new Vector3(bottom_scale.x - deplenish_speed, bottom_scale.y, bottom_scale.z));
                bottom_scale.x -= Time.deltaTime * 2;
            }
            else
            {
                checks++;
            }
            if (destination_z < bottom_scale.z)
            {
                bottom_water.transform.SetScale(new Vector3(bottom_scale.x, bottom_scale.y, bottom_scale.z - deplenish_speed));
            }
            else
            {
                checks++;
            }

            if (checks == 3)
            {
                reducing_water = false;
            }
        }
    }

    // OnTrigger Lever ------------------------
    void OnTriggerEnter()
    {
        if (current_usage > 0)
        {
            fountain_interact.GetComponent<CompImage>().ActivateRender();
            on_fountain_range = true;
        }
    }

    void OnTriggerLost()
    {
        on_fountain_range = false;
        fountain_interact.GetComponent<CompImage>().DeactivateRender();
    }
}