﻿using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class BarrelFall : CulverinBehaviour
{

    public enum ModeBarrel
    {
        UNKOWN = 0,
        PUZZLE,
        FILLING
    }

    ModeBarrel mode_puzzle = ModeBarrel.UNKOWN;
    public float speed = 1.0f;
    public float weight = 1.0f;
    public int target_tile_x = 0;
    public int target_tile_y = 0;
    public float initial_fall_speed = 0.0f;
    public float tile_size = 25.4f;
    public float error_margin = 1.0f;
    public float floor_height = 0.0f;
    float target_pos_x = 0.0f;
    float target_pos_y = 0.0f;


    bool in_tile = false;
    bool placed = false;

    float fall_displacement = 0.0f;
    float fall_time = 0.0f;

    void Start()
    {

    }

    void Update()
    {       
        if (in_tile == false)
        {
            MoveToTile();
        }
        else
        {
            Fall();
        }
    }

    void MoveToTile()
    {
        target_pos_x = target_tile_x * tile_size;
        target_pos_y = target_tile_y * tile_size;


        Vector3 global_pos = transform.GetGlobalPosition();
        bool in_x = false;
        bool in_y = false;

        //Displace barrel in X
        if (Mathf.Abs(target_pos_x - global_pos.x) > error_margin)
        {
            float displacement;
            if (target_pos_x - global_pos.x < 0)
            {
                displacement = -1;
            }
            else displacement = 1;

            transform.local_position = new Vector3(transform.local_position.x + (Time.deltaTime * speed * displacement), transform.local_position.y, transform.local_position.z);
        }
        else in_x = true;

        //Displace barrel in Y
        if (Mathf.Abs(target_pos_y - global_pos.z) > error_margin)
        {
            float displacement;
            if (target_pos_y - global_pos.z < 0)
            {
                displacement = -1;
            }
            else displacement = 1;

            transform.local_position = new Vector3(transform.local_position.x, transform.local_position.y, transform.local_position.z + (Time.deltaTime * speed * displacement));
        }
        else in_y = true;

        if (in_x && in_y)
        {
            //If arrived to tile
            in_tile = true;
        }
    }
    void Fall()
    {
        Vector3 global_pos = transform.GetGlobalPosition();
        if (global_pos.y > floor_height)
        {
           // Debug.Log(fall_time);
            fall_time += Time.deltaTime;
            float new_height = transform.local_position.y;

            fall_displacement = initial_fall_speed* Time.deltaTime * fall_time - 0.5f*9.8f* weight* (fall_time* fall_time);
            new_height += fall_displacement * Time.deltaTime;

            transform.local_position = new Vector3(transform.local_position.x, new_height, transform.local_position.z);
        }
        else
        {
           this.SetEnabled(false);
            placed = true;
        }
    }

    public bool IsPlaced()
    {
        return placed;
    }

    public bool IsPuzzleMode()
    {
        return (mode_puzzle == ModeBarrel.PUZZLE);
    }

    public void SetData(float new_speed, float new_weight, int new_target_tile_x, int new_target_tile_y, float new_initial_fall_speed, ModeBarrel mode, float new_floor_height = 0.0f)
    {
        speed = new_speed;
        weight = new_weight;
        target_tile_x = new_target_tile_x;
        target_tile_y = new_target_tile_y;
        initial_fall_speed = new_initial_fall_speed;
        floor_height = new_floor_height;

        target_pos_x = target_tile_x * tile_size;
        target_pos_y = target_tile_y * tile_size;
        mode_puzzle = mode;

        //Debug.Log(target_pos_y);

    }
}

