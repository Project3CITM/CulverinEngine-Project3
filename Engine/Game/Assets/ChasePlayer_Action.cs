﻿using CulverinEditor;
using CulverinEditor.Debug;

public class ChasePlayer_Action : Action
{
    int current_tile_x;
    int current_tile_y;

    PerceptionEvent event_to_react;
    Movement_Action move;
    CompAnimation anim;
    ACTION_RESULT move_return;

    public bool forgot_event = false;
    public float check_player_timer = 1.0f;
    float timer = 0.0f;

    void Start()
    {
        move = GetComponent<Movement_Action>();
        anim = GetComponent<CompAnimation>();
    }

    public ChasePlayer_Action()
    {
        action_type = ACTION_TYPE.CHASE_ACTION;
    }

    public ChasePlayer_Action(float speed):base(speed)
    {
        action_type = ACTION_TYPE.CHASE_ACTION;
    }

    public override bool ActionStart()
    {
        bool ret = move.ActionStart();
        Debug.Log("Chasing Player");

        current_tile_x = move.GetCurrentTileX();
        current_tile_y = move.GetCurrentTileY();

        anim = GetComponent<CompAnimation>();
        anim.SetTransition("Unshade");

        //event_to_react.start_counting = false;

        /*event_to_react = new PerceptionPlayerSeenEvent(5, 2, 24, gameObject);

        event_to_react.SetOrigin(current_tile_x, current_tile_y);
        event_to_react.SetDestiny(2, 24);*/

        event_to_react.start_counting = false;

        move.GoToPrevious(current_tile_x, current_tile_y, event_to_react.objective_tile_x, event_to_react.objective_tile_y, true);

        return ret;
    }

    public override bool ActionEnd()
    {
        move.NotChase();
        base.ActionEnd();
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        /*if (anim.IsAnimationStopped("Prep desenvainar"))
            anim.SetTransition("Desenv");
        else if (anim.IsAnimationStopped("Desenv"))
            anim.SetTransition("Chase");*/

        if (interupt || forgot_event)
        {
            move.Interupt();
        }

        timer += Time.deltaTime;

        if (timer >= check_player_timer)
        {
            timer = 0.0f;

            current_tile_x = move.GetCurrentTileX();
            current_tile_y = move.GetCurrentTileY();

            int player_x, player_y;
            GetComponent<PerceptionSightEnemy>().GetPlayerTilePos(out player_x, out player_y);
            move.GoToPrevious(current_tile_x, current_tile_y, player_x, player_y);
        }

        if (GetComponent<PerceptionSightEnemy>().player_seen == false)
            event_to_react.start_counting = true;
        else
            event_to_react.start_counting = false;

        ///Make Move update
        move.chase = true;
        move_return = move.ActionUpdate();

        if (move_return == ACTION_RESULT.AR_SUCCESS)
        {
            Debug.Log("Chased");
        }

        if (move_return == ACTION_RESULT.AR_FAIL)
        {
            move.ActionEnd();
            return ACTION_RESULT.AR_FAIL;
        }        

        return move_return;
    }

    public void SetEvent(PerceptionEvent e)
    {
        event_to_react = e;
    }
}