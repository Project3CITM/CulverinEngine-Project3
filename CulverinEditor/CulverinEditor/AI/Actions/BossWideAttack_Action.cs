using CulverinEditor.Debug;
using CulverinEditor;

public class BossWideAttack_Action : Action
{
    public BossWideAttack_Action()
    {
        action_type = ACTION_TYPE.BOSS_ATTACK_WIDE_ACTION;
    }

    public BossWideAttack_Action(float dmg)
    {
        action_type = ACTION_TYPE.BOSS_ATTACK_WIDE_ACTION;
        damage = dmg;
    }

    public enum BWA_STATE
    {
        WAITING,
        PRE_APPLY,
        POST_APPLY
    }

    BWA_STATE state = BWA_STATE.WAITING;
    public float damage = 1.0f;
    public float apply_damage_point = 0.5f;
    public float attack_duration = 1.0f;


    public override bool ActionStart()
    {

        state = BWA_STATE.PRE_APPLY;
        GetComponent<CompAnimation>().SetTransition("ToAttack");
        GetComponent<CompAnimation>().SetClipDuration("Attack", attack_duration);
        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {

        if (state == BWA_STATE.PRE_APPLY && GetComponent<CompAnimation>().IsAnimOverXTime(apply_damage_point))
        {
            state = BWA_STATE.POST_APPLY;

            int enemy_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
            int enemy_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

            GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int player_tile_x, out int player_tile_y);

            switch (GetComponent<Movement_Action>().GetDirection())
            {
                case Movement_Action.Direction.DIR_WEST:
                    if ((enemy_tile_x - 1 == player_tile_x || enemy_tile_x == player_tile_x) && (player_tile_y == enemy_tile_y || player_tile_y == enemy_tile_y -1 || player_tile_y == enemy_tile_y + 1))
                    {
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_EAST:
                    if ((enemy_tile_x + 1 == player_tile_x || enemy_tile_x == player_tile_x) && (player_tile_y == enemy_tile_y || player_tile_y == enemy_tile_y - 1 || player_tile_y == enemy_tile_y + 1))
                    {
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_NORTH:
                    if ((enemy_tile_y - 1 == player_tile_y || enemy_tile_y == player_tile_y) && (player_tile_x == enemy_tile_y || player_tile_x == enemy_tile_y - 1 || player_tile_y == enemy_tile_x + 1))
                    {
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_SOUTH:
                    if ((enemy_tile_y + 1 == player_tile_y || enemy_tile_y == player_tile_y) && (player_tile_x == enemy_tile_y || player_tile_x == enemy_tile_y - 1 || player_tile_y == enemy_tile_x + 1))
                    {
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                    }
                    break;
            }
        }
        else if (state == BWA_STATE.POST_APPLY && GetComponent<CompAnimation>().IsAnimationStopped("Attack"))
        {

            state = BWA_STATE.WAITING;
            return ACTION_RESULT.AR_SUCCESS;
        }
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        Debug.Log("Wide attack end!");
        interupt = false;
        return false;
    }
}