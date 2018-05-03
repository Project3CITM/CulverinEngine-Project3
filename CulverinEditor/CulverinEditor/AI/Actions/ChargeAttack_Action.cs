using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;

public class ChargeAttack_Action : Action
{
    PathNode objective;
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
    GameObject player;

    public ChargeAttack_Action()
    {
        action_type = ACTION_TYPE.CHARGE_ATTACK_ACTION;
    }

    void Start()
    {
        objective = new PathNode(0,0);
        speed = velocity * GetComponent<Movement_Action>().tile_size;
        player = GetLinkedObject("player_obj");
        trans = gameObject.GetComponent<Transform>();
    }

    public override bool ActionStart()
    {
        player.GetComponent<MovementController>().GetPlayerPos(out player_x, out player_y);
        float player_transform_x = player.GetComponent<Transform>().position.x;
        float player_transform_z = player.GetComponent<Transform>().position.z;
        objective = new PathNode(player_x, player_y);

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
        GetComponent<CompAnimation>().SetTransition("ToChargeAttack");
        GetComponent<CompAnimation>().SetClipDuration("ChargeAttack", duration);

        return true;
    }

    // Update is called once per frame
    public override ACTION_RESULT ActionUpdate()
    {
        Vector3 pos = new Vector3(trans.position);
        pos.x = pos.x + speed_x * Time.deltaTime;
        pos.z = pos.z + speed_z * Time.deltaTime;
        trans.position = pos;

        movement_x -= Mathf.Abs(speed_x) * Time.deltaTime;
        movement_z -= Mathf.Abs(speed_z) * Time.deltaTime;

        player.GetComponent<MovementController>().GetPlayerPos(out player_x, out player_y);
        float tile_size = GetComponent<Movement_Action>().tile_size;
        uint current_x = (uint)(pos.x / tile_size) + 1;
        uint current_y = (uint)(pos.z / tile_size) + 1;

        if (current_x == player_x && current_y == player_y)
        {
            Debug.Log("Push");
            if (player.GetComponent<CharactersManager>().Push(damage, GetPushTile()) == true)
                GetComponent<CompAudio>().PlayEvent("SwordHit");
        }

        if (movement_z <= 0.0f && movement_x <= 0.0f)
        {
            Vector3 final_pos = new Vector3(trans.position);
            final_pos.x = objective.GetTileX() * tile_size;
            final_pos.z = objective.GetTileY() * tile_size;
            trans.position = final_pos;
         
            GetComponent<Movement_Action>().tile.SetCoords(objective.GetTileX(), objective.GetTileY());
            GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
        }

        if (GetComponent<CompAnimation>().IsAnimationStopped("ChargeAttack"))
            return ACTION_RESULT.AR_SUCCESS;

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        GetComponent<CompAnimation>().SetTransition("ToIdleAttack");
        return true;
    }

    float GetDuration()
    {
        Vector3 distance = new Vector3(player.transform.position - transform.position);
        return distance.Length / speed;
    }

    private PathNode GetPushTile()
    {
        Vector3 player_boss_vec = new Vector3(player.GetComponent<Transform>().position - trans.position);

        float delta = Mathf.Atan2(player_boss_vec.x, player_boss_vec.z);

        if (delta > Mathf.PI)
            delta = delta - 2 * Mathf.PI;
        if (delta < (-Mathf.PI))
            delta = delta + 2 * Mathf.PI;

        delta = Mathf.Rad2deg(delta);

        Pathfinder pf = GetLinkedObject("map").GetComponent<Pathfinder>();
        PathNode ret = new PathNode(-1, -1);

        //Charging from North
        if (delta > -50.0f && delta < 50.0f)
        {
            ret = new PathNode(player_x, player_y + 1);

            if (pf.IsWalkableTile(ret) == false)
            {
                if (delta < 0.0)
                {
                    ret = new PathNode(player_x - 1, player_y);
                    if (pf.IsWalkableTile(ret) == false)
                        ret = new PathNode(player_x + 1, player_y);
                }
                else
                {
                    ret = new PathNode(player_x + 1, player_y);
                    if (pf.IsWalkableTile(ret) == false)
                        ret = new PathNode(player_x - 1, player_y);
                }
            }
        }
        //Charging from West
        else if (delta > 50.0f && delta < 140.0f)
        {
            ret = new PathNode(player_x - 1, player_y);

            if (pf.IsWalkableTile(ret) == false)
            {
                if (delta < 90.0)
                {
                    ret = new PathNode(player_x, player_y - 1);
                    if (pf.IsWalkableTile(ret) == false)
                        ret = new PathNode(player_x, player_y + 1);
                }
                else
                {
                    ret = new PathNode(player_x, player_y + 1);
                    if (pf.IsWalkableTile(ret) == false)
                        ret = new PathNode(player_x, player_y - 1);
                }
            }
        }
        //Charging from East
        else if (delta > -140.0f && delta < -50.0f)
        {
            ret = new PathNode(player_x + 1, player_y);

            if (pf.IsWalkableTile(ret) == false)
            {
                if (delta < -90.0)
                {
                    ret = new PathNode(player_x, player_y + 1);
                    if (pf.IsWalkableTile(ret) == false)
                        ret = new PathNode(player_x - 1, player_y - 1);
                }
                else
                {
                    ret = new PathNode(player_x - 1, player_y - 1);
                    if (pf.IsWalkableTile(ret) == false)
                        ret = new PathNode(player_x + 1, player_y + 1);
                }
            }
        }
        //Charging from South
        else if (delta < -140.0f || delta > 140.0f)
        {
            ret = new PathNode(player_x, player_y - 1);

            if (pf.IsWalkableTile(ret) == false)
            {
                if (delta < 0.0)
                {
                    ret = new PathNode(player_x + 1, player_y);
                    if (pf.IsWalkableTile(ret) == false)
                        ret = new PathNode(player_x - 1, player_y);
                }
                else
                {
                    ret = new PathNode(player_x - 1, player_y);
                    if (pf.IsWalkableTile(ret) == false)
                        ret = new PathNode(player_x + 1, player_y);
                }
            }
        }
        return ret;
    }

    private void PushPlayer(PathNode tile)
    {
        //player.GetComponent<MovementController>().MoveTo(tile);
        /*
        Vector3 target_pos = new Vector3(0.0f, 0.0f, 0.0f);
        target_pos.x = tile.GetTileX() * GetComponent<Movement_Action>().tile_size;
        target_pos.z = tile.GetTileY() * GetComponent<Movement_Action>().tile_size;

        Vector3 forward = new Vector3(player.GetComponent<Transform>().GetForwardVector());
        Vector3 pos = new Vector3(player.GetComponent<Transform>().position);
        Vector3 obj_vec = new Vector3(target_pos - pos);

        float delta = Vector3.AngleBetweenXZ(forward, obj_vec);

        if (delta > Mathf.PI)
            delta = delta - 2 * Mathf.PI;
        if (delta < (-Mathf.PI))
            delta = delta + 2 * Mathf.PI;

        delta = Mathf.Rad2deg(delta);

        if (delta > -50.0f && delta < 50.0f)
            player.GetComponent<MovementController>().MoveForward();
        else if (delta > -140.0f && delta < -50.0f)
        {
        }
        else if (delta > 50.0f && delta < 140.0f)
        {
        }
        else if (delta < -140.0f || delta > 140.0f)
        {
        }*/
    }
}
