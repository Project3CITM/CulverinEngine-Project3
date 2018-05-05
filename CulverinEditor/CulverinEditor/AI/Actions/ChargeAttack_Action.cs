using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;

public class ChargeAttack_Action : Action
{
    PathNode objective;
    PathNode origin;
    float movement_x = 0.0f;
    float movement_z = 0.0f;
    float speed_x = 0.0f;
    float speed_z = 0.0f;
    public float velocity = 1.0f;
    public float attack_duration = 1.0f;
    public float damage = 1.0f;
    float speed = 1.0f;
    public float after_cd = 1.0f;
    Transform trans;
    float timer = 1.0f;
    int player_x = -1;
    int player_y = -1;
    public GameObject player = null;
    CharactersManager player_manager = null;
    bool pushed = false;
    public float charge_attack_start_point = 0.9f;

    enum Charge_Phase
    {
        CP_CHARGE,
        CP_ATTACK
    }

    Charge_Phase phase = Charge_Phase.CP_CHARGE;

    public ChargeAttack_Action()
    {
        action_type = ACTION_TYPE.CHARGE_ATTACK_ACTION;
    }

    void Start()
    {
        objective = new PathNode(0,0);
        speed = velocity * GetComponent<Movement_Action>().tile_size;
        player = GetLinkedObject("player");
        player_manager = player.GetComponent<CharactersManager>();
        trans = gameObject.GetComponent<Transform>();
    }

    public override bool ActionStart()
    {
        player.GetComponent<MovementController>().GetPlayerPos(out player_x, out player_y);
        float player_transform_x = player.GetComponent<Transform>().position.x;
        float player_transform_z = player.GetComponent<Transform>().position.z;
        objective = new PathNode(player_x, player_y);

        origin = GetComponent<Movement_Action>().GetCurrentTile();        

        movement_x = player_transform_x - trans.position.x;
        movement_z = player_transform_z - trans.position.z;

        float x_factor = movement_x / (movement_x + movement_z);
        float z_factor = movement_z / (movement_x + movement_z);

        speed_x = speed * x_factor;
        speed_z = speed * z_factor;

        if (movement_x < 0.0f)
            speed_x *= -1;
        if (movement_z < 0.0f)
            speed_z *= -1;

        movement_x = Mathf.Abs(movement_x);
        movement_z = Mathf.Abs(movement_z);

        //Set Anim Duration
        float duration = GetDuration();
        GetComponent<CompAnimation>().SetTransition("ToCharge");

        pushed = false;

        phase = Charge_Phase.CP_CHARGE;

        return true;
    }

