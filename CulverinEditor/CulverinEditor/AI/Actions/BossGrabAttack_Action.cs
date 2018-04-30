using CulverinEditor.Debug;
using CulverinEditor;

public class BossGrabAttack_Action : Action
{
    public BossGrabAttack_Action()
    {
        action_type = ACTION_TYPE.BOSS_ATTACK_GRAB_ACTION;
    }

    public BossGrabAttack_Action(float dmg)
    {
        action_type = ACTION_TYPE.BOSS_ATTACK_GRAB_ACTION;
        damage = dmg;
    }

    public enum BGA_STATE
    {
        WAITING,
        PRE_APPLY,
        FIRST_APPLY,
        SECOND_APPLY,
        THIRD_APPLY,
        POST_APPLY,
        GRAB_FAILING,
        GRAB_FAILED
    }

    BGA_STATE state = BGA_STATE.WAITING;
    public float damage = 1.0f;
    public float grab_point = 0.3f;
    public float first_hit_point = 0.5f;
    public float second_hit_point = 0.7f;
    public float third_hit_point = 0.9f;
    public float attack_duration = 5.0f;
    CharactersManager player = null;

    public override bool ActionStart()
    {
        state = BGA_STATE.PRE_APPLY;
        player = GetLinkedObject("target").GetComponent<CharactersManager>();

        if (player == null)
        {
            Debug.Log("[error] Attack Action Start: Player is null!");
        }

        if (player.dying == false)
        {
            GetComponent<CompAnimation>().SetTransition("ToAttack");
            GetComponent<CompAnimation>().SetClipDuration("Attack", attack_duration);
            GetComponent<CompAudio>().PlayEvent("AttackPreparation");
        }

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (player.dying)
        {
            Debug.Log("DON'T ATTACK PLAYER", Department.PLAYER, Color.ORANGE);
            return ACTION_RESULT.AR_FAIL; //Player is dead, don't attack
        }

        if (state == BGA_STATE.PRE_APPLY && GetComponent<CompAnimation>().IsAnimOverXTime(grab_point))
        {
            state = BGA_STATE.GRAB_FAILING;

            int enemy_tile_x = GetComponent<Movement_Action>().GetCurrentTileX();
            int enemy_tile_y = GetComponent<Movement_Action>().GetCurrentTileY();

            GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int player_tile_x, out int player_tile_y);

            switch (GetComponent<Movement_Action>().GetDirection())
            {
                case Movement_Action.Direction.DIR_WEST:
                    if (enemy_tile_x - 1 == player_tile_x && player_tile_y == enemy_tile_y)
                    {
                        state = BGA_STATE.FIRST_APPLY;
                        Debug.Log("Grabing player...");
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.GRABBED);
                    }
                    break;
                case Movement_Action.Direction.DIR_EAST:
                    if (enemy_tile_x + 1 == player_tile_x && player_tile_y == enemy_tile_y)
                    {
                        state = BGA_STATE.FIRST_APPLY;
                        Debug.Log("Grabing player...");
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.GRABBED);
                    }
                    break;
                case Movement_Action.Direction.DIR_NORTH:
                    if (enemy_tile_y - 1 == player_tile_y && player_tile_x == enemy_tile_x)
                    {
                        state = BGA_STATE.FIRST_APPLY;
                        Debug.Log("Grabing player...");
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.GRABBED);
                    }
                    break;
                case Movement_Action.Direction.DIR_SOUTH:
                    if (enemy_tile_y + 1 == player_tile_y && player_tile_x == enemy_tile_x)
                    {
                        state = BGA_STATE.FIRST_APPLY;
                        Debug.Log("Grabing player...");
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
                        GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.GRABBED);
                    }
                    break;
            }
        }
        else if (state == BGA_STATE.GRAB_FAILING && GetComponent<CompAnimation>().IsAnimOverXTime(grab_point))
        {
            Debug.Log("[yellow] Grab dodged!!");
            state = BGA_STATE.GRAB_FAILED;
            GetComponent<CompAnimation>().SetTransition("ToGrabFail");
        }
        else if (state == BGA_STATE.FIRST_APPLY && GetComponent<CompAnimation>().IsAnimOverXTime(first_hit_point) && GetComponent<CompAnimation>().IsAnimationRunning("Attack"))
        {
            state = BGA_STATE.SECOND_APPLY;
            Debug.Log("First crush...");
            GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
        }
        else if (state == BGA_STATE.SECOND_APPLY && GetComponent<CompAnimation>().IsAnimOverXTime(second_hit_point) && GetComponent<CompAnimation>().IsAnimationRunning("Attack"))
        {
            state = BGA_STATE.THIRD_APPLY;
            Debug.Log("Second crush...");
            GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
        }
        else if (state == BGA_STATE.THIRD_APPLY && GetComponent<CompAnimation>().IsAnimOverXTime(third_hit_point) && GetComponent<CompAnimation>().IsAnimationRunning("Attack"))
        {
            state = BGA_STATE.POST_APPLY;
            Debug.Log("Third crush...");
            GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetDamage(damage);
            GetLinkedObject("player_obj").GetComponent<CharactersManager>().SetCurrentCharacterState(CharacterController.State.IDLE);
        }
        else if ((state == BGA_STATE.POST_APPLY && GetComponent<CompAnimation>().IsAnimationStopped("Attack")) || 
            (state == BGA_STATE.GRAB_FAILED && GetComponent<CompAnimation>().IsAnimationStopped("GrabFailed")))
        {
            state = BGA_STATE.WAITING;
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return false;
    }
}