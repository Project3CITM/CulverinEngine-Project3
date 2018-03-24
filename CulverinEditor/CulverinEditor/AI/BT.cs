﻿using CulverinEditor;
using CulverinEditor.Debug;

public class BT : CulverinBehaviour
{
    public enum AI_STATE
    {
        AI_IDLE,
        AI_STUNNED,
        AI_MOVING,
        AI_ATTACKING,
        AI_BLOCKING,
        AI_DEAD
    }

    protected Action null_action = new Action();
    protected Action current_action = null;
    protected Action next_action = null;
    protected AI_STATE state = AI_STATE.AI_IDLE;

    //Blackboard
    public bool player_detected = false;
    public bool heard_something = false;

    //Path Go and Back
    public int origin_path_x;
    public int origin_path_y;
           
    public int end_path_x;
    public int end_path_y;

    public float anim_speed = 1.0f;

    virtual public void Start()
    {
        next_action = null_action;
        current_action = GetComponent<Idle_Action>();
        current_action.SetAnimSpeed(anim_speed);
        current_action.ActionStart();
        MakeDecision();
    }

    public virtual void Update()
    {
        Action.ACTION_RESULT result = current_action.ActionUpdate();

        if (current_action.action_type == Action.ACTION_TYPE.IDLE_ACTION || current_action.action_type == Action.ACTION_TYPE.IDLE_ATTACK_ACTION)
        {
            MakeDecision();
        }
        else
        {
            switch (result)
            {
                case Action.ACTION_RESULT.AR_FAIL:
                    current_action.ActionEnd();
                    MakeDecision();
                    break;

                case Action.ACTION_RESULT.AR_IN_PROGRESS:
                    break;

                case Action.ACTION_RESULT.AR_SUCCESS:
                    current_action.ActionEnd();
                    MakeDecision();
                    break;

                default:
                    Debug.Log("Error on action state!");
                    break;
            }
        }
    }

    public virtual void MakeDecision()
    {
        if(next_action.action_type != Action.ACTION_TYPE.NO_ACTION)
        {
            current_action = next_action;
            next_action = new Action();
        }
    }

    public AI_STATE GetState()
    {
        return state;
    }

    public void InterruptAction()
    {
        current_action.Interupt();
    }
}