    // Update is called once per frame
    public override ACTION_RESULT ActionUpdate()
    {
        if (player_manager.dying)
        {
            Debug.Log("DON'T ATTACK PLAYER", Department.PLAYER, Color.YELLOW);
            return ACTION_RESULT.AR_FAIL; //Player is dead, don't attack
        }

        switch (phase)
        {
            case Charge_Phase.CP_CHARGE:
                Vector3 pos = new Vector3(trans.position);
                pos.x = pos.x + speed_x * Time.deltaTime;
                pos.z = pos.z + speed_z * Time.deltaTime;
                trans.position = pos;

                movement_x -= Mathf.Abs(speed_x) * Time.deltaTime;
                movement_z -= Mathf.Abs(speed_z) * Time.deltaTime;

                player.GetComponent<MovementController>().GetPlayerPos(out player_x, out player_y);
                float tile_size = GetComponent<Movement_Action>().tile_size;
                uint current_x = (uint)(pos.x / tile_size);
                uint current_y = (uint)(pos.z / tile_size);

                if (current_x == player_x && current_y == player_y && pushed == false)
                {
                    PathNode push_tile = GetPushTile();
                    Debug.Log("Pushing player from: " + current_x + "," + current_y + " to " + push_tile.GetTileX() + "," + push_tile.GetTileY());
                    Debug.Log("Boss ends charge in: " + objective.GetTileX() + "," + objective.GetTileY());                    
                    pushed = true;
                    if (player.GetComponent<CharactersManager>().Push(damage, push_tile) == true)
                        GetComponent<CompAudio>().PlayEvent("SwordHit");
                }

                if (movement_z <= 0.0f && movement_x <= 0.0f)
                {
                    Vector3 final_pos = new Vector3(trans.position);
                    final_pos.x = objective.GetTileX() * tile_size;
                    final_pos.z = objective.GetTileY() * tile_size;
                    trans.position = final_pos;

                    GetComponent<Movement_Action>().tile.SetCoords(objective.GetTileX(), objective.GetTileY());
                    GetComponent<CompAnimation>().SetTransition("ToChargeAttack");
                    phase = Charge_Phase.CP_ATTACK;
                }
                break;

            case Charge_Phase.CP_ATTACK:
                if (GetComponent<CompAnimation>().IsAnimationStopped("ChargeAttack"))
                    return ACTION_RESULT.AR_SUCCESS;
                break;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
        pushed = false;
        return true;
    }

    float GetDuration()
    {
        Vector3 distance = new Vector3(player.transform.position - transform.position);
        return distance.Length / speed;
    }

    private PathNode GetPushTile()
    {
        int vec_x = player_x - origin.GetTileX();
        int vec_y = player_y - origin.GetTileY();

        Debug.Log("player-Boss vec: " + vec_x + "," + vec_y);

        float delta = Mathf.Atan2(vec_y, vec_x);

        /*if (delta > Mathf.PI)
            delta = delta - 2 * Mathf.PI;
        if (delta < (-Mathf.PI))
            delta = delta + 2 * Mathf.PI;*/

        delta = Mathf.Rad2deg(delta);

        Debug.Log("Delta: " + delta);

        Pathfinder pf = GetLinkedObject("map").GetComponent<Pathfinder>();
        PathNode ret = new PathNode(-1, -1);
         
        //Charging from North
        if (delta > -50.0f && delta < 50.0f)
        {
            ret = new PathNode(player_x, player_y + 1);

            if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
            {
                if (delta < 0.0)
                {
                    ret = new PathNode(player_x - 1, player_y);
                    if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
                        ret = new PathNode(player_x + 1, player_y);
                }
                else
                {
                    ret = new PathNode(player_x + 1, player_y);
                    if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
                        ret = new PathNode(player_x - 1, player_y);
                }
            }
        }
        //Charging from East
        else if (delta > 50.0f && delta < 140.0f)
        {
            ret = new PathNode(player_x + 1, player_y);

            if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
            {
                if (delta < 90.0)
                {
                    ret = new PathNode(player_x, player_y - 1);
                    if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
                        ret = new PathNode(player_x, player_y + 1);
                }
                else
                {
                    ret = new PathNode(player_x, player_y + 1);
                    if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
                        ret = new PathNode(player_x, player_y - 1);
                }
            }
        }
        //Charging from West
        else if (delta > -140.0f && delta < -50.0f)
        {
            ret = new PathNode(player_x - 1, player_y);

            if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
            {
                if (delta < -90.0)
                {
                    ret = new PathNode(player_x, player_y + 1);
                    if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
                        ret = new PathNode(player_x, player_y - 1);
                }
                else
                {
                    ret = new PathNode(player_x, player_y - 1);
                    if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
                        ret = new PathNode(player_x, player_y + 1);
                }
            }
        }
        //Charging from South
        else if (delta < -140.0f || delta > 140.0f)
        {
            ret = new PathNode(player_x, player_y - 1);

            if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
            {
                if (delta < 0.0)
                {
                    ret = new PathNode(player_x + 1, player_y);
                    if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
                        ret = new PathNode(player_x - 1, player_y);
                }
                else
                {
                    ret = new PathNode(player_x - 1, player_y);
                    if (pf.IsWalkableTile(ret) == false || (ret.GetTileX() == objective.GetTileX() && ret.GetTileY() == objective.GetTileY()))
                        ret = new PathNode(player_x + 1, player_y);
                }
            }
        }
        return ret;
    }
}
