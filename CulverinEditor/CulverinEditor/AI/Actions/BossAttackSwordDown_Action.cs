﻿using CulverinEditor.Debug;
using CulverinEditor;

public class BossAttackSwordDown_Action : Action
{
    public BossAttackSwordDown_Action()
    {
        action_type = ACTION_TYPE.BOSS_ATTACK_SWORD_DOWN_ACTION;
    }

    public BossAttackSwordDown_Action(float dmg)
    {
        action_type = ACTION_TYPE.BOSS_ATTACK_SWORD_DOWN_ACTION;
        damage = dmg;
    }

    public enum BASD_STATE
    {
        WAITING,
        PRE_APPLY,
        POST_APPLY
    }

    BASD_STATE state = BASD_STATE.WAITING;
    public float damage = 1.0f;
    public float apply_damage_point = 0.5f;
    public float attack_duration = 1.0f;


    public override bool ActionStart()
    {

        state = BASD_STATE.PRE_APPLY;
        GetComponent<CompAnimation>().SetTransition("ToAttack");
        GetComponent<CompAnimation>().SetClipDuration("Attack", attack_duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (state == BASD_STATE.PRE_APPLY && GetComponent<CompAnimation>().IsAnimOverXTime(apply_damage_point))
        {
            state = BASD_STATE.POST_APPLY;

            int enemy_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
            int enemy_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

            GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int player_tile_x, out int player_tile_y);
         
            switch(GetComponent<Movement_Action>().GetDirection())
            {
                case Movement_Action.Direction.DIR_WEST:
                    if ((enemy_tile_x - 1 == player_tile_x || enemy_tile_x - 2 == player_tile_x) && player_tile_y == enemy_tile_y)
                    {
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_EAST:
                    if ((enemy_tile_x + 1 == player_tile_x || enemy_tile_x + 2 == player_tile_x) && player_tile_y == enemy_tile_y)
                    {
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_NORTH:
                    if ((enemy_tile_y - 1 == player_tile_y || enemy_tile_y - 2 == player_tile_y) && player_tile_x == enemy_tile_x)
                    {
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_SOUTH:
                    if ((enemy_tile_y + 1 == player_tile_y || enemy_tile_y + 2 == player_tile_y) && player_tile_x == enemy_tile_x)
                    {
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                    }
                    break;
            }
        }
        else if (state == BASD_STATE.POST_APPLY && GetComponent<CompAnimation>().IsAnimationStopped("Attack"))
        {

            state = BASD_STATE.WAITING;
            return ACTION_RESULT.AR_SUCCESS;
        }
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        Debug.Log("SwordDown attack end!");
        interupt = false;
        return false;
    }
}