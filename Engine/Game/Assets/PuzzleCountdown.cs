using System;
using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class PuzzleCountdown:CulverinBehaviour
{


    public float initial_time = 0.0f;
    public float current_time = 0.0f;
    public float state_2_time = 0.0f;
    public float state_3_time = 0.0f;

    public bool active = false;

    int state = 1;

    public void StartCountdown()
    {
        current_time = initial_time;
        active = true;
        state = 1;
    }

    public bool IsCountdownOver()
    {
        if(current_time <= 0.0f && active == false)
        {
            return true;
        }
        
        return false;
    }

    void Update()
    {
        if(active)
        {
            current_time -= Time.deltaTime;


            if(current_time <= state_2_time && state == 1)
            {
                state = 2;
            }
            else if (current_time <= state_3_time && state == 2)
            {
                state = 3;
            }
            else if (current_time <= 0.0f)
            {
                active = false;
            }

        }


    }


}