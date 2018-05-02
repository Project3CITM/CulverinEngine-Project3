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
    public float preparation_time = 0.8f;
    public float attack_duration = 1.0f;

    private CompAudio audio;
    private CompAnimation anim;
    private Movement_Action move;
    private MovementController movement_ctrl;
    private CharactersManager characters_manager;

    public override bool ActionStart()
    {
        anim = GetComponent<CompAnimation>();
        audio = GetComponent<CompAudio>();
        move = GetComponent<Movement_Action>();
        movement_ctrl = GetLinkedObject("player_obj").GetComponent<MovementController>();
        characters_manager = GetLinkedObject("player_obj").GetComponent<CharactersManager>();

        state = BWA_STATE.PRE_APPLY;
        anim.SetTransition("ToWideAttack");
        anim.SetClipDuration("WideAttack", preparation_time / apply_damage_point);
        audio.PlayEvent("AttackPreparation");

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (state == BWA_STATE.PRE_APPLY && anim.IsAnimOverXTime(apply_damage_point))
        {
            anim.SetClipDuration("WideAttack", (attack_duration / (1.0f - apply_damage_point)));

            state = BWA_STATE.POST_APPLY;

            int enemy_tile_x = move.GetCurrentTileX();
            int enemy_tile_y = move.GetCurrentTileY();

            movement_ctrl.GetPlayerPos(out int player_tile_x, out int player_tile_y);
            audio.PlayEvent("BossSwordSwing");

            switch (move.GetDirection())
            {
                case Movement_Action.Direction.DIR_WEST:
                    if ((enemy_tile_x - 1 == player_tile_x || enemy_tile_x == player_tile_x) && (player_tile_y == enemy_tile_y || player_tile_y == enemy_tile_y -1 || player_tile_y == enemy_tile_y + 1))
                    {
                        characters_manager.GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_EAST:
                    if ((enemy_tile_x + 1 == player_tile_x || enemy_tile_x == player_tile_x) && (player_tile_y == enemy_tile_y || player_tile_y == enemy_tile_y - 1 || player_tile_y == enemy_tile_y + 1))
                    {
                        characters_manager.GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_NORTH:
                    if ((enemy_tile_y - 1 == player_tile_y || enemy_tile_y == player_tile_y) && (player_tile_x == enemy_tile_x || player_tile_x == enemy_tile_x - 1 || player_tile_x == enemy_tile_x + 1))
                    {
                        characters_manager.GetDamage(damage);
                    }
                    break;
                case Movement_Action.Direction.DIR_SOUTH:
                    if ((enemy_tile_y + 1 == player_tile_y || enemy_tile_y == player_tile_y) && (player_tile_x == enemy_tile_x || player_tile_x == enemy_tile_x - 1 || player_tile_x == enemy_tile_x + 1))
                    {
                        characters_manager.GetDamage(damage);
                    }
                    break;
            }
        }
        else if (state == BWA_STATE.POST_APPLY && anim.IsAnimationStopped("WideAttack"))
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